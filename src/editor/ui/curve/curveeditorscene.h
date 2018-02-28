#pragma once 

#include <QtGui>
#include <qgraphicsscene.h>

#include "curvepointitem.h"

namespace Idogep {

	class CurveDocument;

	class CurvePointItem;
	class CurveEditorWidget;

	class CurveEditorScene : public QGraphicsScene
	{
		Q_OBJECT
	public:
		CurveEditorScene(CurveEditorWidget* pWidget, QObject* parent = NULL);

		QSizeF scale() const;
		QPointF offset() const;

		QPointF point2Screen(QPointF point) const;
		QPointF screen2Point(QPointF point) const;

		void documentChanged(CurveDocument * doc);

		virtual void drawBackground(QPainter* painter, const QRectF& r);
		virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
		virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
		virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
		void viewResized(QResizeEvent *event);

	private:
		
		void drawCurve(QPainter* painter, const QRectF& r);
		void drawCurve_old(QPainter* painter, const QRectF& r);

		void drawCursor(QPainter* painter, const QRectF& r);

		CurveDocument* m_document;

		QSizeF m_scale;

		CurveEditorWidget* m_widget;

		QPointF m_ofs;
		bool m_modifyOfs, m_modifyScale, m_modifyDemoTime;
		bool m_followTimeBar, m_displayWaveform;

		float m_demoTime, m_demoTimeChanged;

		static QPointF _interpolateHermite(QPointF p0, QPointF p1, QPointF r0, QPointF r1, float t);
		void updateAudiogram();

		QImage* m_audiogramImage;
	};

}
