#include "scenegraphview.h"
#include "ui_scenegraphview.h"

SceneGraphView::SceneGraphView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SceneGraphView)
{
    ui->setupUi(this);
}

SceneGraphView::~SceneGraphView()
{
    delete ui;
}
