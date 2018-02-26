#pragma once 
#include <QtGui>
#include <qgraphicsitem.h>

#include "Event.h"

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

		uint32_t index() { return m_id; }
		void setIndex(uint32_t id) { m_id = id; }

		uint32_t subIndex() { return m_subid; }
		void setSubId(uint32_t sid) { m_subid = sid; }

		uint32_t nodeType() { return m_nodeType; }
		void setNodeType(uint32_t nt) { m_nodeType = nt; }

		uint32_t color() { return m_color; }
		void setColor(uint32_t color) { m_color = color; }
		
		Event<CurvePointItem*> onMovePoint;
		Event<CurvePointItem*> onMoveTangent;
		Event<CurvePointItem*> onStartEdit;
		Event<CurvePointItem*> onCommitEdit;

	protected:
		virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

		virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
		void fakeMouseMoveEvent(QGraphicsSceneMouseEvent* event);

		virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);

		virtual void keyPressEvent(QKeyEvent * event);
		virtual void keyReleaseEvent(QKeyEvent * event);

		void editTangent(QGraphicsSceneMouseEvent* event);
		void editPosition(QGraphicsSceneMouseEvent* event);

		void toggleTangentEditing();

	private:
		QPointF m_coord, m_tangent;
		float m_radix, m_radix2;

		bool m_showTangent;

		uint32_t m_id;
		uint32_t m_subid;

		uint32_t m_nodeType;
		uint32_t m_color;
	};

}
