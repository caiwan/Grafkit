#include "curveeditorwidget.h"
#include "ui_curveeditorwidget.h"

#include "curveeditorscene.h"

#include "Event.h"

using namespace Idogep;

CurveEditorWidget::CurveEditorWidget(QWidget* parent) : QDockWidget(parent), ui(new Ui::CurveEditorWidget)
{
    ui->setupUi(this);
    m_ces = new CurveEditorScene(this);
    ui->graphicsView->setScene(m_ces);

	// Event map
	// .. 
}

CurveEditorWidget::~CurveEditorWidget()
{
    delete ui;
}

void Idogep::CurveEditorWidget::resizeEvent(QResizeEvent * event)
{
	m_ces->viewResized(event);
	QWidget::resizeEvent(event);
}
