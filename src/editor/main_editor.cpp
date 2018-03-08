#include <qapplication.h>

#include <qtimer.h>

#include <qmessagebox.h>
#include <qevent.h>

#include "utils/AssetFile.h"

#include "main_editor.h"

#include "editor.h"

#include "ui/splashwidget.h"
#include "ui/mainWindow.h"

using namespace Idogep;
using namespace Grafkit;

Idogep::EditorApplication * Idogep::EditorApplication::s_self;

Idogep::EditorApplication::EditorApplication(int argc, char **argv) :
	QObject(nullptr),
	Grafkit::ClonableInitializer(),
	Grafkit::IResourceManager(),
	m_qApp(argc, argv), m_editor(nullptr)
{
	assert(s_self == nullptr);

	QCoreApplication::setOrganizationName("IndustrialRevolutioners");
	QCoreApplication::setOrganizationDomain("caiwan.github.io");
	QCoreApplication::setApplicationName(APP_NAME);

	QFile f(":/res/css/global.css"); f.open(QFile::ReadOnly);
	m_qApp.setStyleSheet(f.readAll()); f.close();

	m_file_loader = new FileAssetFactory("./");

	s_self = this;
}

int Idogep::EditorApplication::execute()
{
	m_mainWindow = new MainWindow(this);

	// setup events
	//m_wnd->onMainWindowClose += Delegate(this, &EditorApplication::onMainWindowClose);

	// --- 

	SplashWidget *sw = new SplashWidget();

	LoaderThread *loader = new LoaderThread();

	onLoaderFinished += Delegate(sw, &SplashWidget::hide);
	onLoaderFinished += Delegate(sw, &SplashWidget::deleteLater);
	onLoaderFinished += Delegate(m_mainWindow, &MainWindow::showMaximized);
	onLoaderFinished += Delegate(this, &EditorApplication::nextTick);
	onLoaderFinished += Delegate(loader, &LoaderThread::deleteLater);

	sw->show();

	connect(loader, SIGNAL(finished()), this, SLOT(loaderFinished()));
	loader->start();

	m_editor = new Editor(m_render, this, m_mainWindow, m_renderWidget);

	// Argparse??
	m_editor->NewDocument();

	m_renderWidget = new QGrafkitContextWidget(m_render);
	m_renderWidget->initialize();
	
	// init stuff here

	//m_wnd->setCentralWidget(m_widget);

	//onNew();

	return m_qApp.exec();
}

void Idogep::EditorApplication::onMainWindowClose(QCloseEvent * event)
{
	bool changes = true;
	QMessageBox::StandardButton resBtn = QMessageBox::question(m_mainWindow, APP_NAME,
		tr("Are you sure?\n"),
		QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
		QMessageBox::Yes);

	if (resBtn != QMessageBox::Yes) {
		event->ignore();
	}
	else {
		event->accept();
	}
}

void Idogep::EditorApplication::onNew()
{
	if (m_document && m_document->dirty()) {
		// ablak goez here 
		// + save
		// + return ha nem 
		onBeforePreload -= Delegate(m_document, &EditorDocument::beforePreload);
		onAfterPreload -= Delegate(m_document, &EditorDocument::afterPreload);
		delete m_document;
		m_document = nullptr;
	}

	m_document = new EditorDocument();

	onBeforePreload += Delegate(m_document, &EditorDocument::beforePreload);
	onAfterPreload += Delegate(m_document, &EditorDocument::afterPreload);

	m_wnd->setDocument(m_document);

	preload();
}

void Idogep::EditorApplication::mainloop()
{
	this->m_editor->RenderFrame();
	this->nextTick();
}

void Idogep::EditorApplication::loaderFinished()
{
	onLoaderFinished();
}

void Idogep::EditorApplication::nextTick()
{
	QTimer::singleShot(0, this, SLOT(mainloop()));
}

void Idogep::EditorApplication::preload()
{
	onBeforePreload(this);
	DoPrecalc();
	onAfterPreload(m_render);
}


// ========================================================================================

Idogep::LoaderThread::LoaderThread(QObject* parent) : QThread(parent)
{
}

void Idogep::LoaderThread::run()
{
	msleep(1000);
}


// ========================================================================================

int main(int argc, char **argv)
{
	Q_INIT_RESOURCE(resources);

	auto app = new Idogep::EditorApplication(argc, argv);
	return app->execute();
}
