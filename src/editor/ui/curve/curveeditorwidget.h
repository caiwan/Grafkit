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

#ifndef CURVEEDITORWIDGET_HH
#define CURVEEDITORWIDGET_HH

#include <QtGui>

namespace Ui {
    class CurveEditorWidget;
}

class CurveEditorScene;
class CurvePointItem;

class CurveEditorWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit CurveEditorWidget(QWidget *parent = 0);
    ~CurveEditorWidget();

    QList<CurvePointItem*>* getCurve(QString name);
    static void preinitCurves(QStringList& parameters, QList<float*> mappedValueList, QList<float> defaultValues);

    virtual void resizeEvent(QResizeEvent* event);

    void setMusicTime(float t);

    void addPointToCurve(CurvePointItem* cpi, QString& curveName);

    void Clear();

public slots:
    void setBackgroundImage(QImage img);

    void setCurve(QString parameterName);
    void setDemoTime(float t, bool manual = false);
    void setFollowTimeBar(bool b);
    void setDisplayWaveform(bool b);
    void getNewAudiogram();

    void makeWaypoint(float px, float py, float pz, float tx, float ty, float tz);

    void cutSelectedPoints();
    void copySelectedPoints();
    void pasteSelectedPoints();

signals:
    void musicTimeChanged(unsigned int t);

private:
    Ui::CurveEditorWidget *ui;
    CurveEditorScene* _ces;
};

#endif // CURVEEDITORWIDGET_HH

#endif
