#include "qgraphicsscene.h"
#include "qgraphicssceneevent.h"

#include "qgraphicsview.h"

#include "render/animation.h"

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

void Idogep::CurveEditorScene::documentChanged(CurveDocument * doc)
{
	m_document = doc;
	doc->addCurveToScene(this);
	this->update();
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

#if 0
	// debug 
	if (!m_document) {
		painter->fillRect(QRect(0, 0, 16, 16), QBrush(Qt::red));
	}
	else {
		painter->fillRect(QRect(0, 0, 16, 16), QBrush(Qt::green));
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

	if (!thereIsAnItem) {
		CurvePointItem* cpi = new CurvePointItem(QPointF((event->scenePos().x() - m_ofs.x()) / m_scale.width(), (event->scenePos().y() - m_ofs.y()) / -m_scale.height()));
		//m_pointItems->append(cpi);

		addItem(cpi);
		cpi->update();
		update();
		views().at(0)->update();
	}

	QGraphicsScene::mouseDoubleClickEvent(event);
}

void CurveEditorScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
	bool thereIsAnItem = false;
	/*if (m_pointItems) for (int i = 0; i < m_pointItems->size(); i++)
	{
		if (m_pointItems->at(i)->contains(m_pointItems->at(i)->mapFromScene(event->scenePos()))) {
			thereIsAnItem = true;
			break;
		}
	}
*/
	if (!thereIsAnItem) {
		switch (event->button()) {
		case Qt::LeftButton:

			//setDemoTime((event->scenePos().x() - m_ofs.x()) / m_scale.width());
			//m_widget->setMusicTime((event->scenePos().x() - m_ofs.x()) / m_scale.width());
			// eventet dobjon itt 

			m_modifyDemoTime = true;
			break;
		case Qt::MidButton:
			m_modifyOfs = true;
			break;
		case Qt::RightButton:
			m_modifyScale = true;
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
		m_scale += QSizeF(p.x(), p.y());
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

void Idogep::CurveEditorScene::viewResized(QResizeEvent * event)
{
	updateAudiogram();
}

//void CurveEditorScene::deleteCurvePoint(CurvePointItem* cpi) {
//	if (!m_pointItems) return;
//	removeItem(cpi);
//	delete m_pointItems->takeAt(m_pointItems->indexOf(cpi));
//	update();
//}

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

float CurveEditorScene::simpleInterpolate(QList<CurvePointItem*>* list, float t) {
	//if (!list) return 0.0f;
	//if (list->isEmpty()) return 0.0f;
	//if (list->count() == 1) return list->at(0)->coord().y();

	//QList<CurvePointItem*> unsortedPointItems;
	//if (m_pointItems) unsortedPointItems = QList<CurvePointItem*>(*list);
	//QList<CurvePointItem*> sortedPointItems;
	//while (!unsortedPointItems.isEmpty()) {
	//	CurvePointItem* minPoint = NULL;
	//	float minTime = 9999.0f;
	//	for (int i = 0; i < unsortedPointItems.size(); i++) {
	//		if (unsortedPointItems[i]->time() < minTime) {
	//			minPoint = unsortedPointItems[i];
	//			minTime = unsortedPointItems[i]->time();
	//		}
	//	}
	//	sortedPointItems.append(minPoint);
	//	unsortedPointItems.removeOne(minPoint);
	//}

	//for (int i = 0; i < sortedPointItems.size(); i++) {
	//	if (!i) {
	//		if (t <= sortedPointItems.at(i)->coord().x()) return sortedPointItems.at(i)->coord().y();
	//	}
	//	else if (i == sortedPointItems.size() - 1) return sortedPointItems.at(i)->coord().y();

	//	if (t > sortedPointItems.at(i)->coord().x() && t <= sortedPointItems.at(i + 1)->coord().x()) {
	//		qreal TangentX = sortedPointItems.at(i + 1)->coord().x() - sortedPointItems.at(i)->coord().x();
	//		QPointF Tangent0 = QPointF(TangentX, TangentX * sortedPointItems.at(i)->tangent().y() / sortedPointItems.at(i)->tangent().x());
	//		QPointF Tangent1 = QPointF(TangentX, TangentX * sortedPointItems.at(i + 1)->tangent().y() / sortedPointItems.at(i + 1)->tangent().x());

	//		return _interpolateHermite(
	//			sortedPointItems.at(i)->coord(),
	//			sortedPointItems.at(i + 1)->coord(),
	//			Tangent0,
	//			Tangent1,
	//			(t - sortedPointItems.at(i)->coord().x()) / (sortedPointItems.at(i + 1)->coord().x() - sortedPointItems.at(i)->coord().x())
	//		).y();
	//	}
	//}
	return 0.0f; // should never happen !
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
	auto track = m_document->getTrack();

	if (!points->isEmpty()) {
		painter->setRenderHint(QPainter::Antialiasing);
		painter->setPen(QPen(grey));

		// boundaries
		double tmin = offset().x();
		int idmin = track->FindKeyIndex(tmin);

		double tmax = rect.width() / scale().width() - tmin;
		int idmax = track->FindKeyIndex(tmax) + 1;


		if (idmin == 0 && points->at(0)->x() > rect.x())
			painter->drawLine(
				0, points->at(0)->pos().y(),
				points->at(0)->pos().x(), points->at(0)->pos().y()
			);

		if (idmax == track->GetKeyCount() && points->last()->pos().x() < rect.x() + rect.width()) {
			painter->drawLine(
				points->last()->pos().x(), points->last()->pos().y(),
				rect.x() + rect.width(), points->last()->pos().y()
			);
			idmax--; // avoid max+1 index
			points->last()->setVisible(true);
		}

		if (idmax - idmin > 1 && idmax) {
			for (int i = idmin; i < idmax; i++)
			{
				CurvePointItem *point = points->at(i);
				points->at(i)->setVisible(true);

				auto k0 = track->GetKey(i);
				auto k1 = track->GetKey(i + 1);

				// valahogy lehetne optiomalgatni ezt is 
				int steps = 32;
				double stepWidth = 0.;
				steps = 64;
				stepWidth = (k1.m_key - k0.m_key) / steps;

				for (int j = 0; j < steps; j++) {
					double t = k0.m_key + j * stepWidth;
					double v = track->GetValue(t);

					QPointF p1(t, track->GetValue(t));
					p1 = this->point2Screen(p1); /*QPointF(p1.x() * m_scale.width(), p1.y() * -m_scale.height());*/

					QPointF p2(t + stepWidth, track->GetValue(t + stepWidth));
					p2 = this->point2Screen(p2); /*QPointF(p2.x() * m_scale.width(), p2.y() * -m_scale.height());*/

					painter->drawLine(p1, p2);
				}
			}
		}
		else {
			// ... 
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
	if (abs(rect.x() + rect.width() - (m_demoTime * m_scale.width() + m_ofs.x())) < 128) barOffset = -56.0f;

	painter->setFont(QFont(QString("Open Sans"), 8));
	painter->setPen(Qt::NoPen);
	painter->setBrush(QColor(48, 224, 48, 96));
	painter->drawRect(QRectF(QPointF((m_demoTime * m_scale.width() + m_ofs.x()) + barOffset, rect.height() - 16.0f), QSizeF(56.0f, 16.0f)));
	painter->setPen(QColor(255, 255, 255, 96));
	painter->setBrush(QColor(48, 224, 48, 96));
	painter->drawText(QRectF(QPointF((m_demoTime * m_scale.width() + m_ofs.x()) + barOffset, rect.height() - 16.0f), QSizeF(56.0f, 16.0f)), strTime, QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));

}

//void CurveEditorScene::setCurrentCurve(QList<CurvePointItem*>* curve) {
//	m_curCurve = curve;
//	if (m_pointItems) foreach(CurvePointItem* cpi, *m_pointItems) {
//		removeItem(cpi);
//	}
//
//	m_pointItems = curve;
//
//	foreach(CurvePointItem* cpi, *m_pointItems) {
//		addItem(cpi);
//		cpi->update();
//	}
//	update();
//}
//
//void CurveEditorScene::setDemoTime(float t)
//{
//	m_demoTime = t;
//	m_demoTimeChanged = true;
//	m_widget->setDemoTime(t, true);
//	update();
//}
//
//float CurveEditorScene::demoTime() const
//{
//	return m_demoTime;
//}
//
//void CurveEditorScene::setFollowTimeBar(bool b)
//{
//	m_followTimeBar = b;
//}
//
//void CurveEditorScene::setDisplayWaveform(bool b)
//{
//	m_displayWaveform = b;
//	update();
//}
//
//void CurveEditorScene::getNewAudiogram()
//{
//	requestAudiogram();
//	update();
//}
//
//QList<CurvePointItem*>* CurveEditorScene::curve() const
//{
//	return m_curCurve;
//}

//void CurveEditorScene::cutSelectedPoints()
//{
//	if (!m_pointItems) return;
//
//	QClipboard* cb = QApplication::clipboard();
//	cb->clear();
//
//	QString data;
//
//	CurvePointItem* firstItem = NULL;
//
//	QList<CurvePointItem*> unsortedPointItems;
//	if (m_pointItems) unsortedPointItems = QList<CurvePointItem*>(*m_pointItems);
//	QList<CurvePointItem*> sortedPointItems;
//	while (!unsortedPointItems.isEmpty()) {
//		CurvePointItem* minPoint = NULL;
//		float minTime = 9999.0f;
//		for (int i = 0; i < unsortedPointItems.size(); i++) {
//			if (unsortedPointItems[i]->time() < minTime) {
//				minPoint = unsortedPointItems[i];
//				minTime = unsortedPointItems[i]->time();
//			}
//		}
//		sortedPointItems.append(minPoint);
//		unsortedPointItems.removeOne(minPoint);
//	}
//
//	QList<CurvePointItem*> pointsToDelete;
//
//	foreach(QGraphicsItem* it, sortedPointItems)
//	{
//		CurvePointItem* cpit = dynamic_cast<CurvePointItem*>(it);
//		if (!cpit) continue;
//		if (!cpit->isSelected()) continue;
//		if (!firstItem) firstItem = cpit;
//		float delta = cpit->coord().x() - firstItem->coord().x();
//
//		data.append(QString::number(delta, 'f', 6));
//		data.append(QString("\t"));
//		data.append(QString::number(cpit->coord().y(), 'f', 6));
//		data.append(QString("\t"));
//		data.append(QString::number(cpit->tangent().x(), 'f', 6));
//		data.append(QString("\t"));
//		data.append(QString::number(cpit->tangent().y(), 'f', 6));
//		data.append(QString("\n"));
//
//		pointsToDelete.append(cpit);
//	}
//	cb->setText(data.prepend("FRQ_PRDC_DATA\n"));
//
//	for (int i = 0; i < pointsToDelete.size(); i++)
//	{
//		deleteCurvePoint(pointsToDelete.at(i));
//	}
//}

//void CurveEditorScene::copySelectedPoints()
//{
//	if (!m_pointItems) return;
//
//	QClipboard* cb = QApplication::clipboard();
//	cb->clear();
//
//	QString data;
//
//	CurvePointItem* firstItem = NULL;
//
//	QList<CurvePointItem*> unsortedPointItems;
//	if (m_pointItems) unsortedPointItems = QList<CurvePointItem*>(*m_pointItems);
//	QList<CurvePointItem*> sortedPointItems;
//	while (!unsortedPointItems.isEmpty()) {
//		CurvePointItem* minPoint = NULL;
//		float minTime = 9999.0f;
//		for (int i = 0; i < unsortedPointItems.size(); i++) {
//			if (unsortedPointItems[i]->time() < minTime) {
//				minPoint = unsortedPointItems[i];
//				minTime = unsortedPointItems[i]->time();
//			}
//		}
//		sortedPointItems.append(minPoint);
//		unsortedPointItems.removeOne(minPoint);
//	}
//
//	foreach(QGraphicsItem* it, sortedPointItems)
//	{
//		CurvePointItem* cpit = dynamic_cast<CurvePointItem*>(it);
//		if (!cpit) continue;
//		if (!cpit->isSelected()) continue;
//		if (!firstItem) firstItem = cpit;
//		float delta = cpit->coord().x() - firstItem->coord().x();
//
//		data.append(QString::number(delta, 'f', 6));
//		data.append(QString("\t"));
//		data.append(QString::number(cpit->coord().y(), 'f', 6));
//		data.append(QString("\t"));
//		data.append(QString::number(cpit->tangent().x(), 'f', 6));
//		data.append(QString("\t"));
//		data.append(QString::number(cpit->tangent().y(), 'f', 6));
//		data.append(QString("\n"));
//	}
//	cb->setText(data.prepend("FRQ_PRDC_DATA\n"));
//}

//void CurveEditorScene::pasteSelectedPoints()
//{
//	if (!m_pointItems) return;
//
//	QClipboard* cb = QApplication::clipboard();
//
//	QString dataStr = cb->text();
//	if (dataStr.isEmpty()) return;
//
//	QStringList data = dataStr.split("\n");
//	if (data[0] != QString("FRQ_PRDC_DATA")) return;
//	data.removeFirst();
//	data.removeLast();
//
//	foreach(QString point, data)
//	{
//		QStringList strCoords = point.split("\t");
//		float cx = m_demoTime + strCoords.at(0).toFloat();
//		float cy = strCoords.at(1).toFloat();
//
//		float tx = m_demoTime + strCoords.at(2).toFloat();
//		float ty = strCoords.at(3).toFloat();
//
//		CurvePointItem* ncpi = new CurvePointItem();
//		ncpi->setCoord(QPointF(cx, cy));
//		ncpi->setTangent(QPointF(tx, ty));
//		m_pointItems->append(ncpi);
//		m_curCurve->append(ncpi);
//		addItem(ncpi);
//	}
//
//	update();
//}

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
