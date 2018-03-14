#include "resourceselector.h"
#include "ui_resourceselector.h"

ResourceSelector::ResourceSelector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResourceSelector)
{
    ui->setupUi(this);
}

ResourceSelector::~ResourceSelector()
{
    delete ui;
}
