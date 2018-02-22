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

		void documentChanged(CurveDocument * doc);

		virtual void drawBackground(QPainter* painter, const QRectF& r);
		virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
		virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
		virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
		void viewResized(QResizeEvent *event);

		//void deleteCurvePoint(CurvePointItem* cpi);

		//void setCurrentCurve(QList<CurvePointItem*>* curve);
		//void setDemoTime(float t);
		//void setFollowTimeBar(bool b);
		//void setDisplayWaveform(bool b);
		//void getNewAudiogram();

		//float demoTime() const;
		//QList<CurvePointItem*>* curve() const;

		float simpleInterpolate(QList<CurvePointItem*>* list, float t);

		//void cutSelectedPoints();
		//void copySelectedPoints();
		//void pasteSelectedPoints();

	private:
		//QList<CurvePointItem*>* m_pointItems;
		//QList<CurvePointItem*>* m_curCurve;

		CurveDocument* m_document;

		QSizeF m_scale;

		CurveEditorWidget* m_widget;

		QPointF m_ofs;
		bool m_modifyOfs, m_modifyScale, m_modifyDemoTime;
		bool m_followTimeBar, m_displayWaveform;

		float m_demoTime, m_demoTimeChanged;

		static QPointF _interpolateHermite(QPointF p0, QPointF p1, QPointF r0, QPointF r1, float t);
		void requestAudiogram();

		QImage* m_audiogramImage;
	};

}
