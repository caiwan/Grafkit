#include <QGraphicsSceneEvent>
#include <QPainter>

#include "curveeditorscene.h"

#include "curvepointitem.h"

using namespace Idogep;

using namespace Grafkit;

#define KEY_SET_INTERPOLATION (Qt::Key_I)
#define KEY_SET_TANGENT (Qt::Key_R)

#define RADIX_DEFUALT (1.f/4.f)

CurvePointItem::CurvePointItem(const Animation::Key key, const size_t index, QGraphicsItem* parent)
	: QGraphicsItem(parent)
	, m_id(index)
	, m_key(key)
	, m_scaling(1, 1)
{
	m_color = 0;
	m_nodeType = 0;

	m_showTangent = false;

	m_radix = m_radix2 = RADIX_DEFUALT;

	setFlag(ItemIsMovable);
	setFlag(ItemIsSelectable);

	show();
}

// TODO: might be useful when copy
//CurvePointItem::CurvePointItem(const CurvePointItem& other)
//    : QGraphicsItem(other.parentItem())
//{
//    m_id = other.m_id;
//    m_color = other.m_color;
//    m_nodeType = other.m_nodeType;
//
//    m_key = other.m_key;
//
//    m_showTangent = false;
//
//    m_radix = other.m_radix;
//    m_radix2 = RADIX_DEFUALT;
//
//    setFlag(ItemIsMovable);
//    setFlag(ItemIsSelectable);
//}

CurvePointItem::~CurvePointItem() = default;

void CurvePointItem::RecalculatePosition(TimelineArea const* area)
{
	if (m_key.m_time < 0.f)
		m_key.m_time = 0.f;
	setPos(area->Point2Screen(GetCoord()));
	m_scaling = area->Scale(); // save last scaling for paint tangent on
}

QRectF CurvePointItem::boundingRect() const { return { -m_radix2 * 32.0f, -m_radix2 * 32.0f, m_radix2 * 64.0f, m_radix2 * 64.0f }; }

void CurvePointItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	const QColor selectedColor = QColor(224, 224, 255);
	const QColor pointColor = QColor(224, 224, 224, 144);

	painter->setRenderHint(QPainter::Antialiasing);

	if (m_radix == 0.0f)
		m_radix = 0.1f;

	painter->setPen(Qt::NoPen);
	if (isSelected())
		painter->setBrush(selectedColor);
	else
		painter->setBrush(pointColor);

	painter->drawEllipse(QPoint(0, 0), 4, 4);

	if (isSelected())
	{
		painter->setPen(QPen(selectedColor, 1.0, Qt::SolidLine));
		painter->setBrush(Qt::NoBrush);
		painter->drawEllipse(QPoint(0, 0), 6, 6);
	}

	if (m_showTangent)
	{
		const QPointF _tangent = GetTangent();
		painter->setPen(QPen(QColor(255, 48, 48), 1.0, Qt::DashLine));
		painter->setBrush(Qt::NoBrush);

		const float screenTangentX = _tangent.x() * m_scaling.width();
		const float screenTangentY = -_tangent.y() * m_scaling.height();

		const float n = sqrt(screenTangentX * screenTangentX + screenTangentY * screenTangentY);
		const QPointF vTgt = QPointF(screenTangentX / n, screenTangentY / n);
		painter->drawLine(QPointF(0, 0), vTgt * 32.0f * m_radix);

		painter->setPen(QPen(QColor(96, 96, 96), 1.0, Qt::DotLine));
		painter->setBrush(Qt::NoBrush);
		painter->drawEllipse(QPointF(0, 0), 32.0f * m_radix, 32.0f * m_radix);
	}
}

QVariant CurvePointItem::itemChange(const GraphicsItemChange change, const QVariant& value) { return QGraphicsItem::itemChange(change, value); }


void CurvePointItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	switch (event->button())
	{
	case Qt::RightButton:
		m_showTangent = true;
		m_radix2 = m_radix;
		m_originalKey = m_key;
		onStartEdit(this);
		break;
	case Qt::LeftButton:
		m_originalKey = m_key;
		onStartEdit(this);
		QGraphicsItem::mousePressEvent(event);
		break;

	default:
		QGraphicsItem::mousePressEvent(event);
		break;
	}
	scene()->update();
}

void CurvePointItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	switch (event->button())
	{
	case Qt::RightButton:
		m_showTangent = false;
		m_radix2 = RADIX_DEFUALT;
		onCommitEdit(this);
		break;

	case Qt::LeftButton:
		onCommitEdit(this);
		QGraphicsItem::mouseReleaseEvent(event);
		break;

	default:
		QGraphicsItem::mouseReleaseEvent(event);
		break;
	}
	scene()->update();
}

void CurvePointItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	const CurveEditorScene* ces = dynamic_cast<CurveEditorScene*>(scene());

	assert(ces);

	if (m_showTangent) { EditTangent(event); }
	else { EditPosition(event); }

	// Ha tobb elem van kijelolve, akkor azokat lehet mozgatni elvileg egyben
	foreach(QGraphicsItem* it, ces->selectedItems())
	{
		CurvePointItem* cpit = dynamic_cast<CurvePointItem*>(it);
		if (cpit != nullptr && cpit != this)
		{
			const QPointF delta = cpit->pos() - pos();
			const QPointF fakeMousePos = event->pos() + delta;
			const QPointF fakeMouseScenePos = event->scenePos() + delta;
			QGraphicsSceneMouseEvent fakeEvent(QEvent::GraphicsSceneMouseMove);
			fakeEvent.setPos(fakeMousePos);
			fakeEvent.setScenePos(fakeMouseScenePos);
			cpit->FakeMouseMoveEvent(&fakeEvent);
		}
	}

	scene()->update();
	QGraphicsItem::mouseMoveEvent(event);
}

// ezzel lehet a tobbit mozgatni
void CurvePointItem::FakeMouseMoveEvent(class QGraphicsSceneMouseEvent* event)
{

	if (m_showTangent) { EditTangent(event); }
	else { EditPosition(event); }

	scene()->update();
}

void CurvePointItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
	//CurveEditorScene* ces = (CurveEditorScene*)scene();
	//ces->deleteCurvePoint(this);
}

//void Idogep::CurvePointItem::keyPressEvent(QKeyEvent * event)
//{
//	// itt csak modosito billnetyuket lenene erdemes 
//	QGraphicsItem::keyPressEvent(event);
//}
//
//void Idogep::CurvePointItem::keyReleaseEvent(QKeyEvent * event)
//{
//	if (isSelected()) {
//		switch (event->key())
//		{
//		case KEY_SET_INTERPOLATION:
//			// set interpolation, toggle 
//			event->accept();
//			break;
//		case KEY_SET_TANGENT:
//			toggleTangentEditing();
//			event->accept();
//			break;
//		default:
//			event->ignore();
//			break;
//		}
//	}
//	else {
//		event->ignore();
//	}
//}

void CurvePointItem::EditTangent(class QGraphicsSceneMouseEvent* event) {
	const CurveEditorScene* ces = dynamic_cast<CurveEditorScene*>(scene());
	assert(ces);

	const TimelineArea* area = ces->GetArea();
	assert(area);

	m_radix = sqrt(
		event->pos().x() * event->pos().x() +
		event->pos().y() * event->pos().y()) / 32.0f;

	if (m_radix == 0.0f)
		m_radix = 0.01f;

	m_radix2 = m_radix;

	const float epsilon = 0.005f;
	float angle = atan2(event->pos().y(), event->pos().x());
	if (angle < -M_PI / 2.0f + epsilon)
		angle = -M_PI / 2.0f + epsilon;
	else if (angle > M_PI / 2.0f - epsilon)
		angle = M_PI / 2.0f - epsilon;

	QSizeF areaScale = area->Scale();

	QPointF tangent;
	tangent.setX(cos(angle) * m_radix / areaScale.width());
	tangent.setY(-sin(angle) * m_radix / areaScale.height());

	SetTangent(tangent);

	onEditKey(this);
}

void CurvePointItem::EditPosition(class QGraphicsSceneMouseEvent* event) {

	const CurveEditorScene* ces = dynamic_cast<CurveEditorScene*>(scene());
	assert(ces);
	const TimelineArea* area = ces->GetArea();
	assert(area);

	const QPointF coord = area->Screen2Point(event->scenePos());

	SetCoord(coord);

	onEditKey(this);
}

void CurvePointItem::ToggleTangentEditing() {
	if (m_showTangent) { m_radix2 = 1.0f / 8.0f; }
	else { m_radix2 = m_radix; }

	m_showTangent = !m_showTangent;
	this->update();
}
