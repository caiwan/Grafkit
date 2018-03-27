#include "qgraphicsscene.h"
#include "qgraphicssceneevent.h"

#include "qgraphicsview.h"

#include "animation/animation.h"

#include "curveeditorwidget.h"
#include "curveeditorscene.h"
#include "curvepointitem.h"
#include "curvedoc.h"

//#include "qfmodex.hh"

using namespace Idogep;

namespace {
	const QColor grey = QColor(192, 192, 192);
	const QColor red = QColor(255, 128, 128);
	const QColor blue = QColor(128, 128, 255);
	const QColor green = QColor(128, 255, 128);
	const QColor purple = QColor(255, 128, 255);
}

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

	m_audiogramImage = NULL;
}

QSizeF CurveEditorScene::scale() const
{
	return m_scale;
}

QPointF CurveEditorScene::offset() const {
	return m_ofs;
}

void Idogep::CurveEditorScene::setDocument(CurveDocument * doc)
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

	m_document->recalculate();

	if (m_ofs.x() > 0.0f)
	{
		m_ofs.setX(0.0f);
		updateAudiogram();
	}

	if (m_audiogramImage && m_displayWaveform)
	{
		painter->drawImage(rect, *m_audiogramImage);
	}

	painter->translate(rect.topLeft());

	// --- 
	// 0. Draw the axis + subaxis. =)))
	float sPos = 0.0f;
	if (rect.x() < 0.0f)
		sPos = -1.0f * fmod(fabs(rect.x()), m_scale.width());
	else sPos = fmod(fabs(rect.x()), m_scale.width());

	float sc = m_scale.width() / 4.0f;
	for (float f = fmod(m_ofs.x(), sc); f <= sceneRect().width() + fmod(m_ofs.x(), sc); f += sc) {
		painter->setPen(QPen(QColor(56, 56, 56)));
		painter->drawLine(f, 0.0f, f, sceneRect().height());
	}
	sc = m_scale.width();
	for (float f = fmod(m_ofs.x(), sc); f <= sceneRect().width() + fmod(m_ofs.x(), sc); f += sc) {
		painter->setPen(QPen(QColor(64, 64, 64)));
		painter->drawLine(f, 0.0f, f, sceneRect().height());
	}

	sc = m_scale.height() / 4.0f;
	for (float f = fmod(m_ofs.y(), sc); f <= sceneRect().height() + fmod(m_ofs.y(), sc); f += sc) {
		painter->setPen(QPen(QColor(56, 56, 56)));
		painter->drawLine(0.0f, f, sceneRect().width(), f);
	}
	sc = m_scale.height();
	for (float f = fmod(m_ofs.y(), sc); f <= sceneRect().height() + fmod(m_ofs.y(), sc); f += sc) {
		painter->setPen(QPen(QColor(64, 64, 64)));
		painter->drawLine(0.0f, f, sceneRect().width(), f);
	}

	painter->setPen(QPen(QColor(144, 144, 144)));
	painter->drawLine(m_ofs.x(), 0.0f, m_ofs.x(), sceneRect().height());
	painter->setPen(QPen(QColor(144, 144, 144)));
	painter->drawLine(0.0f, m_ofs.y(), sceneRect().width(), m_ofs.y());

	// ---- 

	drawCurve(painter, rect);
	// --- 
	// Draw cursor 
	drawCursor(painter, rect);

#if 1
	// debug 
	if (m_document->getCurvePoints() == nullptr ) {
		painter->fillRect(QRect(0, 0, 16, 16), QBrush(Qt::red));
	}

#endif
}

void CurveEditorScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
	//if (!m_pointItems) return;

	bool thereIsAnItem = false;
	//for (int i = 0; i < m_pointItems->size(); i++) {
	//	if (m_pointItems->at(i)->contains(m_pointItems->at(i)->mapFromScene(event->scenePos()))) {
	//		thereIsAnItem = true;
	//		break;
	//	}
	//}

	//if (!thereIsAnItem) {
	//	CurvePointItem* cpi = new CurvePointItem(QPointF((event->scenePos().x() - m_ofs.x()) / m_scale.width(), (event->scenePos().y() - m_ofs.y()) / -m_scale.height()));
	//	//m_pointItems->append(cpi);

	//	addItem(cpi);
	//	cpi->update();
	//	update();
	//	views().at(0)->update();
	//}

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

QPointF Idogep::CurveEditorScene::point2Screen(QPointF point) const
{
	return QPointF(
		point.x() * scale().width() + offset().x(),
		point.y() * -scale().height() + offset().y()
	);
}

QPointF Idogep::CurveEditorScene::screen2Point(QPointF point) const
{
	return QPointF(
		(point.x() - offset().x()) / scale().width(),
		(point.y() - offset().y()) / -scale().height()
	);
}

void Idogep::CurveEditorScene::drawCurve(QPainter * painter, const QRectF & rect)
{
	// 2. draw the curves.
	QList<CurvePointItem*> *points = m_document->getCurvePoints();

	// prevernt crash on NPE
	if (!points || points->isEmpty())
		return;

	auto track = m_document->getTrack();

	painter->setRenderHint(QPainter::Antialiasing);
	painter->setPen(QPen(grey));

	// boundaries
	QPointF pMin = screen2Point(rect.topLeft());
	int idmin = track->FindKeyIndex(pMin.x());

	QPointF pMax = screen2Point(rect.bottomRight());
	int idmax = track->FindKeyIndex(pMax.x()) + 1;

	if (idmin == 0 && points->at(0)->x() > rect.x())
		painter->drawLine(
			0, points->at(0)->pos().y(),
			points->at(0)->pos().x(), points->at(0)->pos().y()
		);

	if (points->last()->pos().x() < rect.x() + rect.width()) {
		painter->drawLine(
			points->last()->pos().x(), points->last()->pos().y(),
			rect.x() + rect.width(), points->last()->pos().y()
		);

		points->last()->setVisible(true);
	}

	if (idmax >= track->GetKeyCount() - 1)
		idmax = track->GetKeyCount() - 1; // avoid max+1 index

	for (int i = idmin; i < idmax; i++)
	{
		CurvePointItem *point = points->at(i);
		point->setVisible(true);

		// debug stuff
		{
			QRectF rekt = point->boundingRect();
			rekt.moveCenter(point->pos());
			painter->fillRect(rekt, QBrush(QColor(255, 0, 0), Qt::Dense6Pattern));
		}

		auto k0 = track->GetKey(i);
		auto k1 = track->GetKey(i + 1);

		// valahogy lehetne optiomalgatni ezt is 
		int steps = 32;
		double stepWidth = 0.;
		//steps = 64;
		stepWidth = (k1.m_key - k0.m_key) / steps;

		for (int j = 0; j < steps; j++) {
			double t = k0.m_key + j * stepWidth;
			double v = track->GetValue(t);

			QPointF p1(t, track->GetValue(t));
			p1 = this->point2Screen(p1);

			QPointF p2(t + stepWidth, track->GetValue(t + stepWidth));
			p2 = this->point2Screen(p2);

			painter->drawLine(p1, p2);
		}
	}
}

void Idogep::CurveEditorScene::drawCurve_old(QPainter * painter, const QRectF & rect)
{
	// 2. draw the curves.
	QList<CurvePointItem*> *points = m_document->getCurvePoints();

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

void Idogep::CurveEditorScene::drawCursor(QPainter * painter, const QRectF & rect)
{
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

}


void CurveEditorScene::updateAudiogram()
{
	if (!m_displayWaveform) return;

	float leftTime = -float(m_ofs.x()) / float(m_scale.width());
	float rightTime = leftTime + (float(sceneRect().width()) / float(m_scale.width()));

	if (leftTime < 0.0f || rightTime < 0.0f || leftTime >= rightTime) return;

	QImage* img = nullptr;
	m_document->getAudiogram(&img, leftTime, rightTime, int(sceneRect().width()), int(sceneRect().height()));

	if (!img) return;

	m_audiogramImage = img;
}
