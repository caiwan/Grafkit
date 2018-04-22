#pragma once 
#include <QtGui>
#include <qgraphicsitem.h>

#include "animation/animation.h"
#include "utils/Event.h"


namespace Idogep {
	class CurvePointItem : public QGraphicsItem
	{
	public:
		CurvePointItem(QGraphicsItem* parent = NULL);
		CurvePointItem(Grafkit::Animation::Key key,  size_t index, QGraphicsItem* parent = NULL);
		CurvePointItem(const CurvePointItem& other);

		~CurvePointItem();
		
		QRectF boundingRect() const;
		void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

		void recalculatePosition();

		QVariant itemChange(GraphicsItemChange change, const QVariant &value);

		// TODO: refactor rendesen ezeket:
		float time() const { return m_key.m_time; }
		void setTime(float t) { m_key.m_time = t; }
		float value() const { return m_key.m_value; }
		void setValue(float v) { m_key.m_value = v; }

		QPointF coord() const { return QPointF(m_key.m_time, m_key.m_value); }
		void setCoord(QPointF c) { m_key.m_time = c.x(); m_key.m_value = c.y(); }

		QPointF tangent() const { return QPointF(m_key.m_tangent.x, m_key.m_tangent.y); }
		void setTangent(QPointF t) { m_key.m_tangent.x = t.x(); m_key.m_tangent.y = t.y(); }
		// --- 

		Grafkit::Animation::Key GetKey() const { return m_key; }
		void SetKey(Grafkit::Animation::Key key) {m_key = key;}

		// ... 

		uint32_t index() { return m_id; }
		void setIndex(uint32_t id) { m_id = id; }

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

		//virtual void keyPressEvent(QKeyEvent * event);
		//virtual void keyReleaseEvent(QKeyEvent * event);

		void editTangent(QGraphicsSceneMouseEvent* event);
		void editPosition(QGraphicsSceneMouseEvent* event);

		void toggleTangentEditing();

	private:
		//QPointF m_coord, m_tangent;
		float m_radix, m_radix2;

		bool m_showTangent;

		uint32_t m_id;

		Grafkit::Animation::Key m_key;

		uint32_t m_nodeType;
		uint32_t m_color;
	};

}
