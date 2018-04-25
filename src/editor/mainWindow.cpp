#include "mainWindow.h"
#include <string>

#include  <QtWidgets>
#include  <QMenu>
#include  <QStatusBar>

#include  "models/document.h"

#include  "main_editor.h"
#include  "mainWindow.h"
#include  "editor.h"

#include "utils/Command.h"

#include "proxies/LoggerProxy.h"

#include "modules/curveEditor/curveeditorwidget.h"

#include "modules/outlineView/scenegraphviewwidget.h"

#include "modules/propertyEditor/propertyeditorwidget.h"
#include "modules/logView/logwidget.h"


using namespace Idogep;

Idogep::MainWindow::MainWindow(
	Editor * const & editor
) : Roles::ManageCommandStackRole()
//m_editor(editor)
{
	createActions();
	createStatusBar();
	createDockWindows();

	// connect childrens events
	ConnectEvents(editor);
}

void Idogep::MainWindow::DocumentChanged(Document * const & document)
{
	m_outlineViewer->SetModel(document->GetScenegraph());
}

void Idogep::MainWindow::ToggleUndo(bool enabled)
{
	m_undoAct->setEnabled(enabled);
}

void Idogep::MainWindow::ToggleRedo(bool enabled)
{
	m_redoAct->setEnabled(enabled);
}

void Idogep::MainWindow::closeEvent(QCloseEvent * event)
{
	QSettings settings;
	settings.setValue("mainWindowGeometry", saveGeometry());
	settings.setValue("mainWindowState", saveState());

}

void Idogep::MainWindow::createActions()
{
	// -- File 
	QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

	// -- -- Save
	const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
	QAction *saveAct = new QAction(saveIcon, tr("&Save..."), this);
	saveAct->setShortcuts(QKeySequence::Save);
	saveAct->setStatusTip(tr("Save the current form letter"));
	connect(saveAct, &QAction::triggered, this, &MainWindow::saveSlot);
	fileMenu->addAction(saveAct);

	// -- --
	fileMenu->addSeparator();

	// -- -- Quit
	QAction *quitAct = fileMenu->addAction(tr("&Quit"), this, &QWidget::close);
	quitAct->setShortcuts(QKeySequence::Quit);
	quitAct->setStatusTip(tr("Quit the application"));

	// -- Edit 
	// -- -- Undo
	QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
	const QIcon undoIcon = QIcon::fromTheme("edit-undo", QIcon(":/images/undo.png"));
	m_undoAct = new QAction(undoIcon, tr("&Undo"), this);
	m_undoAct->setShortcuts(QKeySequence::Undo);
	m_undoAct->setStatusTip(tr("Undo the last editing action"));
	m_undoAct->setEnabled(false);
	connect(m_undoAct, &QAction::triggered, this, &MainWindow::undoSlot);
	editMenu->addAction(m_undoAct);

	// -- -- Redo
	const QIcon redoIcon = QIcon::fromTheme("edit-redo", QIcon(":/images/redo.png"));
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
	QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

	QAction *aboutQtAct = helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
	aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
}

void Idogep::MainWindow::createStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}

void Idogep::MainWindow::createDockWindows()
{
	m_curveEditor = new CurveEditorWidget(this);
	addDockWidget(Qt::BottomDockWidgetArea, m_curveEditor);

	m_outlineViewer = new SceneGraphViewWidget(this);
	addDockWidget(Qt::LeftDockWidgetArea, m_outlineViewer);

	m_propertyVewer = new PropertyEditorWidget(this);
	addDockWidget(Qt::LeftDockWidgetArea, m_propertyVewer);
	tabifyDockWidget(m_propertyVewer, m_outlineViewer);

	m_logWidget = new LogWidget(this);
	addDockWidget(Qt::BottomDockWidgetArea, m_logWidget);
	tabifyDockWidget(m_logWidget, m_curveEditor);
}

void Idogep::MainWindow::ConnectEvents(Editor * const & editor)
{
	editor->onDocumentChanged += Delegate(this, &MainWindow::DocumentChanged);

	EditorApplication* app = EditorApplication::Instance();
	app->GetLoggerProxy()->onUpdateLog += Delegate(m_logWidget, &LogWidget::UpdateLog);

	//m_outlineMediator->

		// Command Stack
	CommandStack * commandStack = (editor->GetCommandStack());
	ConnectCommandStackEvents(commandStack);
	editor->GetCommandStack()->ConnectEmitter(m_curveEditor);
}

