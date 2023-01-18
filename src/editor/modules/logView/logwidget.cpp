#include "logwidget.h"
#include "ui_logwidget.h"

#include <QTimer>
#include <QStyle>
#include <QScrollBar>

#include <QDebug>

using namespace Idogep;

LogWidget::LogWidget(QWidget* parent) :
    QDockWidget(parent)
    , LogView()
    , ui(new Ui::LogWidget)
{
    ui->setupUi(this);
    ui->textEdit->setProperty("class", "monospace");
}

LogWidget::~LogWidget() { delete ui; }

void LogWidget::UpdateLog(std::string log)
{
    m_messages.push(log);
    QTimer::singleShot(0, this, SLOT(UpdateLogSlot()));
}

void LogWidget::UpdateLogSlot()
{
    while (!m_messages.empty())
    {
        std::string log = m_messages.front();
        ui->textEdit->appendPlainText(QString::fromStdString(log));
        m_messages.pop();
    }
    ui->textEdit->verticalScrollBar()->setValue(ui->textEdit->verticalScrollBar()->maximum());
}
