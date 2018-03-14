#include "PropertyEditorWidget.h"
#include "ui_PropertyEditorWidget.h"

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
