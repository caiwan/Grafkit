#if 0

#pragma once 

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

#endif
