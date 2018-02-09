#include <QPaintEvent>
#include <QResizeEvent>

#include "QGrafkitContextWidget.h"

QGrafkitContextWidget::QGrafkitContextWidget(Grafkit::Renderer & render, QWidget *parent) : QWidget(parent), m_render(render), m_isInited(false)
{
}

QGrafkitContextWidget::~QGrafkitContextWidget()
{
}

void QGrafkitContextWidget::initialize()
{
	if (!m_isInited) {
		HWND hWnd = reinterpret_cast<HWND>(winId());
		m_render.Initialize(width(), height(), false, hWnd, false);
	}
	m_isInited = true;
}

QPaintEngine * QGrafkitContextWidget::paintEngine()
{
	return nullptr;
}

void QGrafkitContextWidget::paintEvent(QPaintEvent * event)
{
	// ... 
}

void QGrafkitContextWidget::resizeEvent(QResizeEvent * event)
{
	if (m_isInited) {
		// ... 
	}
}
