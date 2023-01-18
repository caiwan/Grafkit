#include <QGraphicsScene>

#include "AnimationEditorWidget.h"
#include "ui_AnimationEditorWidget.h"

#include "utils/tree/treemodel.h"
#include "AnimationTreeModel.h"

using namespace Idogep;

AnimationEditorWidget::AnimationEditorWidget(QWidget* parent) : QDockWidget(parent)
, AnimationEditorView()
, ui(new Ui::AnimationEditorWidget)
{
    ui->setupUi(this);

    setFocusPolicy(Qt::ClickFocus);


    // slots and signals crap thing
    connect(ui->btn_play, SIGNAL(clicked()), this, SLOT(playPressedSlot()));
    connect(ui->btn_stop, SIGNAL(clicked()), this, SLOT(stopPressedSlot()));

    connect(ui->treeView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(itemClickedSlot(const QModelIndex &)));
    connect(ui->treeView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(itemDoubleClickedSlot(const QModelIndex &)));
}

AnimationEditorWidget::~AnimationEditorWidget() { delete ui; }

void AnimationEditorWidget::SetGraphicsScene(QGraphicsScene* graphicsScene) const
{
    assert(graphicsScene);
    ui->graphicsView->setScene(graphicsScene);
}

void AnimationEditorWidget::SetPointEditorWidget(QWidget* widget)
{
    // safe replace widget 
    int oldid = ui->horizontalLayout->indexOf(ui->pointEditorWidget);

    ui->horizontalLayout->removeWidget(ui->pointEditorWidget);
    ui->pointEditorWidget->setParent(nullptr);
    delete ui->pointEditorWidget;

    ui->pointEditorWidget = widget;
    ui->pointEditorWidget->setParent(ui->dockWidgetContents);
    ui->horizontalLayout->insertWidget(oldid, widget);
}

void AnimationEditorWidget::resizeEvent(QResizeEvent* event)
{
    qDebug() << "Resize AEW" << event->size();

    View* scene = dynamic_cast<View*>(ui->graphicsView->scene());
    if (scene)
        scene->RequestRefreshView(true);

    QWidget::resizeEvent(event);
}


void AnimationEditorWidget::RefreshView(bool force) {
}

void AnimationEditorWidget::UpdateAnimationModel(TreeModel* model)
{
    // this will force update
    ui->treeView->setModel(nullptr);
    ui->treeView->setModel(model);
}

// ========================================================================================================


void AnimationEditorWidget::itemClickedSlot(const QModelIndex& index)
{
    const TreeItem* item = reinterpret_cast<TreeItem *>(index.internalPointer());
    const AnimationChannelItem* animationItem = dynamic_cast<const AnimationChannelItem*>(item);
    if (!animationItem)
        return;

    Grafkit::Animation::TrackRef track = animationItem->GetTrack();
    Grafkit::Animation::ChannelRef channel = animationItem->GetChannel();
    onChannelSelected(track, animationItem->GetTrackId(), channel);

    RefreshView(false);
}

void AnimationEditorWidget::itemDoubleClickedSlot(const QModelIndex& index)
{
    // TBD if needed
    // add stuff here 
}


// ========================================================================================================
