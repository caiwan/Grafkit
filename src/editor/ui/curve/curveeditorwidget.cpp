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
	//onResize += Delegate(m_ces, &CurveEditorScene::viewResized);
	//onResize += Delegate(this, &QWidget::resizeEvent);
}

CurveEditorWidget::~CurveEditorWidget()
{
    delete ui;
}
