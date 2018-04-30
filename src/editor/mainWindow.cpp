#include "mainWindow.h"
#include <string>

#include  <QtWidgets>
#include  <QAction>
#include  <QMenu>
#include  <QStatusBar>

using namespace Idogep;

Idogep::MainWindow::MainWindow()
	: m_redoAct(nullptr), m_undoAct(nullptr)
{
	CreateActions();
	CreateStatusBar();
}

Idogep::MainWindow::~MainWindow()
{
}

void MainWindow::RefreshView(bool force)
{
    // ... 
}

void Idogep::MainWindow::ToggleUndo(const bool enabled)
{
	m_undoAct->setEnabled(enabled);
}

void Idogep::MainWindow::ToggleRedo(const bool enabled)
{
	m_redoAct->setEnabled(enabled);
}

void Idogep::MainWindow::closeEvent(QCloseEvent * event)
{
	QSettings settings;
	settings.setValue("mainWindowGeometry", saveGeometry());
	settings.setValue("mainWindowState", saveState());
}

void Idogep::MainWindow::CreateActions()
{
	// -- File 
    auto fileMenu = menuBar()->addMenu(tr("&File"));

	// -- -- Save
	const auto saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
    auto saveAct = new QAction(saveIcon, tr("&Save..."), this);
	saveAct->setShortcuts(QKeySequence::Save);
	saveAct->setStatusTip(tr("Save the current form letter"));
	connect(saveAct, &QAction::triggered, this, &MainWindow::saveSlot);
	fileMenu->addAction(saveAct);

	// -- --
	fileMenu->addSeparator();

	// -- -- Quit
    auto quitAct = fileMenu->addAction(tr("&Quit"), this, &QWidget::close);
	quitAct->setShortcuts(QKeySequence::Quit);
	quitAct->setStatusTip(tr("Quit the application"));

	// -- Edit 
	// -- -- Undo
    auto editMenu = menuBar()->addMenu(tr("&Edit"));
	const auto undoIcon = QIcon::fromTheme("edit-undo", QIcon(":/images/undo.png"));
	m_undoAct = new QAction(undoIcon, tr("&Undo"), this);
	m_undoAct->setShortcuts(QKeySequence::Undo);
	m_undoAct->setStatusTip(tr("Undo the last editing action"));
	m_undoAct->setEnabled(false);
	connect(m_undoAct, &QAction::triggered, this, &MainWindow::undoSlot);
	editMenu->addAction(m_undoAct);

	// -- -- Redo
	const auto redoIcon = QIcon::fromTheme("edit-redo", QIcon(":/images/redo.png"));
	m_redoAct = new QAction(undoIcon, tr("&Redo"), this);
	m_redoAct->setShortcuts(QKeySequence::Redo);
	m_redoAct->setStatusTip(tr("Redo the last editing action"));
	m_redoAct->setEnabled(false);
	connect(m_undoAct, &QAction::triggered, this, &MainWindow::redoSlot);
	editMenu->addAction(m_redoAct);

	// -- View
	m_viewMenu = menuBar()->addMenu(tr("&View"));
	menuBar()->addSeparator();

	// -- Help
    auto helpMenu = menuBar()->addMenu(tr("&Help"));

    auto aboutQtAct = helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
	aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
}

void Idogep::MainWindow::CreateStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}

//void Idogep::MainWindow::createDockWindows()
//{
//	m_curveEditor = new CurveEditorWidget(this);
//	addDockWidget(Qt::BottomDockWidgetArea, m_curveEditor);
//
//	m_outlineViewer = new SceneGraphViewWidget(this);
//	addDockWidget(Qt::LeftDockWidgetArea, m_outlineViewer);
//
//	m_propertyViewer = new PropertyEditorWidget(this);
//	addDockWidget(Qt::LeftDockWidgetArea, m_propertyViewer);
//	tabifyDockWidget(m_propertyViewer, m_outlineViewer);
//

//}

//void Idogep::MainWindow::ConnectEvents(Editor * const & editor)
//{
//	editor->onDocumentChanged += Delegate(this, &MainWindow::DocumentChanged);
//
//	EditorApplication* app = EditorApplication::Instance();
//	app->GetLoggerProxy()->onUpdateLog += Delegate(m_logWidget, &LogWidget::UpdateLog);
//
//	//m_outlineMediator->
//
//		// Command Stack
//	CommandStack * commandStack = (editor->GetCommandStack());
//	ConnectCommandStackEvents(commandStack);
//	editor->GetCommandStack()->ConnectEmitter(m_curveEditor);
//}

