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

#ifndef CURVEPOINTITEM_HH
#define CURVEPOINTITEM_HH

#include <QtGui>

class CurvePointItem : public QGraphicsItem
{
public:
    CurvePointItem(QPointF coord = QPointF(0.0, 0.0), QPointF tangent = QPointF(1.0, 0.0), QGraphicsItem* parent = NULL);
    CurvePointItem(const CurvePointItem& cpi);
    ~CurvePointItem();
    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    void recalculatePosition();

    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    float time() const;
    void setTime(float t);
    float value() const;
    void setValue(float v);

    QPointF coord() const;
    void setCoord(QPointF c);

    QPointF tangent() const;
    void setTangent(QPointF t);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
            void fakeMouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);

private:
    QPointF _coord, _tangent;
    float _radix, _radix2;

    bool _showTangent;
};

#endif // CURVEPOINTITEM_HH

#endif
