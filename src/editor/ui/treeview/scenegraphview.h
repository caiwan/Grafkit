#ifndef SCENEGRAPHVIEW_H
#define SCENEGRAPHVIEW_H

#include <QWidget>

namespace Ui {
class SceneGraphView;
}

class SceneGraphView : public QWidget
{
    Q_OBJECT

public:
    explicit SceneGraphView(QWidget *parent = 0);
    ~SceneGraphView();

private:
    Ui::SceneGraphView *ui;
};

#endif // SCENEGRAPHVIEW_H
