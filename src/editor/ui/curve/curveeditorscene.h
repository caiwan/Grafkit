#if 0

#pragma once 

#include <QtGui>

#include "curvepointitem.h"

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

#endif
