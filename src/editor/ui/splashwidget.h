#pragma once

#include <qwidget.h>
#include <qdialog.h>

namespace Idogep {


	class SplashWidget : public QWidget
	{
		Q_OBJECT
	public:
		SplashWidget(QWidget* parent = NULL);
		virtual ~SplashWidget();

		virtual void paintEvent(QPaintEvent* event);
		virtual void showEvent(QShowEvent* event);
	};
	
	class AboutDialog : public QDialog {
		Q_OBJECT
	public:
		AboutDialog(QWidget* parent = nullptr);
		virtual ~AboutDialog();

	private:
		SplashWidget * m_widget;
	};

}
