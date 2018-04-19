#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QDockWidget>

namespace Ui {
	class LogWidget;
}

namespace Idogep {
	class LogWidget : public QDockWidget
	{
		Q_OBJECT

	public:
		explicit LogWidget(QWidget *parent = 0);
		~LogWidget();

		void UpdateLog(QString log);


	private:
		Ui::LogWidget *ui;
	};
}

#endif // LOGWIDGET_H
