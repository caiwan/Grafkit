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

#ifndef CURVEEDITORSCENE_HH
#define CURVEEDITORSCENE_HH

#include <QtGui>

#include "curvepointitem.hh"

class CurvePointItem;
class CurveEditorWidget;

class CurveEditorScene : public QGraphicsScene
{
    Q_OBJECT
public:
    CurveEditorScene(CurveEditorWidget* pWidget, QObject* parent = NULL);

    QSizeF scale() const;
    QPointF offset() const;

    virtual void drawBackground(QPainter* painter, const QRectF& r);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
            void viewResized();

    void deleteCurvePoint(CurvePointItem* cpi);

    void setCurrentCurve(QList<CurvePointItem*>* curve);
    void setDemoTime(float t);
    void setFollowTimeBar(bool b);
    void setDisplayWaveform(bool b);
    void getNewAudiogram();

    float demoTime() const;
    QList<CurvePointItem*>* curve() const;

    float simpleInterpolate(QList<CurvePointItem*>* list, float t);

    void cutSelectedPoints();
    void copySelectedPoints();
    void pasteSelectedPoints();

private:
    QList<CurvePointItem*>* _pointItems;
    QList<CurvePointItem*>* _curCurve;
    QSizeF _scale;

    CurveEditorWidget* _widget;

    QPointF _ofs;
    bool _modifyOfs, _modifyScale, _modifyDemoTime;
    bool _followTimeBar, _displayWaveform;

    float _demoTime, _demoTimeChanged;

    static QPointF _interpolateHermite(QPointF p0, QPointF p1, QPointF r0, QPointF r1, float t);
    void requestAudiogram();

    QImage* _audiogramImage;
};

#endif // CURVEEDITORSCENE_HH

#endif
