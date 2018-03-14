#include "logwidget.h"
#include "ui_logwidget.h"

LogWidget::LogWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::LogWidget)
{
    ui->setupUi(this);
}

LogWidget::~LogWidget()
{
    delete ui;
}
