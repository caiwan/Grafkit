#include <QGraphicsSceneEvent>
#include <QPainter>

#include "curveeditorscene.h"

#include "CurvePointItem.h"
#include "CurvePointEditor.h"
#include <QKeyEvent>

using namespace Idogep;

using namespace Grafkit;

#define KEY_SET_INTERPOLATION (Qt::Key_I)
#define KEY_SET_TANGENT (Qt::Key_R)

#define KEY_SNAP_X (Qt::Key_Shift)
#define KEY_SNAP_Y (Qt::Key_Alt)

#define BOUNDING_BOX_SCALE 16.
#define RADIX_SCALE 32.

CurvePointItem::CurvePointItem(const Animation::Key key, const size_t index, QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , m_id(index)
    , m_key(key)
    , m_areaScaling(1, 1)
{
    m_color = 0;
    m_nodeType = 0;

    m_showTangent = false;
    m_snapXAxis = false;
    m_snapYAxis = false;

    m_lastRadius = 1.;
    m_lastAngle = 0.;

    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemIsFocusable);

    show();
}

CurvePointItem::~CurvePointItem() = default;

void CurvePointItem::RecalculatePosition(TimelineArea const* area)
{
    if (m_key.m_time < 0.f)
        m_key.m_time = 0.f;
    setPos(area->Point2Screen(GetCoord()));
    m_areaScaling = area->GetScale(); // save last scaling for paint tangent on
}

QRectF CurvePointItem::boundingRect() const { return { -BOUNDING_BOX_SCALE / 2, -BOUNDING_BOX_SCALE / 2, BOUNDING_BOX_SCALE, BOUNDING_BOX_SCALE }; }

void CurvePointItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    const QColor selectedColor = QColor(224, 224, 255);
    const QColor pointColor = QColor(224, 224, 224, 144);

    painter->setRenderHint(QPainter::Antialiasing);

    painter->setPen(Qt::NoPen);
    if (isSelected())
        painter->setBrush(selectedColor);
    else
        painter->setBrush(pointColor);

    painter->drawEllipse(QPoint(0, 0), 4, 4);

    if (isSelected())
    {
        painter->setPen(QPen(selectedColor, 1.0, Qt::SolidLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(QPoint(0, 0), 6, 6);
    }

    if (m_showTangent)
    {
        const float angle = m_key.m_angle;
        const float radix = m_key.m_radius;
        const QPointF tangent = { cos(angle * (.5*M_PI)), -sin(angle* (.5*M_PI)) };

        painter->setPen(QPen(QColor(255, 48, 48), 1.0, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);


        painter->drawLine(QPointF(0, 0), tangent * RADIX_SCALE * radix);

        painter->setPen(QPen(QColor(96, 96, 96), 1.0, Qt::DotLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(QPointF(0, 0), RADIX_SCALE * radix, RADIX_SCALE * radix);
    }
}

QVariant CurvePointItem::itemChange(const GraphicsItemChange change, const QVariant& value) { return QGraphicsItem::itemChange(change, value); }

void CurvePointItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    switch (event->button())
    {
    case Qt::RightButton:
        if (!isSelected())
            return;

        m_showTangent = true;
        m_lastRadius = m_key.m_radius;
        m_lastAngle = m_key.m_angle;
        onStartEdit(m_id, m_key);
        break;

    case Qt::LeftButton:
        onStartEdit(m_id, m_key);
        QGraphicsItem::mousePressEvent(event);
        break;

    default:
        QGraphicsItem::mousePressEvent(event);
        break;
    }
    scene()->update();
}

void CurvePointItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    switch (event->button())
    {
    case Qt::RightButton:
        if (!isSelected())
            return;

        m_showTangent = false;
        onCommitEdit(m_id, m_key);
        break;

    case Qt::LeftButton:
        if (!isSelected())
            return;

        onCommitEdit(m_id, m_key);
        QGraphicsItem::mouseReleaseEvent(event);
        break;

    default:
        QGraphicsItem::mouseReleaseEvent(event);
        break;
    }
    scene()->update();
}

void CurvePointItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (!isSelected())
        return;

    const CurveEditorScene* ces = dynamic_cast<CurveEditorScene*>(scene());

    assert(ces);

    if (m_showTangent) { EditTangent(event); }
    else { EditPosition(event); }

    // Ha tobb elem van kijelolve, akkor azokat lehet mozgatni elvileg egyben
    foreach(QGraphicsItem* it, ces->selectedItems())
    {
        CurvePointItem* cpit = dynamic_cast<CurvePointItem*>(it);
        if (cpit != nullptr && cpit != this)
        {
            const QPointF delta = cpit->pos() - pos();
            const QPointF fakeMousePos = event->pos() + delta;
            const QPointF fakeMouseScenePos = event->scenePos() + delta;
            QGraphicsSceneMouseEvent fakeEvent(QEvent::GraphicsSceneMouseMove);
            fakeEvent.setPos(fakeMousePos);
            fakeEvent.setScenePos(fakeMouseScenePos);
            cpit->FakeMouseMoveEvent(&fakeEvent);
        }
    }

    scene()->update();
    QGraphicsItem::mouseMoveEvent(event);
}

// ezzel lehet a tobbit mozgatni
void CurvePointItem::FakeMouseMoveEvent(class QGraphicsSceneMouseEvent* event)
{
    if (m_showTangent) { EditTangent(event); }
    else { EditPosition(event); }

    scene()->update();
}

void CurvePointItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    //CurveEditorScene* ces = (CurveEditorScene*)scene();
    //ces->deleteCurvePoint(this);
}

void CurvePointItem::keyPressEvent(QKeyEvent * event)
{
    if (isSelected())
    {
        switch (event->key())
        {
            // This thing does not work as it intended to.
        case KEY_SNAP_X:
            m_showTangent = true;
            m_snapXAxis = true; break;

        case KEY_SNAP_Y:
            m_showTangent = true;
            m_snapYAxis = true; break;

        default:
            QGraphicsItem::keyReleaseEvent(event);
            return;
        }

        event->accept();
    }
    else {
        QGraphicsItem::keyReleaseEvent(event);
    }
}

void CurvePointItem::keyReleaseEvent(QKeyEvent * event)
{
    if (isSelected()) {
        switch (event->key())
        {
        case KEY_SNAP_X:
            m_showTangent = false;
            m_snapXAxis = false;
            break;

        case KEY_SNAP_Y:
            m_showTangent = false;
            m_snapYAxis = false;
            break;

            //	case KEY_SET_INTERPOLATION:
            //		// set interpolation, toggle 
            //		event->accept();
            //		break;
            //	case KEY_SET_TANGENT:
            //		toggleTangentEditing();
            //		event->accept();
            //		break;
        default:
            QGraphicsItem::keyReleaseEvent(event);
            return;
        }

        event->accept();
    }
    else {
        QGraphicsItem::keyReleaseEvent(event);
    }
}

void CurvePointItem::EditTangent(class QGraphicsSceneMouseEvent* event)
{
    const float x = event->pos().x();
    const float y = event->pos().y();

    const float radius = sqrt(x * x + y * y) / 32.0f;
    const float angle = atan2(-y, x) / (.5*M_PI); // -1 .. 1

    if (m_snapXAxis)
        m_key.m_radius += radius - m_lastRadius;
    else
        m_key.m_radius = radius;

    if (m_snapYAxis)
        m_key.m_angle += angle - m_lastAngle;
    else
        m_key.m_angle = angle;

    onEditKey(m_id, m_key); // will check and update constraints

    m_lastAngle = angle;
    m_lastRadius = radius;
}

void CurvePointItem::RefreshView(bool force) { scene()->update(); }

void CurvePointItem::EditPosition(class QGraphicsSceneMouseEvent* event)
{
    const CurveEditorScene* ces = dynamic_cast<CurveEditorScene*>(scene());
    assert(ces);
    const TimelineArea* area = ces->GetArea();
    assert(area);

    const QPointF coord = area->Screen2Point(event->scenePos());

    SetCoord(coord);

    onEditKey(m_id, m_key);
}

void CurvePointItem::ToggleTangentEditing()
{
    if (m_showTangent) { m_lastRadius = 1.0f; }
    else { m_lastRadius = m_key.m_radius; }

    m_showTangent = !m_showTangent;
    this->update();
}
