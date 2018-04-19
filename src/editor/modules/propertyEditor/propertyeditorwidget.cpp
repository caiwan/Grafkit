#include "PropertyEditorWidget.h"
#include "ui_PropertyEditorWidget.h"

using namespace Idogep;

PropertyEditorWidget::PropertyEditorWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::PropertyEditorWidget)
{
    ui->setupUi(this);
}

PropertyEditorWidget::~PropertyEditorWidget()
{
    delete ui;
}
