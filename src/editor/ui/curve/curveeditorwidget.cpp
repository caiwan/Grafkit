#include "Event.h"

#include "curveeditorwidget.h"
#include "ui_curveeditorwidget.h"

#include "curveeditorscene.h"

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
