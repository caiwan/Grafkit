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

void Idogep::SceneGraphViewWidget::SetModel(Ref<Referencable> model)
{
	//DEBUG_ASSERT(model.Valid());
	//Role::HasSceneGraphRole* hasScenegraph = static_cast<Role::HasSceneGraphRole*>(model.Get());
	//DEBUG_ASSERT(hasScenegraph);
	//Grafkit::SceneGraphRef sccenegraph = hasScenegraph->GetScenegraph();
	//RefreshView(false);
}

//void Idogep::SceneGraphViewWidget::setModel(TreeModel * model)
//{
//	// this will force refresh, probaly.
//	//ui->treeView->setModel(nullptr);
//	//ui->treeView->setModel(model);
//}

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
