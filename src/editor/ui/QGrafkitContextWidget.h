#pragma once

/*
QT widget that holds a context for GK Rendering context
*/

#include <QWidget>

#include "render/renderer.h"

class QPaintEngine;

class QResizeEvent;
class QPaintEvent;

namespace Idogep {
	class QGrafkitContextWidget : public QWidget {
		Q_OBJECT
	public:
		QGrafkitContextWidget(Grafkit::Renderer & render, QWidget * parent = nullptr);
		~QGrafkitContextWidget();

		void initialize();

		QPaintEngine* paintEngine();

		void paintEvent(QPaintEvent *event);
		void resizeEvent(QResizeEvent *event);

	private:
		Grafkit::Renderer &m_render;
		bool m_isInited;
	};
}
