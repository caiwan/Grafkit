#pragma once

#include <qwidget.h>

namespace Idogep {

	class SplashWidget : public QWidget
	{
		Q_OBJECT
	public:
		SplashWidget(QWidget* parent = NULL);
		~SplashWidget();

		virtual void paintEvent(QPaintEvent* event);
		virtual void showEvent(QShowEvent* event);
	};

}
