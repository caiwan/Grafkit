#include "scenegraphviewwidget.h"

#include "utils/resource.h"
#include "utils/tree/treemodel.h"

#include "ui_scenegraphviewwidget.h"

using namespace Idogep;
using namespace Grafkit;

SceneGraphViewWidgetModel::SceneGraphViewWidgetModel() :
    m_model(nullptr) 
{
}

SceneGraphViewWidget::SceneGraphViewWidget(QWidget* parent)
    : QDockWidget(parent)
    , OutlineView()
    , ui(new Ui::SceneGraphViewWidget)
{
    ui->setupUi(this);
    connect(ui->treeView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(itemClickedSlot(const QModelIndex &)));
    connect(ui->treeView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(itemDoubleClickedSlot(const QModelIndex &)));
}

SceneGraphViewWidget::~SceneGraphViewWidget() { delete ui; }

void SceneGraphViewWidget::SetModel(SceneGraphViewWidgetModel* model)
{
    // this will force refresh, probaly.
    ui->treeView->setModel(nullptr);
    ui->treeView->setModel(model->GetModel());
}


void SceneGraphViewWidget::RefreshView(bool force)
{
    // ...
}

// ReSharper disable CppInconsistentNaming
void SceneGraphViewWidget::itemClickedSlot(const QModelIndex& index) const
{
    assert(m_outlineModule);
    m_outlineModule->onItemSelected(static_cast<TreeItem*>(index.internalPointer()));
}

void SceneGraphViewWidget::itemDoubleClickedSlot(const QModelIndex& index) const
{
    assert(m_outlineModule);
    // TODO Add if needed
    m_outlineModule->onItemSelected(static_cast<TreeItem*>(index.internalPointer()));

}
// ReSharper enable CppInconsistentNaming

