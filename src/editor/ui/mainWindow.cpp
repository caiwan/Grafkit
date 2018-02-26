
#include <QtWidgets>
#include <QMenu>
#include <QStatusBar>

#include "document.h"

#include "mainWindow.h"
#include "main_editor.h"

#include "ui/curve/curveeditorwidget.h"
#include "ui/splashwidget.h"

using namespace Idogep;

Idogep::MainWindow::MainWindow(EditorApplication *const& app)
{
	createActions(app);
	createStatusBar(app);
	createDockWindows(app);

	onMainWindowClose += Delegate(app, &EditorApplication::onMainWindowClose);
	onDocumentChanged += Delegate(this, &MainWindow::setDocument);
}

void Idogep::MainWindow::setDocument(EditorDocument * document)
{
	// ez nem terljesen jo, disztributalni kell az esemenyeket
	m_curveEditor->onDocumentChanged(document->curveDocument());
}

void Idogep::MainWindow::closeEvent(QCloseEvent * event)
{
	QSettings settings;
	settings.setValue("mainWindowGeometry", saveGeometry());
	settings.setValue("mainWindowState", saveState());

	onMainWindowClose(event);
}

void Idogep::MainWindow::createActions(EditorApplication *const& app)
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

	viewMenu = menuBar()->addMenu(tr("&View"));

	menuBar()->addSeparator();

	QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

	m_aboutDlg = new AboutDialog();

	//QAction *aboutAct = helpMenu->addAction(tr("&About"), m_aboutDlg, &QDialog::show);
	//aboutAct->setStatusTip(tr("Show the application's About box"));

	QAction *aboutQtAct = helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
	aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
}

void Idogep::MainWindow::createStatusBar(EditorApplication *const& app)
{
	statusBar()->showMessage(tr("Ready"));
}

void Idogep::MainWindow::createDockWindows(EditorApplication *const& app)
{
	m_curveEditor = new CurveEditorWidget();
	addDockWidget(Qt::BottomDockWidgetArea, m_curveEditor);
}
