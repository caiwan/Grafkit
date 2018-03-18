#include "scenegraphviewwidget.h"
#include "ui_scenegraphviewwidget.h"

#include "treemodel.h"

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

void Idogep::SceneGraphViewWidget::setModel(TreeModel * model)
{
	// this will force refresh, probaly.
	ui->treeView->setModel(nullptr);
	ui->treeView->setModel(model);
}
