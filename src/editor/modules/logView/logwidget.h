#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QDockWidget>

#include "LogModule.h"

namespace Ui {
	class LogWidget;
}

namespace Idogep {
	class LogWidget : public QDockWidget, public LogView
	{
		Q_OBJECT
	public:
		explicit LogWidget(QWidget *parent = nullptr);
		~LogWidget();

	    void UpdateLog(std::string log) override;

	protected:
	    void RefreshView(bool force) override {}

	private:
		Ui::LogWidget *ui;
	};
}

#endif // LOGWIDGET_H
