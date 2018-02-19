#pragma once 
#include <QtGui>
#include <qgraphicsitem.h>

namespace Idogep {
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
		QPointF m_coord, m_tangent;
		float m_radix, m_radix2;

		bool m_showTangent;
	};

}
