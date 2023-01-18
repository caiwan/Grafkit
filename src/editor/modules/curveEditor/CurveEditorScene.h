#pragma once

#include <qgraphicsscene.h>

#include "curvepointitem.h"
#include "CurveEditor.h"

namespace Idogep
{
    class TimelineArea;
    class AudiogramPainter;
    class CurvePointBuilder;

    class CursorItem;

    class CurveEditorScene : public QGraphicsScene, public CurveEditorView
    {
        Q_OBJECT
    public:
        explicit CurveEditorScene(QObject* parent = nullptr);
        ~CurveEditorScene();

        void MusicChangedEvent() override { UpdateAudiogram(); RequestRefreshView(true); }
        void PlaybackChangedEvent(bool isPlaying) override;
        void DemoTimeChangedEvent(const float &time) override;

        void Invalidate() override { m_isCurveValid = false; }

        void ClearCurvePoints() override;
        void AddCurvePoint(CurvePointItem* pointItem) override;

        const TimelineArea* GetArea() const { return m_area; }

    protected:
        void RefreshView(bool force) override;

        bool event(QEvent* event) override;

        void drawBackground(QPainter* painter, const QRectF& r) override;

        void DrawCurve(QPainter* painter, const QRectF& r);

        void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
        void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
        void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
        void HandleMousePan(QGraphicsSceneMouseEvent * event);
        
        void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

        void wheelEvent(QGraphicsSceneWheelEvent* event) override;

        void keyPressEvent(QKeyEvent* event) override;
        void keyReleaseEvent(QKeyEvent* event) override;

    private slots:
        void SelectionChangedSlot();

    private:
        TimelineArea * m_area;
        CursorItem *m_cursorItem;

        bool m_modifyDemoTime;

        bool m_altPressed;
        bool m_ctrlPressed;
        bool m_shiftPressed;

        bool m_midButton;
        bool m_rightButton;

        bool m_isValidAudiogram;
 
        QPixmap m_curvePixmap;

        void UpdateAudiogram();
        QImage* m_audiogramImage;

        bool m_isCurveValid;
    };


    class TimelineArea
    {
    public:

        TimelineArea();

        QSizeF GetScale() const { return m_scale; }
        void SetScale(const QSizeF& scale) { m_scale = scale; Invalidate(); }

        QPointF GetOffset() const { return m_offset; }
        void SetOffset(const QPointF& offset) { m_offset = offset; Invalidate(); }

        QRectF GetSceneRect() const { return m_sceneRect; }
        void SetSceneRect(const QRectF& sceneRect) { m_sceneRect = sceneRect; Invalidate(); }

        QPointF Point2Screen(QPointF point) const;
        QPointF Screen2Point(QPointF point) const;

        QPointF GetMin() const;
        QPointF GetMax() const;

        void DrawGrid(QPainter*const& painter, const QRectF& r);
        void Pan(const QPointF& p);
        void Zoom(const QPointF& z);
        void ResetView();

    private:
        void Invalidate();
        void DrawGridToPicture(QPainter*const& painter, const QRectF& r) const;

        QPixmap m_gridImage;
        bool m_gridValid;

        QSizeF m_scale;
        QPointF m_offset;

        QTransform m_transfrom; // unit -> screen
        QTransform m_inverse;   // screen -> unit

        QRectF m_sceneRect;

    };

    class CursorItem
    {
    public:
        CursorItem();

        void DrawCursor(QPainter*const& painter, const QRectF& r);

        float GetPosition() const { return m_position; }
        void SetPosition(const float position) { m_position = position; }

        void SetArea(TimelineArea* const area) { m_area = area; }

    private:
        float m_position;
        TimelineArea *m_area;
    };

}
