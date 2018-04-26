#include "logwidget.h"
#include "ui_logwidget.h"

#include <qscrollbar.h>

using namespace Idogep;

Idogep::LogWidget::LogWidget(QWidget *parent) :
	QDockWidget(parent),
	ui(new Ui::LogWidget)
{
	ui->setupUi(this);

	const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
	ui->textEdit->setFont(fixedFont);
}

Idogep::LogWidget::~LogWidget()
{
	delete ui;
}

void Idogep::LogWidget::UpdateLog(std::string log)
{
	ui->textEdit->appendPlainText(QString::fromStdString(log));
	ui->textEdit->verticalScrollBar()->setValue(ui->textEdit->verticalScrollBar()->maximum());
}
