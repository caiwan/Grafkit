#include <QPaintEvent>
#include <QResizeEvent>

#include <qdebug>
#include "QGrafkitContextWidget.h"


using namespace Idogep;

QGrafkitContextWidget::QGrafkitContextWidget(Grafkit::Renderer & render, QWidget *parent) : QWidget(parent), m_render(render), m_isInited(false)
{
	setMinimumSize(400, 300);
}

QGrafkitContextWidget::~QGrafkitContextWidget()
{
}

void QGrafkitContextWidget::Initialize()
{
	if (!m_isInited) {
		HWND hWnd = reinterpret_cast<HWND>(winId());
		QSize wSize = size();
		qDebug() << "Create rendering surface " << wSize;
		m_render.Initialize(wSize.width(), wSize.height(), false, hWnd, false);
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
		QSize size = event->size();
		qDebug() << "Resize rendering surface " << size;
		m_render.Resize(size.width(), size.height());
		onResizeSurface(m_render);
	}
}
