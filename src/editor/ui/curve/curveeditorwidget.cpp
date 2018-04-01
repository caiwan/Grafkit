#include "Event.h"

#include "curveeditorwidget.h"
#include "ui_curveeditorwidget.h"

#include "curveeditorscene.h"

#include "../treeview/treemodel.h"

using namespace Idogep;

CurveEditorWidget::CurveEditorWidget(QWidget* parent) : QDockWidget(parent), ui(new Ui::CurveEditorWidget), CurveDocument()
{
	ui->setupUi(this);
	m_scene = new CurveEditorScene(this);
	m_scene->setDocument(this);
	ui->graphicsView->setScene(m_scene);

	// slots and signals crap thing 
	connect(ui->btn_play, SIGNAL(clicked()), this, SLOT(playPressedSlot()));
	connect(ui->btn_stop, SIGNAL(clicked()), this, SLOT(stopPressedSlot()));
	connect(ui->btn_playbackOptions, SIGNAL(clicked()), this, SLOT(optionsPressedSlot()));

	//onAnimationModelUpdated +=
}

CurveEditorWidget::~CurveEditorWidget()
{
	delete ui;
}

void Idogep::CurveEditorWidget::resizeEvent(QResizeEvent * event)
{
	m_scene->viewResized(event);
	QWidget::resizeEvent(event);
}

void Idogep::CurveEditorWidget::refreshView(bool force)
{
	if (force) {
		m_scene->clear();
		clearAudiogram();
		addCurveToScene(m_scene);
		// addCursorsToScene(m_scene); // TODO
		m_scene->refreshView();
	}
	else {
		addCurveToScene(m_scene);
		m_scene->update();
	}
}

void Idogep::CurveEditorWidget::UpdateAnimationModel(TreeModel * model)
{
	// this will force update
	ui->treeView->setModel(nullptr);
	ui->treeView->setModel(model);
}

// ========================================================================================================


void Idogep::CurveEditorWidget::playbackChanged(bool isPlaying)
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






// ========================================================================================================

#include "../treeview/treemodel.h"
#include "../treeview/treeitem.h"

using namespace Grafkit;
using namespace Idogep;

Idogep::AnimationChannelItem::AnimationChannelItem(const QList<QVariant>& data, TreeItem * parentItem) : TreeItem(data, parentItem)
{
}

Idogep::ManageAnimationsRole::ManageAnimationsRole() : m_animationListModel(nullptr)
{
}

// ========================================================================================================


namespace {
	class AnimationTreeModel : public TreeModel {
	public:
		AnimationTreeModel(Grafkit::AnimationRef animation);

	protected:
		virtual QStringList Header();
		virtual	void Build(TreeItem * parentItem);

	protected:
		void BuildTrack(TreeItem * const &parentItem, Ref<Animation::Track>& track);

	private:
		Grafkit::AnimationRef m_animation;
	};
	AnimationTreeModel::AnimationTreeModel(Grafkit::AnimationRef animation) : m_animation(animation)
	{
	}

	QStringList AnimationTreeModel::Header()
	{
		return QStringList() << "Channel";
	}

	void AnimationTreeModel::Build(TreeItem * parentItem)
	{

		for (size_t i = 0; i < m_animation->GetTrackCount(); i++) {
			Ref<Animation::Track> track = m_animation->GetTrack(i);
			
			QVariantList data;
			data << QString::fromStdString(track->GetName());
			TreeItem *item = new TreeItem(data, parentItem);
			
			parentItem->addChild(item);
			
			BuildTrack(item, track);
		}
	}
	void AnimationTreeModel::BuildTrack(TreeItem * const & parentItem, Ref<Animation::Track> &track)
	{
		for (size_t i = 0; i < track->GetChannelCount(); i++) {
			Ref<Animation::Channel> channel = track->GetChannel(i);
			
			QVariantList data;
			data << QString::fromStdString(channel->GetName());
			AnimationChannelItem *item = new AnimationChannelItem(data, parentItem);
			item->SetChannel(channel);

			parentItem->addChild(item);
		}
	}
}

// ========================================================================================================

void Idogep::ManageAnimationsRole::AnimationChangedEvent(Grafkit::AnimationRef animation)
{
	m_animation = animation;
	TreeModel * oldModel = m_animationListModel;
	m_animationListModel = new AnimationTreeModel(m_animation);
	m_animationListModel->BuildModel();
	UpdateAnimationModel(m_animationListModel);
	if (oldModel)
		delete oldModel;
}

