#include "mainWindow.h"
#include <string>

#include  <QtWidgets>
#include  <QAction>
#include  <QMenu>
#include  <QStatusBar>

using namespace Idogep;

MainWindow::MainWindow() : QMainWindow(nullptr)
, EditorView()
, m_undoAct(nullptr)
, m_redoAct(nullptr)
{
    CreateActions();
    CreateStatusBar();
    setFocusPolicy(Qt::ClickFocus);
}

MainWindow::~MainWindow() {
}

void MainWindow::RefreshView(bool force) { // ... 
}

void MainWindow::ToggleUndo(const bool enabled) { m_undoAct->setEnabled(enabled); }

void MainWindow::ToggleRedo(const bool enabled) { m_redoAct->setEnabled(enabled); }

void MainWindow::closeEvent(QCloseEvent* event)
{
    Q_UNUSED(event);
    QSettings settings;
    settings.setValue("mainWindowGeometry", saveGeometry());
    settings.setValue("mainWindowState", saveState());
}

bool MainWindow::OpenFolder(std::string& outFilename)
{
    outFilename.clear();

    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "./", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (dir.isEmpty())
        return false;

    outFilename = dir.toStdString();

    return true;
}

void MainWindow::CreateActions()
{
    // -- File 
    auto fileMenu = menuBar()->addMenu(tr("&File"));

    // -- -- Load
    const auto loadIcon = QIcon::fromTheme("document-load", QIcon(":/images/load.png"));
    auto loadAct = new QAction(loadIcon, tr("&Load..."), this);
    loadAct->setShortcuts(QKeySequence::Open);
    loadAct->setStatusTip(tr("Load"));
    connect(loadAct, &QAction::triggered, this, &MainWindow::loadSlot);
    fileMenu->addAction(loadAct);

    // -- -- Save
    const auto saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
    auto saveAct = new QAction(saveIcon, tr("&Save..."), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::saveSlot);
    fileMenu->addAction(saveAct);

    // -- --
    fileMenu->addSeparator();

    // -- -- Quit
    const auto quitIcon = QIcon::fromTheme("quit", QIcon(":/quit.png"));
    auto quitAct = fileMenu->addAction(quitIcon, tr("&Quit"), this, &QWidget::close);
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
    connect(m_redoAct, &QAction::triggered, this, &MainWindow::redoSlot);
    editMenu->addAction(m_redoAct);

    // -- View
    m_viewMenu = menuBar()->addMenu(tr("&View"));
    menuBar()->addSeparator();

    // -- Help
    auto helpMenu = menuBar()->addMenu(tr("&Help"));

    auto aboutQtAct = helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
}

void MainWindow::CreateStatusBar() { statusBar()->showMessage(tr("Ready")); }

