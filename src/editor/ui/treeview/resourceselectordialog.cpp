#include "resourceselectordialog.h"
#include "ui_resourceselectordialog.h"

ResourceSelectorDialog::ResourceSelectorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResourceSelectorDialog)
{
    ui->setupUi(this);
}

ResourceSelectorDialog::~ResourceSelectorDialog()
{
    delete ui;
}
