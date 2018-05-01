#include "utils/Event.h"

#include "curveeditorwidget.h"
#include "ui_curveeditorwidget.h"

#include "curveeditorscene.h"

#include "utils/tree/treemodel.h"

using namespace Idogep;

CurveEditorWidget::CurveEditorWidget(QWidget* parent) : QDockWidget(parent), CurveEditorView(), ui(new Ui::CurveEditorWidget)
{
	ui->setupUi(this);
#if 0
	m_scene = new CurveEditorScene(this);
	m_scene->SetDocument(this);
	ui->graphicsView->setScene(m_scene);

	// slots and signals crap thing 
	connect(ui->btn_play, SIGNAL(clicked()), this, SLOT(playPressedSlot()));
	connect(ui->btn_stop, SIGNAL(clicked()), this, SLOT(stopPressedSlot()));
	connect(ui->btn_playbackOptions, SIGNAL(clicked()), this, SLOT(optionsPressedSlot()));

	connect(ui->treeView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(itemClickedSlot(const QModelIndex &)));
	connect(ui->treeView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(itemDoubleClickedSlot(const QModelIndex &)));
#endif
}

CurveEditorWidget::~CurveEditorWidget()
{
	delete ui;
}

void Idogep::CurveEditorWidget::resizeEvent(QResizeEvent * event)
{
#if 0
	m_scene->viewResized(event);
	QWidget::resizeEvent(event);
#endif
}

void Idogep::CurveEditorWidget::RefreshView(bool force)
{
#if 0
  	if (force) {
		m_scene->clear();
		ClearAudiogram();
		AddCurveToScene(m_scene);
		// addCursorsToScene(m_scene); // TODO
		m_scene->RefreshView();
	}
	else {
		m_scene->clear();
		AddCurveToScene(m_scene);
		m_scene->update();
	}

#endif

}

void Idogep::CurveEditorWidget::UpdateAnimationModel(TreeModel * model)
{
	// this will force update
	ui->treeView->setModel(nullptr);
	ui->treeView->setModel(model);
}

// ========================================================================================================


void Idogep::CurveEditorWidget::PlaybackChanged(bool isPlaying)
{
	if (isPlaying) {
		ui->btn_play->setText(QApplication::translate("CurveEditorWidget", "Pause", nullptr));
	}
	else {
		ui->btn_play->setText(QApplication::translate("CurveEditorWidget", "Play", nullptr));
	}
}

void Idogep::CurveEditorWidget::demoTimeChanged(float time)
{
	// ...
	// set cursor here 
}


void Idogep::CurveEditorWidget::itemClickedSlot(const QModelIndex & index)
{
#if 0
  	TreeItem * item = reinterpret_cast<TreeItem *> (index.internalPointer());
	AnimationChannelItem * animationItem = dynamic_cast<AnimationChannelItem*>(item);
	if (!animationItem)
		return;

	SetChannel(animationItem->GetChannel());
	RefreshView(false);
#endif

}

void Idogep::CurveEditorWidget::itemDoubleClickedSlot(const QModelIndex & index)
{
}


// ========================================================================================================

#include "utils/tree/treemodel.h"
#include "utils/tree/treeitem.h"

using namespace Grafkit;
using namespace Idogep;

Idogep::AnimationChannelItem::AnimationChannelItem(const QList<QVariant>& data, TreeItem * parentItem) : TreeItem(data, parentItem)
{
}

Idogep::ManageAnimationsRole::ManageAnimationsRole() : m_animationListModel(nullptr)
{
}

// ========================================================================================================

