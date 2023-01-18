#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QDockWidget>
#include <queue>

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

        private slots:
            void UpdateLogSlot() ;
	private:
        std::queue<std::string> m_messages;
		Ui::LogWidget *ui;
	};
}

#endif // LOGWIDGET_H
