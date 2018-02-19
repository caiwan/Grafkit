#include <cmath>

#include "qgraphicsview.h"
#include "qgraphicsscene.h"
#include "qgraphicssceneevent.h"

#include "curvepointitem.h"
#include "curveeditorscene.h"

using namespace Idogep;

CurvePointItem::CurvePointItem(QPointF coord, QPointF tangent, QGraphicsItem* parent) : QGraphicsItem(parent)
{
    m_coord = QPointF(coord);
    m_tangent = QPointF(tangent);
    m_showTangent = false;

    m_radix = m_radix2 = 1.0f/8.0f;

    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
}

CurvePointItem::CurvePointItem(const CurvePointItem& cpi) : QGraphicsItem(cpi.parentItem())
{
    m_coord = QPointF(cpi.m_coord);
    m_tangent = QPointF(cpi.m_tangent);
    m_showTangent = false;

    m_radix = cpi.m_radix;
    m_radix2 = 1.0f/8.0f;

    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
}

CurvePointItem::~CurvePointItem() {
}

QRectF CurvePointItem::boundingRect() const {
    return QRectF(-m_radix2*32.0f, -m_radix2*32.0f, m_radix2*64.0f, m_radix2*64.0f);
}

void CurvePointItem::recalculatePosition() {
    CurveEditorScene* ces = (CurveEditorScene*)scene();
    if(m_coord.x() < 0.0f) m_coord.setX(0.0f);
    setPos(m_coord.x() * ces->scale().width() + ces->offset().x(), m_coord.y() * -ces->scale().height() + ces->offset().y());
}

void CurvePointItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    painter->setRenderHint(QPainter::Antialiasing);

    if(m_radix == 0.0f) m_radix = 0.1f;

    painter->setPen(Qt::NoPen);
    if(isSelected()) painter->setBrush(QColor(224,224,255));
    else painter->setBrush(QColor(224,224,224,144));
    painter->drawEllipse(QPoint(0, 0), 4, 4);

    if(m_showTangent) {
        painter->setPen(QPen(QColor(255, 48, 48), 1.0, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        CurveEditorScene* ces = (CurveEditorScene*)scene();
        float ScreenTangentX = m_tangent.x() * ces->scale().width();
        float ScreenTangentY = -m_tangent.y() * ces->scale().height();
        float n = sqrt(ScreenTangentX*ScreenTangentX + ScreenTangentY*ScreenTangentY);
        QPointF vTgt = QPointF(ScreenTangentX / n, ScreenTangentY / n);
        painter->drawLine(QPointF(0, 0), vTgt*32.0f*m_radix);

        painter->setPen(QPen(QColor(96,96,96), 1.0, Qt::DotLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(QPointF(0, 0), 32.0f*m_radix, 32.0f*m_radix);
    }
}

QVariant CurvePointItem::itemChange(GraphicsItemChange change, const QVariant &value) {
    return QGraphicsItem::itemChange(change, value);
}

float CurvePointItem::time() const {
    return m_coord.x();
}

void CurvePointItem::setTime(float t) {
    m_coord.setX(t);
}

float CurvePointItem::value() const {
    return m_coord.y();
}

void CurvePointItem::setValue(float v) {
    m_coord.setY(v);
}

QPointF CurvePointItem::coord() const {
    return m_coord;
}

void CurvePointItem::setCoord(QPointF c) {
    m_coord = QPointF(c);
}

QPointF CurvePointItem::tangent() const {
    return m_tangent;
}

void CurvePointItem::setTangent(QPointF t) {
    m_tangent = QPointF(t);
}

void CurvePointItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    switch(event->button()) {
    case Qt::RightButton:
        m_showTangent = true;
        m_radix2 = m_radix;
        break;
    default:
        QGraphicsItem::mousePressEvent(event);
        break;
    }
    scene()->update();
}

void CurvePointItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    switch(event->button()) {
    case Qt::RightButton:
        m_showTangent = false;
        m_radix2 = 1.0f/8.0f;
        break;
    default:
        QGraphicsItem::mouseReleaseEvent(event);
        break;
    }
    scene()->update();
}

void CurvePointItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    CurveEditorScene* ces = (CurveEditorScene*)scene();
    if(m_showTangent) {
        m_radix  = sqrt(event->pos().x()*event->pos().x() + event->pos().y()*event->pos().y()) / 32.0f;
        if(m_radix == 0.0f) m_radix = 0.01f;
        m_radix2 = m_radix;

        float angle = atan2(event->pos().y(), event->pos().x());
        float epsilon = 0.005f;
        if(angle < -M_PI / 2.0f + epsilon) angle = -M_PI / 2.0f + epsilon;
        else if(angle > M_PI / 2.0f - epsilon) angle = M_PI / 2.0f - epsilon;
        m_tangent.setX(cos(angle)*m_radix / ces->scale().width());
        m_tangent.setY(-sin(angle)*m_radix / ces->scale().height());
    } else {
        m_coord = QPointF(
            (event->scenePos().x()-ces->offset().x()) / ces->scale().width(),
            (event->scenePos().y()-ces->offset().y()) / -ces->scale().height()
        );
    }

    foreach(QGraphicsItem* it, ces->selectedItems())
    {
        CurvePointItem* cpit = dynamic_cast<CurvePointItem*>(it);
        if(cpit != NULL && cpit != this)
        {
            QPointF delta = cpit->pos() - pos();
            QPointF fakeMousePos = event->pos() + delta;
            QPointF fakeMouseScenePos = event->scenePos() + delta;
            QGraphicsSceneMouseEvent fakeEvent(QEvent::GraphicsSceneMouseMove);
            fakeEvent.setPos(fakeMousePos);
            fakeEvent.setScenePos(fakeMouseScenePos);
            cpit->fakeMouseMoveEvent(&fakeEvent);
        }
    }

    scene()->update();
    QGraphicsItem::mouseMoveEvent(event);
}

void CurvePointItem::fakeMouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    CurveEditorScene* ces = (CurveEditorScene*)scene();
    if(m_showTangent) {
        m_radix  = sqrt(event->pos().x()*event->pos().x() + event->pos().y()*event->pos().y()) / 32.0f;
        if(m_radix == 0.0f) m_radix = 0.01f;
        m_radix2 = m_radix;

        float angle = atan2(event->pos().y(), event->pos().x());
        float epsilon = 0.005f;
        if(angle < -M_PI / 2.0f + epsilon) angle = -M_PI / 2.0f + epsilon;
        else if(angle > M_PI / 2.0f - epsilon) angle = M_PI / 2.0f - epsilon;
        m_tangent.setX(cos(angle)*m_radix / ces->scale().width());
        m_tangent.setY(sin(angle)*m_radix / ces->scale().height());
    } else {
        m_coord = QPointF(
            (event->scenePos().x()-ces->offset().x()) / ces->scale().width(),
            (event->scenePos().y()-ces->offset().y()) / -ces->scale().height()
        );
    }

    scene()->update();
}

void CurvePointItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) {
    CurveEditorScene* ces = (CurveEditorScene*)scene();
    //ces->deleteCurvePoint(this);
}
