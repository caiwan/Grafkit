#include "scenegraphviewwidget.h"
#include "ui_scenegraphviewwidget.h"

using namespace Idogep;

Idogep::SceneGraphViewWidget::SceneGraphViewWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::SceneGraphViewWidget)
{
    ui->setupUi(this);
}

Idogep::SceneGraphViewWidget::~SceneGraphViewWidget()
{
    delete ui;
}
