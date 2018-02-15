/***************************************************************************
 *   Copyright (C) 2012                                                    *
 *   Anatole Duprat <anatole.duprat@gmail.com>                             *
 *   Charles Bulckaen  <xtrium@frequency.fr>                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public License           *
 *   as published by the Free Software Foundation; either version 2        *
 *   of the License, or (at your option) any later version.                *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA          *
 ***************************************************************************/


#if 0


#include <cmath>
#include "curvepointitem.hh"

#include "curveeditorscene.hh"

CurvePointItem::CurvePointItem(QPointF coord, QPointF tangent, QGraphicsItem* parent) : QGraphicsItem(parent)
{
    _coord = QPointF(coord);
    _tangent = QPointF(tangent);
    _showTangent = false;

    _radix = _radix2 = 1.0f/8.0f;

    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
}

CurvePointItem::CurvePointItem(const CurvePointItem& cpi) : QGraphicsItem(cpi.parentItem())
{
    _coord = QPointF(cpi._coord);
    _tangent = QPointF(cpi._tangent);
    _showTangent = false;

    _radix = cpi._radix;
    _radix2 = 1.0f/8.0f;

    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
}

CurvePointItem::~CurvePointItem() {
}

QRectF CurvePointItem::boundingRect() const {
    return QRectF(-_radix2*32.0f, -_radix2*32.0f, _radix2*64.0f, _radix2*64.0f);
}

void CurvePointItem::recalculatePosition() {
    CurveEditorScene* ces = (CurveEditorScene*)scene();
    if(_coord.x() < 0.0f) _coord.setX(0.0f);
    setPos(_coord.x() * ces->scale().width() + ces->offset().x(), _coord.y() * -ces->scale().height() + ces->offset().y());
}

void CurvePointItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    painter->setRenderHint(QPainter::Antialiasing);

    if(_radix == 0.0f) _radix = 0.1f;

    painter->setPen(Qt::NoPen);
    if(isSelected()) painter->setBrush(QColor(224,224,255));
    else painter->setBrush(QColor(224,224,224,144));
    painter->drawEllipse(QPoint(0, 0), 4, 4);

    if(_showTangent) {
        painter->setPen(QPen(QColor(255, 48, 48), 1.0, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        CurveEditorScene* ces = (CurveEditorScene*)scene();
        float ScreenTangentX = _tangent.x() * ces->scale().width();
        float ScreenTangentY = -_tangent.y() * ces->scale().height();
        float n = sqrt(ScreenTangentX*ScreenTangentX + ScreenTangentY*ScreenTangentY);
        QPointF vTgt = QPointF(ScreenTangentX / n, ScreenTangentY / n);
        painter->drawLine(QPointF(0, 0), vTgt*32.0f*_radix);

        painter->setPen(QPen(QColor(96,96,96), 1.0, Qt::DotLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(QPointF(0, 0), 32.0f*_radix, 32.0f*_radix);
    }
}

QVariant CurvePointItem::itemChange(GraphicsItemChange change, const QVariant &value) {
    return QGraphicsItem::itemChange(change, value);
}

float CurvePointItem::time() const {
    return _coord.x();
}

void CurvePointItem::setTime(float t) {
    _coord.setX(t);
}

float CurvePointItem::value() const {
    return _coord.y();
}

void CurvePointItem::setValue(float v) {
    _coord.setY(v);
}

QPointF CurvePointItem::coord() const {
    return _coord;
}

void CurvePointItem::setCoord(QPointF c) {
    _coord = QPointF(c);
}

QPointF CurvePointItem::tangent() const {
    return _tangent;
}

void CurvePointItem::setTangent(QPointF t) {
    _tangent = QPointF(t);
}

void CurvePointItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    switch(event->button()) {
    case Qt::RightButton:
        _showTangent = true;
        _radix2 = _radix;
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
        _showTangent = false;
        _radix2 = 1.0f/8.0f;
        break;
    default:
        QGraphicsItem::mouseReleaseEvent(event);
        break;
    }
    scene()->update();
}

void CurvePointItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    CurveEditorScene* ces = (CurveEditorScene*)scene();
    if(_showTangent) {
        _radix  = sqrt(event->pos().x()*event->pos().x() + event->pos().y()*event->pos().y()) / 32.0f;
        if(_radix == 0.0f) _radix = 0.01f;
        _radix2 = _radix;

        float angle = atan2(event->pos().y(), event->pos().x());
        float epsilon = 0.005f;
        if(angle < -M_PI / 2.0f + epsilon) angle = -M_PI / 2.0f + epsilon;
        else if(angle > M_PI / 2.0f - epsilon) angle = M_PI / 2.0f - epsilon;
        _tangent.setX(cos(angle)*_radix / ces->scale().width());
        _tangent.setY(-sin(angle)*_radix / ces->scale().height());
    } else {
        _coord = QPointF(
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
    if(_showTangent) {
        _radix  = sqrt(event->pos().x()*event->pos().x() + event->pos().y()*event->pos().y()) / 32.0f;
        if(_radix == 0.0f) _radix = 0.01f;
        _radix2 = _radix;

        float angle = atan2(event->pos().y(), event->pos().x());
        float epsilon = 0.005f;
        if(angle < -M_PI / 2.0f + epsilon) angle = -M_PI / 2.0f + epsilon;
        else if(angle > M_PI / 2.0f - epsilon) angle = M_PI / 2.0f - epsilon;
        _tangent.setX(cos(angle)*_radix / ces->scale().width());
        _tangent.setY(sin(angle)*_radix / ces->scale().height());
    } else {
        _coord = QPointF(
            (event->scenePos().x()-ces->offset().x()) / ces->scale().width(),
            (event->scenePos().y()-ces->offset().y()) / -ces->scale().height()
        );
    }

    scene()->update();
}

void CurvePointItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) {
    CurveEditorScene* ces = (CurveEditorScene*)scene();
    ces->deleteCurvePoint(this);
}

#endif
