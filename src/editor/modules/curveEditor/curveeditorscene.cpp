#include <QGraphicsScene>
#include <QGraphicsSceneEvent>
#include <QPainter>

#include "qgraphicsview.h"

#include "animation/animation.h"

#include "curveeditorwidget.h"
#include "curveeditorscene.h"
#include "curvepointitem.h"
#include "curvedoc.h"
#include "CurveSceneModule.h"


using namespace Idogep;
using namespace Grafkit;

namespace {
	const QColor grey = QColor(192, 192, 192);
	const QColor red = QColor(255, 128, 128);
	const QColor blue = QColor(128, 128, 255);
	const QColor green = QColor(128, 255, 128);
	const QColor purple = QColor(255, 128, 255);
}

CurveEditorScene::CurveEditorScene(QObject * parent) : QGraphicsScene(parent), CurveSceneView()
, m_area(nullptr)
, m_audiogramImage(nullptr)
{
	m_area = new TimelineArea();
	setBackgroundBrush(QColor(48, 48, 48));
}

CurveEditorScene::~CurveEditorScene()
{
	delete m_area;
	delete m_audiogramImage;
}

void CurveEditorScene::RefreshView(const bool force)
{
	if (force)
		UpdateAudiogram();
	update();
}

void CurveEditorScene::PlaybackChanged(bool isPlaying)
{
}

void CurveEditorScene::DemoTimeChanged(float time)
{
}

void CurveEditorScene::ClearCurvePoints()
{
	clear();
}

void CurveEditorScene::AddCurvePoint(CurvePointItem* pointItem)
{
	addItem(pointItem);
}

// ---------------------------------------------------------------------
// DRAW STUFF 

void CurveEditorScene::drawBackground(QPainter* painter, const QRectF& r)
{
	QGraphicsScene::drawBackground(painter, r);
	painter->setBrush(Qt::NoBrush);

	setSceneRect(views().at(0)->geometry());

	if (m_displayWaveform)
	{
		painter->fillRect(0, 0, 16, 16, QBrush(QColor(255, 0, 0)));

		UpdateAudiogram();

		if (m_audiogramImage)
		{
			painter->drawImage(r, *m_audiogramImage);
		}
	}

	// because rect() is relative to the widgets parent

	m_area->SetSceneRect(sceneRect());

	
	m_area->DrawGrid(painter, r);

	if (m_displayCurve)
		DrawCurve(painter, r);

	//TOOD draw cursor

}

void CurveEditorScene::DrawCurve(QPainter* painter, const QRectF& r) const
{
	// TODO: Move this out somewhere else 
	CurveSceneModule* parent = dynamic_cast<CurveSceneModule*>(m_module.Get());
	assert(parent);

	CurveManager* curveManager = parent->GetCurveManager();

	// 2. draw the curves.
	const QList<CurvePointItem*> * const points = curveManager->GetCurvePoints();

	// debug 
	{
		painter->fillRect(0, 0, 16, 16, QBrush(QColor(255, 0, 0), Qt::SolidPattern));
	}

	// prevernt crash on NPE
	if (!points || points->isEmpty())
		return;

	//debug
	{
		painter->fillRect(0, 0, 16, 16, QBrush(QColor(0, 255, 0), Qt::SolidPattern));
	}

	auto channel = curveManager->GetChannel();

	curveManager->Recalculate(m_area);

	painter->setRenderHint(QPainter::Antialiasing);
	painter->setPen(QPen(grey));

	// boundaries
	QPointF pMin = m_area->Screen2Point(r.topLeft());
	const int idmin = channel->FindKeyIndex(pMin.x());

	QPointF pMax = m_area->Screen2Point(r.bottomRight());
	int idmax = channel->FindKeyIndex(pMax.x()) + 1;

	if (idmin == 0 && points->at(0)->x() > r.x())
	{
		painter->drawLine(
			0, points->at(0)->pos().y(),
			points->at(0)->pos().x(), points->at(0)->pos().y()
		);

	}


	// bool debug shit 
	int a = 0, b = 0;

	if (points->last()->pos().x() < r.x() + r.width()) {
		painter->drawLine(
			points->last()->pos().x(), points->last()->pos().y(),
			r.x() + r.width(), points->last()->pos().y()
		);

		// debug stuff
		points->last()->setVisible(true);
	}

	if (idmax >= channel->GetKeyCount() - 1) {
		idmax = channel->GetKeyCount() - 1; // avoid max+1 index
	}

	//if (a || b)
	//{
	//	painter->fillRect(32, 0, 16, 16, QBrush(QColor(0, b, a), Qt::SolidPattern));
	//}
	//else
	//{
	//	painter->fillRect(32, 0, 16, 16, QBrush(QColor(255, 0, 0), Qt::SolidPattern));
	//}

	for (int i = idmin; i < idmax; i++)
	{
		CurvePointItem *point = points->at(i);
		point->setVisible(true);


		const auto k0 = channel->GetKey(i);
		const auto k1 = channel->GetKey(i + 1);

		// valahogy lehetne optiomalgatni ezt is 
		const int steps = 32;
		double stepWidth = 0.;
		//steps = 64;
		stepWidth = (k1.m_time - k0.m_time) / steps;

		for (int j = 0; j < steps; j++) {
			const double t = k0.m_time + j * stepWidth;
			double v = channel->GetValue(t);

			QPointF p1(t, channel->GetValue(t));
			p1 = this->m_area->Point2Screen(p1);

			QPointF p2(t + stepWidth, channel->GetValue(t + stepWidth));
			p2 = this->m_area->Point2Screen(p2);

			painter->drawLine(p1, p2);
		}
	}
}

void CurveEditorScene::UpdateAudiogram()
{
	const auto offset = m_area->Offset();
	const auto scale = m_area->Scale();

	const float leftTime = -float(offset.x()) / float(scale.width());
	const float rightTime = leftTime + (float(sceneRect().width()) / float(scale.width()));

	if (leftTime < 0.0f || rightTime < 0.0f || leftTime >= rightTime)
		return;

	QImage* img = nullptr;

	onRequestAudiogram(&img, leftTime, rightTime, int(sceneRect().width()), int(sceneRect().height()));

	if (!img)
		return;

	delete m_audiogramImage;
	m_audiogramImage = img;
}


// ------------------------------------------------------------------
TimelineArea::TimelineArea()
{
	m_scale = QSizeF(64, 64);
	m_offset = QPointF(0, 0);
}

QPointF TimelineArea::Point2Screen(QPointF point) const
{
	return {
		(point.x() * m_scale.width() + m_offset.x() + m_sceneRect.topLeft().x()) ,
		(point.y() * -m_scale.height() + m_offset.y() + m_sceneRect.topLeft().y() + m_sceneRect.height() / 2.)
	};
}

QPointF TimelineArea::Screen2Point(QPointF point) const
{
	return {
		(point.x() - m_offset.x() - m_sceneRect.topLeft().x()) / m_scale.width(),
		(point.y() - m_offset.y() - m_sceneRect.topLeft().y() - m_sceneRect.height() / 2.) / -m_scale.height()
	};
}

// ReSharper disable CppInconsistentNaming
void TimelineArea::DrawGrid(QPainter * painter, const QRectF & r) const
{
	painter->translate(r.topLeft());

	float sPos = 0.0f;
	if (r.x() < 0.0f)
		sPos = -1.0f * fmod(fabs(r.x()), m_scale.width());
	else sPos = fmod(fabs(r.x()), m_scale.width());

	float sc = m_scale.width() / 4.0f;
    const float offsetX = m_offset.x();
    const float offsetY = m_offset.y();

    const float rectWidth = m_sceneRect.width();
    const float rectHeight = m_sceneRect.height();

    for (float f = fmod(offsetX, sc); f <= rectWidth + fmod(offsetX, sc); f += sc) {
		painter->setPen(QPen(QColor(56, 56, 56)));
		painter->drawLine(f, 0.0f, f, rectHeight);
	}

	sc = m_scale.width();
	for (float f = fmod(offsetX, sc); f <= rectWidth + fmod(offsetX, sc); f += sc) {
		painter->setPen(QPen(QColor(64, 64, 64)));
		painter->drawLine(f, 0.0f, f, rectHeight);
	}

	sc = m_scale.height() / 4.0f;
	for (float f = fmod(offsetY, sc); f <= rectHeight + fmod(offsetY + r.height() / 2, sc); f += sc) {
		painter->setPen(QPen(QColor(56, 56, 56)));
		painter->drawLine(0.0f, f, rectWidth, f);
	}

	sc = m_scale.height();
	for (float f = fmod(offsetY, sc); f <= rectHeight + fmod(offsetY + r.height() / 2, sc); f += sc) {
		painter->setPen(QPen(QColor(64, 64, 64)));
		painter->drawLine(0.0f, f, rectWidth, f);
	}

	painter->setPen(QPen(QColor(144, 144, 144)));
	painter->drawLine(offsetX, 0.0f, offsetX, rectHeight);
	painter->setPen(QPen(QColor(144, 144, 144)));
	painter->drawLine(0.0f, offsetY + r.height() / 2, rectWidth, offsetY + r.height() / 2);

	painter->restore();

}
// ReSharper restore CppInconsistentNaming


#if 0

CurveEditorScene::CurveEditorScene(CurveEditorWidget* pWidget, QObject* parent) : QGraphicsScene(parent)
{
	setBackgroundBrush(QColor(48, 48, 48));

	m_scale = QSizeF(64.0f, 64.0f);

	m_ofs = QPointF(32.0f, 108.0f);
	m_modifyOfs = false;
	m_modifyScale = false;

	m_demoTime = 0.0f;
	m_demoTimeChanged = false;

	m_followTimeBar = true;
	m_displayWaveform = true;

	m_widget = pWidget;

	m_audiogramImage = nullptr;

	m_document = nullptr;
}

QSizeF CurveEditorScene::scale() const
{
	return m_scale;
}

QPointF CurveEditorScene::offset() const {
	return m_ofs;
}

void Idogep::CurveEditorScene::SetDocument(ManageCurveRole * doc)
{
	m_document = doc;
	//doc->addCurveToScene(this);
	//this->update();
}

void CurveEditorScene::drawBackground(QPainter* painter, const QRectF& rect)
{
	QGraphicsScene::drawBackground(painter, rect);
	painter->setBrush(Qt::NoBrush);

	setSceneRect(views().at(0)->geometry());

	if (!m_document)
		return;

	m_document->Recalculate();

	if (m_ofs.x() > 0.0f)
	{
		m_ofs.setX(0.0f);
		updateAudiogram();
	}

	if (m_audiogramImage && m_displayWaveform)
	{
		painter->drawImage(rect, *m_audiogramImage);
	}

	// because rect() is relative to the widgets parent
	painter->translate(rect.topLeft());

	// --- 
	// 0. Draw the axis + subaxis. =)))

	painter->restore();

	// ---- 

	drawCurve(painter, rect);
	// --- 
	// Draw cursor 
	drawCursor(painter, rect);

#if 1
	// debug 
	if (m_document->GetCurvePoints() == nullptr) {
		painter->fillRect(QRect(0, 0, 16, 16), QBrush(Qt::red));
	}

#endif
}


void CurveEditorScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
	if (!m_document)
		return;

	CurvePointItem * item = qgraphicsitem_cast<CurvePointItem*>(itemAt(event->scenePos(), QTransform()));

	if (item) {
	}
	else {
		update();
	}

	QGraphicsScene::mouseDoubleClickEvent(event);
}

void CurveEditorScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
	bool thereIsAnItem = false;
	//if (m_pointItems) for (int i = 0; i < m_pointItems->size(); i++)
	//{
	//	if (m_pointItems->at(i)->contains(m_pointItems->at(i)->mapFromScene(event->scenePos()))) {
	//		thereIsAnItem = true;
	//		break;
	//	}
	//}

	if (!thereIsAnItem) {
		switch (event->button()) {
		case Qt::LeftButton:

			//setDemoTime((event->scenePos().x() - m_ofs.x()) / m_scale.width());
			//m_widget->setMusicTime((event->scenePos().x() - m_ofs.x()) / m_scale.width());
			// eventet dobjon itt 

			m_modifyDemoTime = true;
			break;
		case Qt::MidButton:
			// ctrl vagy shift mod. billre modositsa a masikat
			m_modifyScale = true;
			break;

		case Qt::RightButton:
			m_modifyOfs = true;
			break;
		default:
			break;
		}
	}
	update();
	QGraphicsScene::mousePressEvent(event);
}

void CurveEditorScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
	if (m_modifyOfs || m_modifyScale) updateAudiogram();

	m_modifyDemoTime = false;
	m_modifyOfs = false;
	m_modifyScale = false;
	update();
	QGraphicsScene::mouseReleaseEvent(event);
}

void CurveEditorScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
	if (m_modifyOfs) {
		m_ofs += event->scenePos() - event->lastScenePos();
	}
	else if (m_modifyScale) {
		QPointF p(QPointF(event->scenePos() - event->lastScenePos()));
		m_scale += QSizeF(p.x(), p.y()) / 4.;
		if (m_scale.width() < 8.0f) m_scale.setWidth(8.0f);
		if (m_scale.height() < 8.0f) m_scale.setHeight(8.0f);
	}

	if (!m_modifyOfs && !m_modifyScale) {

		// eventet dobjon itt 

		//setDemoTime((event->scenePos().x() - m_ofs.x()) / m_scale.width());
		//if (m_demoTime < 0.0f) setDemoTime(0.0f);
		//m_widget->setMusicTime((event->scenePos().x() - m_ofs.x()) / m_scale.width());
	}

	update();
	QGraphicsScene::mouseMoveEvent(event);
}

// -> GK
#if 0
QPointF CurveEditorScene::_interpolateHermite(QPointF p0, QPointF p1, QPointF r0, QPointF r1, float t) {
	float t2 = t * t;
	float t3 = t2 * t;

	return (
		(2.0f*t3 - 3.0f*t2 + 1.0f) * p0 +
		(-2.0f*t3 + 3.0f*t2) * p1 +
		(t3 - 2.0f*t2 + t) * r0 +
		(t3 - t2) * r1
		);
}
#endif

void Idogep::CurveEditorScene::drawCurve(QPainter * painter, const QRectF & rect)
{

}

#if 0
void Idogep::CurveEditorScene::drawCurve_old(QPainter * painter, const QRectF & rect)
{
	// 2. draw the curves.
	QList<CurvePointItem*> *points = m_document->GetCurvePoints();

	if (!points->isEmpty()) {
		painter->setRenderHint(QPainter::Antialiasing);
		painter->setPen(QPen(QColor(128, 128, 255)));
		if (points->at(0)->x() > rect.x())
			painter->drawLine(0, points->at(0)->pos().y(), points->at(0)->pos().x(), points->at(0)->pos().y());

		if (points->last()->pos().x() < rect.x() + rect.width())
			painter->drawLine(points->last()->pos().x(), points->last()->pos().y(), rect.x() + rect.width(), points->last()->pos().y());

		for (int i = 0; i < points->size() - 1; i++) {
			points->at(i)->setVisible(true);
			// hermite interpolation using those tangents - with a tad of a trick to prevent going back in time. =)
			int steps = 64;
			for (int j = 0; j < steps; j++) {
				float t1 = float(j) / float(steps);
				float t2 = float(j + 1) / float(steps);

				// ez kell majd meg: 

				qreal TangentX = points->at(i + 1)->coord().x() - points->at(i)->coord().x();
				QPointF Tangent0 = QPointF(TangentX, TangentX * points->at(i)->tangent().y() / points->at(i)->tangent().x());
				QPointF Tangent1 = QPointF(TangentX, TangentX * points->at(i + 1)->tangent().y() / points->at(i + 1)->tangent().x());

				QPointF p1 = _interpolateHermite(
					points->at(i)->coord(),
					points->at(i + 1)->coord(),
					Tangent0,
					Tangent1,
					t1
				);
				p1 = QPointF(p1.x() * m_scale.width(), p1.y() * -m_scale.height());

				QPointF p2 = _interpolateHermite(
					points->at(i)->coord(),
					points->at(i + 1)->coord(),
					Tangent0,
					Tangent1,
					t2
				);
				p2 = QPointF(p2.x() * m_scale.width(), p2.y() * -m_scale.height());

				painter->drawLine(p1 + m_ofs, p2 + m_ofs);
			}
		}
		int i = points->size() - 1;
		points->at(i)->setVisible(true);
	}
}

#endif

void Idogep::CurveEditorScene::drawCursor(QPainter * painter, const QRectF & rect)
{
	painter->translate(sceneRect().topLeft());
	// and at the very end, display a big fat green line on top of everything (demo time)
	painter->setPen(QPen(QColor(48, 224, 48, 96), 2.0, Qt::SolidLine));
	painter->setBrush(Qt::NoBrush);
	painter->drawLine(
		(m_demoTime * m_scale.width() + m_ofs.x()), rect.y(),
		(m_demoTime * m_scale.width() + m_ofs.x()), rect.y() + rect.height()
	);

	QString strTime;
	int minutes = int(m_demoTime) / 60;
	if (minutes < 10) strTime.append("0");
	strTime.append(QString::number(minutes)).append(":");
	if (fmod(m_demoTime, 60.0f) < 10.0f) strTime.append("0");
	strTime.append(QString::number(fmod(m_demoTime, 60.0f), 'f', 3));

	if (m_demoTimeChanged)
	{
		m_demoTimeChanged = false;
		if (m_followTimeBar)
		{
			if ((rect.x() + rect.width() - m_ofs.x()) / m_scale.width() < m_demoTime)
			{
				while ((rect.x() + rect.width() - m_ofs.x()) / m_scale.width() < m_demoTime)
					m_ofs.setX(m_ofs.x() - m_scale.width()*2.0f);

				if (m_ofs.x() > 0.0f)  m_ofs.setX(0.0f);

				updateAudiogram();
			}

			else if ((rect.x() - m_ofs.x()) / m_scale.width() > m_demoTime)
			{
				while ((rect.x() - m_ofs.x()) / m_scale.width() > m_demoTime)
					m_ofs.setX(m_ofs.x() + m_scale.width()*2.0f);

				if (m_ofs.x() > 0.0f)  m_ofs.setX(0.0f);

				updateAudiogram();
			}
		}
	}

	float barOffset = 0.0f;
	if (abs(rect.x() + rect.width() - (m_demoTime * m_scale.width() + m_ofs.x())) < 128)
		barOffset = -56.0f;

	painter->setFont(QFont(QString("Open Sans"), 8));
	painter->setPen(Qt::NoPen);
	painter->setBrush(QColor(48, 224, 48, 96));
	painter->drawRect(QRectF(QPointF((m_demoTime * m_scale.width() + m_ofs.x()) + barOffset, rect.height() - 16.0f), QSizeF(56.0f, 16.0f)));
	painter->setPen(QColor(255, 255, 255, 96));
	painter->setBrush(QColor(48, 224, 48, 96));
	painter->drawText(QRectF(QPointF((m_demoTime * m_scale.width() + m_ofs.x()) + barOffset, rect.height() - 16.0f), QSizeF(56.0f, 16.0f)), strTime, QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));

	painter->restore();
}



#endif 

