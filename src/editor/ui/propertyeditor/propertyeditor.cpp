#include "propertyeditor.h"
#include "ui_propertyeditor.h"

PropertyEditor::PropertyEditor(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::PropertyEditor)
{
    ui->setupUi(this);
}

PropertyEditor::~PropertyEditor()
{
    delete ui;
}
