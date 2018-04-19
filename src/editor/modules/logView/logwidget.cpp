#include "logwidget.h"
#include "ui_logwidget.h"

#include <qscrollbar.h>

using namespace Idogep;

Idogep::LogWidget::LogWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::LogWidget)
{
    ui->setupUi(this);
}

Idogep::LogWidget::~LogWidget()
{
    delete ui;
}

void Idogep::LogWidget::UpdateLog(QString log)
{
	ui->textEdit->appendPlainText(log);
	ui->textEdit->verticalScrollBar()->setValue(ui->textEdit->verticalScrollBar()->maximum());
}
