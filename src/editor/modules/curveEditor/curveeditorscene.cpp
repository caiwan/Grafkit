#include <QGraphicsScene>
#include <QGraphicsSceneEvent>
#include <QDebug>

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
	m_area->SetSceneRect(sceneRect());

	if (m_displayWaveform)
	{
		UpdateAudiogram();

		if (m_audiogramImage)
		{
			painter->drawImage(r, *m_audiogramImage);
			//painter->fillRect(0, 0, 16, 16, QBrush(QColor(0, 255, 0)));
		}
		//else
		//{
		//	//painter->fillRect(0, 0, 16, 16, QBrush(QColor(255, 0, 0)));
		//}

	}
	m_area->DrawGrid(painter, r);

	if (m_displayCurve)
		DrawCurve(painter, r);

	// draw cursors here if needed 
}

void CurveEditorScene::DrawCurve(QPainter* painter, const QRectF& r) const
{
	const CurveSceneModule* parent = dynamic_cast<CurveSceneModule*>(m_module.Get());
	assert(parent);

	CurveManager* curveManager = parent->GetCurveManager();

	// 2. draw the curves.

	auto channel = curveManager->GetChannel();

	if (channel.Invalid() || channel->GetKeyCount() == 0)
		return;

	curveManager->Recalculate(m_area);

	painter->setRenderHint(QPainter::Antialiasing);
	painter->setPen(QPen(grey));

	// boundaries
	QPointF pMin = m_area->Screen2Point(r.topLeft());
	const int idmin = channel->FindKeyIndex(pMin.x());

	QPointF pMax = m_area->Screen2Point(r.bottomRight());
	int idmax = channel->FindKeyIndex(pMax.x()) + 1;

	// line -inf -> fist point
	auto firstKey = channel->GetKey(0);
	QPointF firstPoint = m_area->Point2Screen({ firstKey.m_time, firstKey.m_value });
	if (idmin == 0 && firstPoint.x() > r.x())
	{
		painter->drawLine({ 0, firstPoint.y() }, firstPoint);
	}

	// line last point -> +inf
	auto lastKey = channel->GetKey(channel->GetKeyCount() - 1);
	QPointF lastKeyPoint = m_area->Point2Screen({ lastKey.m_time, lastKey.m_value });
	if (lastKeyPoint.x() < r.x() + r.width()) {
		painter->drawLine( lastKeyPoint,  { r.x() + r.width(), lastKeyPoint.y() } );
	}

	if (idmax >= channel->GetKeyCount() - 1) {
		idmax = channel->GetKeyCount() - 1; // avoid max+1 index
	}

	// qnd fix for offset

	for (int i = idmin; i < idmax; i++)
	{
		const auto k0 = channel->GetKey(i);
		const auto k1 = channel->GetKey(i + 1);

		// there should be a nice way to optimize this 
		const int stepCount = 32;
		const double stepWidth = (k1.m_time - k0.m_time) / stepCount;

		for (int j = 0; j < stepCount; j++) {
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


void TimelineArea::DrawGrid(QPainter * painter, const QRectF & r) const
{
	const QTransform transform = painter->transform();
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

	painter->setTransform(transform);
}

