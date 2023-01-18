#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QDockWidget>

#include "ViewModule.h"

namespace Ui {
	class LogWidget;
}

namespace Idogep {
	class LogWidget : public QDockWidget, public virtual Referencable, public View
	{
		Q_OBJECT
	public:
		explicit LogWidget(QWidget *parent = 0);
		~LogWidget();

		void UpdateLog(std::string log);

		virtual void SetModel(Grafkit::IResource * modelResource){}

	protected:
		virtual void RefreshView(bool force){}

	private:
		Ui::LogWidget *ui;
	};
}

#endif // LOGWIDGET_H
