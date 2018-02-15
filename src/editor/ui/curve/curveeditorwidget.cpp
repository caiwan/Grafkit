/***************************************************************************
 *   Copyright (C) 2012                                                    *
 *   Anatole Duprat <anatole.duprat@gmail.com>                             *
 *   Charles Bulckaen  <xtrium@frequency.fr>                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public License           *
 *   as published by the Free Software Foundation; either version 2        *
 *   of the License, or (at your option) any later version.                *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA          *
 ***************************************************************************/

#if 0

#include "curveeditorwidget.hh"
#include "ui_curveeditorwidget.h"

#include "curveeditorscene.hh"

static QMap<QString, QList<CurvePointItem*>*> _curves;
static QMap<QString, float*> _curvesMappedValues;

CurveEditorWidget::CurveEditorWidget(QWidget* parent) : QDockWidget(parent), ui(new Ui::CurveEditorWidget)
{
    ui->setupUi(this);
    _ces = new CurveEditorScene(this);
    ui->graphicsView->setScene(_ces);
}

CurveEditorWidget::~CurveEditorWidget()
{
    delete ui;
}


QList<CurvePointItem*>* CurveEditorWidget::getCurve(QString name)
{
    return _curves[name];
}

void CurveEditorWidget::setBackgroundImage(QImage img)
{
    Q_UNUSED(img);
}

void CurveEditorWidget::setCurve(QString parameterName) {
    if(_curves.find(parameterName) != _curves.end()) {
        setWindowTitle(QString(" :: Curve Editor").prepend(parameterName));
        _ces->setCurrentCurve(_curves[parameterName]);
    }
}

void CurveEditorWidget::preinitCurves(QStringList& parameters, QList<float*> mappedValueList, QList<float> defaultValues) {
    _curves.clear();
    for(int i = 0; i < parameters.size(); i++) {
        QString param(parameters[i]);
        QList<CurvePointItem*>* list = new QList<CurvePointItem*>();
        list->append(new CurvePointItem(QPointF(0.0f, defaultValues.at(i))));
        _curves.insert(param, list);
        _curvesMappedValues.insert(param, mappedValueList.at(i));
    }
}

void CurveEditorWidget::setDemoTime(float t, bool manual)
{
    if(!manual) _ces->setDemoTime(t);
    QStringList cmvKeys(_curvesMappedValues.keys());
    foreach(QString key, cmvKeys) {
        if(&_curvesMappedValues[key]) {
            QList<CurvePointItem*>* curve = _curves[key];
            *(_curvesMappedValues[key]) = _ces->simpleInterpolate(curve, t);
        }
    }
}

void CurveEditorWidget::setFollowTimeBar(bool b)
{
    _ces->setFollowTimeBar(b);
}

void CurveEditorWidget::setDisplayWaveform(bool b)
{
    _ces->setDisplayWaveform(b);
}

void CurveEditorWidget::getNewAudiogram()
{
    _ces->getNewAudiogram();
}

void CurveEditorWidget::setMusicTime(float t)
{
    int time = (int)(1000.0f * t);
    time = time < 0 ? 0 : time;
    emit musicTimeChanged(time);
}

void CurveEditorWidget::addPointToCurve(CurvePointItem* cpi, QString& curveName)
{
    if(_curves.contains(curveName))
    {
        foreach(CurvePointItem* ocpi, *_curves[curveName.toStdString().c_str()])
        {
            if(ocpi->coord().x() == cpi->coord().x()) {
                _curves[curveName.toStdString().c_str()]->removeOne(ocpi);
                delete ocpi;
            }
        }

        _curves[curveName.toStdString().c_str()]->append(cpi);
        if(_ces->curve() == _curves[curveName.toStdString().c_str()])
            _ces->setCurrentCurve(_curves[curveName.toStdString().c_str()]);
    }
}

void CurveEditorWidget::makeWaypoint(float px, float py, float pz, float tx, float ty, float tz)
{
    float t = _ces->demoTime();
    _curves["Camera.Position.X"]->append(new CurvePointItem(QPointF(t, px)));
    _curves["Camera.Position.Y"]->append(new CurvePointItem(QPointF(t, py)));
    _curves["Camera.Position.Z"]->append(new CurvePointItem(QPointF(t, pz)));
    _curves["Camera.Target.X"]->append(new CurvePointItem(QPointF(t, tx)));
    _curves["Camera.Target.Y"]->append(new CurvePointItem(QPointF(t, ty)));
    _curves["Camera.Target.Z"]->append(new CurvePointItem(QPointF(t, tz)));

    if(_ces->curve())
    {
        _ces->setCurrentCurve(_ces->curve());
        _ces->update();
    }
}

void CurveEditorWidget::cutSelectedPoints()
{
    _ces->cutSelectedPoints();
}

void CurveEditorWidget::copySelectedPoints()
{
    _ces->copySelectedPoints();
}

void CurveEditorWidget::pasteSelectedPoints()
{
    _ces->pasteSelectedPoints();
}

void CurveEditorWidget::resizeEvent(QResizeEvent* event)
{
    _ces->viewResized();
    QWidget::resizeEvent(event);
}


void CurveEditorWidget::Clear()
{
    for(int i=0; i<_curves.values().size(); ++i)
        _curves.values().at( i )->clear();
}

#endif
