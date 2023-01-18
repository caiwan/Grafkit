#include <qapplication.h>
#include <qdesktopwidget.h>
#include <qstyleoption.h>
#include <qpainter.h>

#include "splashwidget.h"

#define SCALE .75
#define IWIDTH 750
#define IHEIGHT 537

#define IMAGEURL ":/splash.jpg"

#define WIDTH ( (IWIDTH) * (SCALE))
#define HEIGHT ( (IHEIGHT) * (SCALE))


Idogep::SplashWidget::SplashWidget(QWidget* parent) : QWidget(parent)
{
	int screenID = QApplication::desktop()->primaryScreen();
	QRect screenGeometry = QApplication::desktop()->availableGeometry(screenID);

    setGeometry(
        (screenGeometry.width() - WIDTH ) / 2,
        (screenGeometry.height() - HEIGHT ) / 2,
        WIDTH,
        HEIGHT
    );
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
}

Idogep::SplashWidget::~SplashWidget()
{
}

void Idogep::SplashWidget::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);

    QStyleOption styleOption;
    styleOption.init(this);
    
	QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &styleOption, &painter, this);
	painter.drawPixmap(0, 0, QPixmap(IMAGEURL).scaled(size()));
}

void Idogep::SplashWidget::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
}

Idogep::AboutDialog::AboutDialog(QWidget * parent) : QDialog(parent)
{
	m_widget = new SplashWidget(this);
	connect(m_widget, SIGNAL(clicked()), this, SLOT(accept()));
}

Idogep::AboutDialog::~AboutDialog()
{
}
