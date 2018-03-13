#include <qapplication.h>
#include <QKeyEvent>

#include "preloader.h"
#include "ui_preloader.h"

using namespace Idogep;
using namespace Grafkit;

Idogep::Preloader::Preloader(QWidget *parent) : QDialog(parent), ui(new Ui::Preloader())
{
	ui->setupUi(this);
}


Idogep::Preloader::~Preloader()
{
}

void Idogep::Preloader::OnBeginLoad()
{
	show();
	qApp->processEvents();

}

void Idogep::Preloader::OnElemLoad(size_t actual, size_t count)
{
	if (count > 0)
		ui->progressBar->setValue((100 * actual) / (100 * count));
	else
		ui->progressBar->setValue(100);
	qApp->processEvents();
}

void Idogep::Preloader::OnEndLoad()
{
	hide();
	qApp->processEvents();
}

void Idogep::Preloader::FocusChanged(QWidget * const & from, QWidget * const & to)
{
	if (isHidden())
		return;

	if (!isAncestorOf(to))
	{
		QWidget* widget(qobject_cast<QWidget*>(children().front()));
		widget->setFocus(Qt::OtherFocusReason);
		QKeyEvent event(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier);
		qApp->sendEvent(widget, &event);
	}
}
