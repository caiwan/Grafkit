#pragma once 

#include <QtGui>
#include <qgraphicsscene.h>

#include "curvepointitem.h"
#include "CurveSceneView.h"

namespace Idogep {

	class ManageCurveRole;

	class CurvePointItem;
	class CurveEditorWidget;

	class TimelineArea;

	class CurveEditorScene : public QGraphicsScene, public CurveSceneView
	{
	public:
		CurveEditorScene(QObject* parent = nullptr);
		~CurveEditorScene();

		void MusicChanged() override;
		void PlaybackChanged(bool isPlaying) override;
		void DemoTimeChanged(float time) override;

	protected:
		void RefreshView(bool force) override;

		// QT stuff

	public:
		void drawBackground(QPainter* painter, const QRectF& r) override;
		//virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
		//virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
		//virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
		//virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

	private:
		TimelineArea * m_area;
	};


	class TimelineArea
	{
	public:

		TimelineArea();

	    QSizeF Scale() const
	    {
	        return m_scale;
	    }

	    void SetScale(const QSizeF& scale)
	    {
	        m_scale = scale;
	    }

	    QPointF Offset() const
	    {
	        return m_offset;
	    }

	    void SetOffset(const QPointF& offset)
	    {
	        m_offset = offset;
	    }

	    QRectF SceneRect() const
	    {
	        return m_sceneRect;
	    }

	    void SetSceneRect(const QRectF& sceneRect)
	    {
	        m_sceneRect = sceneRect;
	    }

	    // ReSharper disable CppInconsistentNaming
		QPointF point2Screen(QPointF point) const;
		QPointF screen2Point(QPointF point) const;

		void drawGrid(QPainter* painter, const QRectF& r);
	    // ReSharper restore CppInconsistentNaming

	private:

		QSizeF m_scale;
		QPointF m_offset;

		QRectF m_sceneRect;

	};

#if 0
	class CurveEditorScene : public QGraphicsScene
	{
		Q_OBJECT
	public:
		CurveEditorScene(CurveEditorWidget* pWidget, QObject* parent = NULL);

		QSizeF scale() const;
		QPointF offset() const;

		QPointF point2Screen(QPointF point) const;
		QPointF screen2Point(QPointF point) const;

		void SetDocument(ManageCurveRole * doc);

		virtual void drawBackground(QPainter* painter, const QRectF& r);
		virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
		virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
		virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

		void viewResized(QResizeEvent *event) { updateAudiogram(); }

		void RefreshView() { updateAudiogram(); update(); }

	private:
		void drawCurve(QPainter* painter, const QRectF& r);
		//void drawCurve_old(QPainter* painter, const QRectF& r);

		// TODO: kivenni innnen mashova
		void drawCursor(QPainter* painter, const QRectF& r);

		ManageCurveRole* m_document;

		QSizeF m_scale;

		CurveEditorWidget* m_widget;

		QPointF m_ofs;
		bool m_modifyOfs, m_modifyScale, m_modifyDemoTime;
		bool m_followTimeBar, m_displayWaveform;

		float m_demoTime, m_demoTimeChanged;

		//static QPointF _interpolateHermite(QPointF p0, QPointF p1, QPointF r0, QPointF r1, float t);
		void updateAudiogram();

		QImage* m_audiogramImage;
	};
#endif 
}
