#if 0

#include "curveeditorwidget.hh"
#include "curveeditorscene.hh"
#include "curvepointitem.hh"

#include "qfmodex.hh"

CurveEditorScene::CurveEditorScene(CurveEditorWidget* pWidget, QObject* parent) : QGraphicsScene(parent)
{
    setBackgroundBrush(QColor(48, 48, 48));

    _scale = QSizeF(64.0f, 64.0f);

    _ofs = QPointF(32.0f, 108.0f);
    _modifyOfs = false;
    _modifyScale = false;

    _demoTime = 0.0f;
    _demoTimeChanged = false;

    _followTimeBar = true;
    _displayWaveform = true;

    _pointItems = NULL;
    _curCurve = NULL;

    _widget = pWidget;

    _audiogramImage = NULL;
}

QSizeF CurveEditorScene::scale() const
{
    return _scale;
}

QPointF CurveEditorScene::offset() const {
    return _ofs;
}

void CurveEditorScene::drawBackground(QPainter* painter, const QRectF& rect)
{
    QGraphicsScene::drawBackground(painter, rect);
    painter->setBrush(Qt::NoBrush);

    setSceneRect(views().at(0)->geometry());

    if(_pointItems) for(int i = 0 ; i < _pointItems->size(); i++) {
        _pointItems->at(i)->recalculatePosition();
    }

    if(_ofs.x() > 0.0f)
    {
        _ofs.setX(0.0f);
        requestAudiogram();
    }

    if(_audiogramImage && _displayWaveform)
    {
        painter->drawImage(rect, *_audiogramImage);
    }

    // 0. Draw the axis + subaxis. =)))
    float sPos = 0.0f;
    if(rect.x() < 0.0f)
        sPos = -1.0f * fmod(fabs(rect.x()), _scale.width());
    else sPos = fmod(fabs(rect.x()), _scale.width());

    float sc = _scale.width() / 4.0f;
    for(float f = fmod(_ofs.x(), sc); f <= sceneRect().width() + fmod(_ofs.x(), sc); f += sc) {
        painter->setPen(QPen(QColor(56, 56, 56)));
        painter->drawLine(f, 0.0f, f, sceneRect().height());
    }
    sc = _scale.width();
    for(float f = fmod(_ofs.x(), sc); f <= sceneRect().width() + fmod(_ofs.x(), sc); f += sc) {
        painter->setPen(QPen(QColor(64, 64, 64)));
        painter->drawLine(f, 0.0f, f, sceneRect().height());
    }

    sc = _scale.height() / 4.0f;
    for(float f = fmod(_ofs.y(), sc); f <= sceneRect().height() + fmod(_ofs.y(), sc); f += sc) {
        painter->setPen(QPen(QColor(56, 56, 56)));
        painter->drawLine(0.0f, f, sceneRect().width(), f);
    }
    sc = _scale.height();
    for(float f = fmod(_ofs.y(), sc); f <= sceneRect().height() + fmod(_ofs.y(), sc); f += sc) {
        painter->setPen(QPen(QColor(64, 64, 64)));
        painter->drawLine(0.0f, f, sceneRect().width(), f);
    }

    painter->setPen(QPen(QColor(144, 144, 144)));
    painter->drawLine(_ofs.x(), 0.0f, _ofs.x(), sceneRect().height());
    painter->setPen(QPen(QColor(144, 144, 144)));
    painter->drawLine(0.0f, _ofs.y(), sceneRect().width(), _ofs.y());

    // 1. Sort points according to time
    QList<CurvePointItem*> unsortedPointItems;
    if(_pointItems) unsortedPointItems = QList<CurvePointItem*>(*_pointItems);
    QList<CurvePointItem*> sortedPointItems;
    while(!unsortedPointItems.isEmpty()) {
        CurvePointItem* minPoint = NULL;
        float minTime = 9999.0f;
        for(int i = 0; i < unsortedPointItems.size(); i++) {
            if(unsortedPointItems[i]->time() < minTime) {
                minPoint = unsortedPointItems[i];
                minTime = unsortedPointItems[i]->time();
            }
        }
        sortedPointItems.append(minPoint);
        unsortedPointItems.removeOne(minPoint);
    }
    if(!sortedPointItems.isEmpty()) {
        // 2. draw the curves.
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(QPen(QColor(128,128,255)));
        if(sortedPointItems.at(0)->x() > rect.x())
            painter->drawLine(0, sortedPointItems.at(0)->pos().y(), sortedPointItems.at(0)->pos().x(), sortedPointItems.at(0)->pos().y());

        if(sortedPointItems.last()->pos().x() < rect.x()+rect.width())
            painter->drawLine(sortedPointItems.last()->pos().x(), sortedPointItems.last()->pos().y(), rect.x()+rect.width(), sortedPointItems.last()->pos().y());

        for(int i = 0; i < sortedPointItems.size()-1; i++) {
            sortedPointItems.at(i)->setVisible(true);
            // hermite interpolation using those tangents - with a tad of a trick to prevent going back in time. =)
            int steps = 64;
            for(int j = 0; j < steps; j++) {
                float t1 = float(j)/float(steps);
                float t2 = float(j+1)/float(steps);

                qreal TangentX = sortedPointItems.at(i+1)->coord().x() - sortedPointItems.at(i)->coord().x();
                QPointF Tangent0 = QPointF(TangentX, TangentX * sortedPointItems.at(i)->tangent().y() / sortedPointItems.at(i)->tangent().x());
                QPointF Tangent1 = QPointF(TangentX, TangentX * sortedPointItems.at(i+1)->tangent().y() / sortedPointItems.at(i+1)->tangent().x());

                QPointF p1 = _interpolateHermite(
                    sortedPointItems.at(i)->coord(),
                    sortedPointItems.at(i+1)->coord(),
                    Tangent0,
                    Tangent1,
                    t1
                );
                p1 = QPointF(p1.x() * _scale.width(), p1.y() * -_scale.height());

                QPointF p2 = _interpolateHermite(
                    sortedPointItems.at(i)->coord(),
                    sortedPointItems.at(i+1)->coord(),
                    Tangent0,
                    Tangent1,
                    t2
                );
                p2 = QPointF(p2.x() * _scale.width(), p2.y() * -_scale.height());

                painter->drawLine(p1+_ofs, p2+_ofs);
            }
        }
        int i = sortedPointItems.size()-1;
        sortedPointItems.at(i)->setVisible(true);
    }

    // and at the very end, display a big fat green line on top of everything (demo time)
    painter->setPen(QPen(QColor(48,224,48,96), 2.0, Qt::SolidLine));
    painter->setBrush(Qt::NoBrush);
    painter->drawLine((_demoTime * _scale.width() + _ofs.x()), rect.y(), (_demoTime * _scale.width() + _ofs.x()), rect.y()+rect.height());

    QString strTime;
    int minutes = int(_demoTime) / 60;
    if(minutes < 10) strTime.append("0");
    strTime.append(QString::number(minutes)).append(":");
    if(fmod(_demoTime, 60.0f) < 10.0f) strTime.append("0");
    strTime.append(QString::number(fmod(_demoTime, 60.0f), 'f', 3));

    if(_demoTimeChanged)
    {
        _demoTimeChanged = false;
        if(_followTimeBar)
        {
            if((rect.x() + rect.width() - _ofs.x()) / _scale.width() < _demoTime)
            {
                while((rect.x() + rect.width() - _ofs.x()) / _scale.width() < _demoTime)
                    _ofs.setX(_ofs.x() - _scale.width()*2.0f);

                if(_ofs.x() > 0.0f)  _ofs.setX(0.0f);

                requestAudiogram();
            }

            else if((rect.x() - _ofs.x()) / _scale.width() > _demoTime)
            {
                while((rect.x() - _ofs.x()) / _scale.width() > _demoTime)
                    _ofs.setX(_ofs.x() + _scale.width()*2.0f);

                if(_ofs.x() > 0.0f)  _ofs.setX(0.0f);

                requestAudiogram();
            }
        }
    }

    float barOffset = 0.0f;
    if(abs(rect.x()+rect.width()-(_demoTime * _scale.width() + _ofs.x())) < 128) barOffset = -56.0f;

    painter->setFont(QFont(QString("Sans"), 8));
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(48,224,48,96));
    painter->drawRect(QRectF(QPointF((_demoTime * _scale.width() + _ofs.x())+barOffset, rect.height()-16.0f), QSizeF(56.0f, 16.0f)));
    painter->setPen(QColor(255,255,255,96));
    painter->setBrush(QColor(48,224,48,96));
    painter->drawText(QRectF(QPointF((_demoTime * _scale.width() + _ofs.x())+barOffset, rect.height()-16.0f), QSizeF(56.0f, 16.0f)), strTime, QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));
}

void CurveEditorScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    if(!_pointItems) return;

    bool thereIsAnItem = false;
    for(int i = 0; i < _pointItems->size(); i++) {
        if(_pointItems->at(i)->contains(_pointItems->at(i)->mapFromScene(event->scenePos()))) {
            thereIsAnItem = true;
            break;
        }
    }

    if(!thereIsAnItem) {
        CurvePointItem* cpi = new CurvePointItem(QPointF((event->scenePos().x()-_ofs.x()) / _scale.width(), (event->scenePos().y()-_ofs.y()) / -_scale.height()));
        _pointItems->append(cpi);

        addItem(cpi);
        cpi->update();
        update();
        views().at(0)->update();
    }

    QGraphicsScene::mouseDoubleClickEvent(event);
}

void CurveEditorScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    bool thereIsAnItem = false;
    if(_pointItems) for(int i = 0; i < _pointItems->size(); i++) {
        if(_pointItems->at(i)->contains(_pointItems->at(i)->mapFromScene(event->scenePos()))) {
            thereIsAnItem = true;
            break;
        }
    }

    if(!thereIsAnItem) {
        switch(event->button()) {
        case Qt::LeftButton:
            setDemoTime((event->scenePos().x()-_ofs.x()) / _scale.width());
            _widget->setMusicTime((event->scenePos().x()-_ofs.x()) / _scale.width());
            _modifyDemoTime = true;
            break;
        case Qt::MidButton:
            _modifyOfs = true;
            break;
        case Qt::RightButton:
            _modifyScale = true;
            break;
        default:
            break;
        }
    }
    update();
    QGraphicsScene::mousePressEvent(event);
}

void CurveEditorScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if(_modifyOfs || _modifyScale) requestAudiogram();

    _modifyDemoTime = false;
    _modifyOfs = false;
    _modifyScale = false;
    update();
    QGraphicsScene::mouseReleaseEvent(event);
}

void CurveEditorScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if(_modifyOfs) {
        _ofs += event->scenePos() - event->lastScenePos();
    } else if(_modifyScale) {
        QPointF p(QPointF(event->scenePos() - event->lastScenePos()));
        _scale += QSizeF(p.x(), p.y());
        if(_scale.width() < 8.0f) _scale.setWidth(8.0f);
        if(_scale.height() < 8.0f) _scale.setHeight(8.0f);
    }
    if(!_modifyOfs && !_modifyScale) {
        setDemoTime((event->scenePos().x()-_ofs.x()) / _scale.width());
        if(_demoTime < 0.0f) setDemoTime(0.0f);
        _widget->setMusicTime((event->scenePos().x()-_ofs.x()) / _scale.width());
    }

    update();
    QGraphicsScene::mouseMoveEvent(event);
}

void CurveEditorScene::deleteCurvePoint(CurvePointItem* cpi) {
    if(!_pointItems) return;
    removeItem(cpi);
    delete _pointItems->takeAt(_pointItems->indexOf(cpi));
    update();
}

QPointF CurveEditorScene::_interpolateHermite(QPointF p0, QPointF p1, QPointF r0, QPointF r1, float t) {
    float t2 = t*t;
    float t3 = t2*t;

    return (
        (2.0f*t3 - 3.0f*t2 + 1.0f) * p0 +
        (-2.0f*t3 + 3.0f*t2) * p1 +
        (t3 - 2.0f*t2 + t) * r0 +
        (t3 - t2) * r1
    );
}

float CurveEditorScene::simpleInterpolate(QList<CurvePointItem*>* list, float t) {
    if(!list) return 0.0f;
    if(list->isEmpty()) return 0.0f;
    if(list->count() == 1) return list->at(0)->coord().y();

    QList<CurvePointItem*> unsortedPointItems;
    if(_pointItems) unsortedPointItems = QList<CurvePointItem*>(*list);
    QList<CurvePointItem*> sortedPointItems;
    while(!unsortedPointItems.isEmpty()) {
        CurvePointItem* minPoint = NULL;
        float minTime = 9999.0f;
        for(int i = 0; i < unsortedPointItems.size(); i++) {
            if(unsortedPointItems[i]->time() < minTime) {
                minPoint = unsortedPointItems[i];
                minTime = unsortedPointItems[i]->time();
            }
        }
        sortedPointItems.append(minPoint);
        unsortedPointItems.removeOne(minPoint);
    }

    for(int i = 0; i < sortedPointItems.size(); i++) {
        if(!i) {
            if(t <= sortedPointItems.at(i)->coord().x()) return sortedPointItems.at(i)->coord().y();
        } else if(i == sortedPointItems.size()-1) return sortedPointItems.at(i)->coord().y();

        if(t > sortedPointItems.at(i)->coord().x() && t <= sortedPointItems.at(i+1)->coord().x()) {
            qreal TangentX = sortedPointItems.at(i+1)->coord().x() - sortedPointItems.at(i)->coord().x();
            QPointF Tangent0 = QPointF(TangentX, TangentX * sortedPointItems.at(i)->tangent().y() / sortedPointItems.at(i)->tangent().x());
            QPointF Tangent1 = QPointF(TangentX, TangentX * sortedPointItems.at(i+1)->tangent().y() / sortedPointItems.at(i+1)->tangent().x());

            return _interpolateHermite(
                sortedPointItems.at(i)->coord(),
                sortedPointItems.at(i+1)->coord(),
                Tangent0,
                Tangent1,
                (t-sortedPointItems.at(i)->coord().x()) / (sortedPointItems.at(i+1)->coord().x()-sortedPointItems.at(i)->coord().x())
            ).y();
        }
    }
    return 0.0f; // should never happen !
}

void CurveEditorScene::setCurrentCurve(QList<CurvePointItem*>* curve) {
    _curCurve = curve;
    if(_pointItems) foreach(CurvePointItem* cpi, *_pointItems) {
        removeItem(cpi);
    }

    _pointItems = curve;

    foreach(CurvePointItem* cpi, *_pointItems) {
        addItem(cpi);
        cpi->update();
    }
    update();
}

void CurveEditorScene::setDemoTime(float t)
{
    _demoTime = t;
    _demoTimeChanged = true;
    _widget->setDemoTime(t, true);
    update();
}

float CurveEditorScene::demoTime() const
{
    return _demoTime;
}

void CurveEditorScene::setFollowTimeBar(bool b)
{
    _followTimeBar = b;
}

void CurveEditorScene::setDisplayWaveform(bool b)
{
    _displayWaveform = b;
    update();
}

void CurveEditorScene::getNewAudiogram()
{
    requestAudiogram();
    update();
}

QList<CurvePointItem*>* CurveEditorScene::curve() const
{
    return _curCurve;
}

void CurveEditorScene::cutSelectedPoints()
{
    if(!_pointItems) return;

    QClipboard* cb = QApplication::clipboard();
    cb->clear();

    QString data;

    CurvePointItem* firstItem = NULL;

    QList<CurvePointItem*> unsortedPointItems;
    if(_pointItems) unsortedPointItems = QList<CurvePointItem*>(*_pointItems);
    QList<CurvePointItem*> sortedPointItems;
    while(!unsortedPointItems.isEmpty()) {
        CurvePointItem* minPoint = NULL;
        float minTime = 9999.0f;
        for(int i = 0; i < unsortedPointItems.size(); i++) {
            if(unsortedPointItems[i]->time() < minTime) {
                minPoint = unsortedPointItems[i];
                minTime = unsortedPointItems[i]->time();
            }
        }
        sortedPointItems.append(minPoint);
        unsortedPointItems.removeOne(minPoint);
    }

    QList<CurvePointItem*> pointsToDelete;

    foreach(QGraphicsItem* it, sortedPointItems)
    {
        CurvePointItem* cpit = dynamic_cast<CurvePointItem*>(it);
        if(!cpit) continue;
        if(!cpit->isSelected()) continue;
        if(!firstItem) firstItem = cpit;
        float delta = cpit->coord().x() - firstItem->coord().x();

        data.append(QString::number(delta, 'f', 6));
        data.append(QString("\t"));
        data.append(QString::number(cpit->coord().y(), 'f', 6));
        data.append(QString("\t"));
        data.append(QString::number(cpit->tangent().x(), 'f', 6));
        data.append(QString("\t"));
        data.append(QString::number(cpit->tangent().y(), 'f', 6));
        data.append(QString("\n"));

        pointsToDelete.append(cpit);
    }
    cb->setText(data.prepend("FRQ_PRDC_DATA\n"));

    for(int i = 0; i < pointsToDelete.size(); i++)
    {
        deleteCurvePoint(pointsToDelete.at(i));
    }
}

void CurveEditorScene::copySelectedPoints()
{
    if(!_pointItems) return;

    QClipboard* cb = QApplication::clipboard();
    cb->clear();

    QString data;

    CurvePointItem* firstItem = NULL;

    QList<CurvePointItem*> unsortedPointItems;
    if(_pointItems) unsortedPointItems = QList<CurvePointItem*>(*_pointItems);
    QList<CurvePointItem*> sortedPointItems;
    while(!unsortedPointItems.isEmpty()) {
        CurvePointItem* minPoint = NULL;
        float minTime = 9999.0f;
        for(int i = 0; i < unsortedPointItems.size(); i++) {
            if(unsortedPointItems[i]->time() < minTime) {
                minPoint = unsortedPointItems[i];
                minTime = unsortedPointItems[i]->time();
            }
        }
        sortedPointItems.append(minPoint);
        unsortedPointItems.removeOne(minPoint);
    }

    foreach(QGraphicsItem* it, sortedPointItems)
    {
        CurvePointItem* cpit = dynamic_cast<CurvePointItem*>(it);
        if(!cpit) continue;
        if(!cpit->isSelected()) continue;
        if(!firstItem) firstItem = cpit;
        float delta = cpit->coord().x() - firstItem->coord().x();

        data.append(QString::number(delta, 'f', 6));
        data.append(QString("\t"));
        data.append(QString::number(cpit->coord().y(), 'f', 6));
        data.append(QString("\t"));
        data.append(QString::number(cpit->tangent().x(), 'f', 6));
        data.append(QString("\t"));
        data.append(QString::number(cpit->tangent().y(), 'f', 6));
        data.append(QString("\n"));
    }
    cb->setText(data.prepend("FRQ_PRDC_DATA\n"));
}

void CurveEditorScene::pasteSelectedPoints()
{
    if(!_pointItems) return;

    QClipboard* cb = QApplication::clipboard();

    QString dataStr = cb->text();
    if(dataStr.isEmpty()) return;

    QStringList data = dataStr.split("\n");
    if(data[0] != QString("FRQ_PRDC_DATA")) return;
    data.removeFirst();
    data.removeLast();

    foreach(QString point, data)
    {
        QStringList strCoords = point.split("\t");
        float cx = _demoTime + strCoords.at(0).toFloat();
        float cy = strCoords.at(1).toFloat();

        float tx = _demoTime + strCoords.at(2).toFloat();
        float ty = strCoords.at(3).toFloat();

        CurvePointItem* ncpi = new CurvePointItem();
        ncpi->setCoord(QPointF(cx, cy));
        ncpi->setTangent(QPointF(tx, ty));
        _pointItems->append(ncpi);
        _curCurve->append(ncpi);
        addItem(ncpi);
    }

    update();
}

void CurveEditorScene::viewResized()
{
    requestAudiogram();
}

void CurveEditorScene::requestAudiogram()
{
    if(!_displayWaveform) return;

    float leftTime = -float(_ofs.x()) / float(_scale.width());
    float rightTime = leftTime + (float(sceneRect().width()) / float(_scale.width()));

    if(leftTime < 0.0f || rightTime < 0.0f || leftTime >= rightTime) return;

    QFmodEx* fmodEx = QFmodEx::instance();
    switch(fmodEx->state())
    {
    case QFmodEx::Idle:
    case QFmodEx::Loading:
    case QFmodEx::Error:
        return;
    default:
        break;
    }

    QImage* img = fmodEx->audiogram(leftTime, rightTime, int(sceneRect().width()), int(sceneRect().height()));
    if(!img) return;

    _audiogramImage = img;
}

#endif