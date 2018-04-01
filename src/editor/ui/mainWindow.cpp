#include "mainWindow.h"
#include <string>

#include  <QtWidgets>
#include  <QMenu>
#include  <QStatusBar>

#include  "document.h"

#include  "main_editor.h"
#include  "mainWindow.h"
#include  "editor.h"

#include "proxies/LoggerProxy.h"

#include "ui/curve/curveeditorwidget.h"

#include "scenegraphmodel.h"
#include "ui/treeview/scenegraphviewwidget.h"

#include "ui/propertyeditor/propertyeditorwidget.h"
#include "ui/logwidget.h"

#include  "ui/splashwidget.h"

using namespace Idogep;

Idogep::MainWindow::MainWindow(
	Editor * const & editor
) : ManageOutlineViewRole()
//m_editor(editor)
{
	createActions();
	createStatusBar();
	createDockWindows();

	// connect childrens events
	connectEvents(editor);
}

void Idogep::MainWindow::DocumentChanged(Document * const & document)
{
	// ... 
	m_outlineViewer->setModel(document->GetOutlineModel());
}

void Idogep::MainWindow::closeEvent(QCloseEvent * event)
{
	QSettings settings;
	settings.setValue("mainWindowGeometry", saveGeometry());
	settings.setValue("mainWindowState", saveState());

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

void Idogep::MainWindow::connectEvents(Editor * const & editor)
{
	editor->onDocumentChanged += Delegate(this, &MainWindow::DocumentChanged);

	EditorApplication* app = EditorApplication::Instance();
	app->GetLoggerProxy()->onUpdateLog += Delegate(m_logWidget, &LogWidget::UpdateLog);

	// animation
	m_outlineViewer->onItemSelected += Delegate(this, &ManageOutlineViewRole::ItemSelectedEvent);
	onAnimationSelected += Delegate(m_curveEditor, &CurveEditorWidget::AnimationChangedEvent);
}

// ------

#include <qabstractitemmodel.h>
#include "treeitem.h"
#include "scenegraphmodel.h"

Idogep::ManageOutlineViewRole::ManageOutlineViewRole()
{
}

void Idogep::ManageOutlineViewRole::ItemSelectedEvent(TreeItem *item)
{
	//TreeItem *item = reinterpret_cast<TreeItem*> (index.internalPointer());
	if (!item)
		return;

	HasItemAnimationsRole * animationItem = dynamic_cast<HasItemAnimationsRole*>(item);
	if (animationItem) {
		onAnimationSelected(animationItem->GetAnimation());
	}
}

