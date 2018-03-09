#include "mainWindow.h"
#include <string>

#include  <QtWidgets>
#include  <QMenu>
#include  <QStatusBar>

#include  "document.h"

#include  "mainWindow.h"
#include  "editor.h"

#include  "ui/curve/curveeditorwidget.h"
#include  "ui/PlaybackOptionDialog.h"
#include  "ui/splashwidget.h"

using namespace Idogep;

Idogep::MainWindow::MainWindow(Editor * const & editor) : m_editor(editor)
{
	createActions();
	createStatusBar();
	createDockWindows();

	// events
}

void Idogep::MainWindow::closeEvent(QCloseEvent * event)
{
	QSettings settings;
	settings.setValue("mainWindowGeometry", saveGeometry());
	settings.setValue("mainWindowState", saveState());

}

// ez nem lesz
void Idogep::MainWindow::playbackOptions()
{
	PlaybackOptionDialog dlg(this);
	if (dlg.exec())
	{
		std::string filename = dlg.fileName().toStdString();
		//m_document->Op
		//onPlaybackOptionsChanged();
	}
}

void Idogep::MainWindow::createActions()
{

	QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
	//QToolBar *fileToolBar = addToolBar(tr("File"));

	const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
	QAction *saveAct = new QAction(saveIcon, tr("&Save..."), this);
	saveAct->setShortcuts(QKeySequence::Save);
	saveAct->setStatusTip(tr("Save the current form letter"));
	//connect(saveAct, &QAction::triggered, this, &MainWindow::save);
	fileMenu->addAction(saveAct);
	//fileToolBar->addAction(saveAct);

	fileMenu->addSeparator();

	QAction *quitAct = fileMenu->addAction(tr("&Quit"), this, &QWidget::close);
	quitAct->setShortcuts(QKeySequence::Quit);
	quitAct->setStatusTip(tr("Quit the application"));

	QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
	//QToolBar *editToolBar = addToolBar(tr("Edit"));
	const QIcon undoIcon = QIcon::fromTheme("edit-undo", QIcon(":/images/undo.png"));
	QAction *undoAct = new QAction(undoIcon, tr("&Undo"), this);
	undoAct->setShortcuts(QKeySequence::Undo);
	undoAct->setStatusTip(tr("Undo the last editing action"));

	//connect(undoAct, &QAction::triggered, this, &MainWindow::undo);

	editMenu->addAction(undoAct);
	//editToolBar->addAction(undoAct);

	m_viewMenu = menuBar()->addMenu(tr("&View"));

	menuBar()->addSeparator();

	QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

	m_aboutDlg = new AboutDialog();

	//QAction *aboutAct = helpMenu->addAction(tr("&About"), m_aboutDlg, &QDialog::show);
	//aboutAct->setStatusTip(tr("Show the application's About box"));

	QAction *aboutQtAct = helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
	aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
}

void Idogep::MainWindow::createStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}

void Idogep::MainWindow::createDockWindows()
{
	m_curveEditor = new CurveEditorWidget();
	addDockWidget(Qt::BottomDockWidgetArea, m_curveEditor);

	//m_playbackOptionsDlg = new PlaybackOptionDialog(this);
}
