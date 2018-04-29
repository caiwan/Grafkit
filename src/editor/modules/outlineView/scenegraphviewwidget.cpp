#include "scenegraphviewwidget.h"

#include "utils/resource.h"

#include "ui_scenegraphviewwidget.h"

#include "utils/tree/treemodel.h"

#include "models/Document.h"

using namespace Idogep;
using namespace Grafkit;

Idogep::SceneGraphViewWidget::SceneGraphViewWidget(QWidget *parent) :
	QDockWidget(parent),
	ui(new Ui::SceneGraphViewWidget)
{
	ui->setupUi(this);

	connect(ui->treeView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(itemClickedSlot(const QModelIndex &)));
	connect(ui->treeView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(itemDoubleClickedSlot(const QModelIndex &)));
}

Idogep::SceneGraphViewWidget::~SceneGraphViewWidget()
{
	delete ui;
}

void Idogep::SceneGraphViewWidget::SetModel(Grafkit::IResource * modelResource)
{
	Ref<Resource<SceneGraphViewWidgetModel>> model = dynamic_cast<Resource<SceneGraphViewWidgetModel>*>(modelResource);
	assert(model.Valid());
	assert(model->Valid());

	// this will force refresh, probaly.
	ui->treeView->setModel(nullptr);
	ui->treeView->setModel((*model)->GetModel());
}


void Idogep::SceneGraphViewWidget::itemClickedSlot(const QModelIndex & index)
{
	onItemSelected(static_cast<TreeItem*>(index.internalPointer()));
}

void Idogep::SceneGraphViewWidget::RefreshView(bool force)
{
}

void Idogep::SceneGraphViewWidget::itemDoubleClickedSlot(const QModelIndex & index)
{
	onItemSelected(static_cast<TreeItem*>(index.internalPointer()));
}
