#pragma once

#include <qapplication.h>
#include <qthread.h>

#include "render/renderer.h"
#include "utils/ResourceManager.h"
#include "utils/ResourcePreloader.h"
#include "utils/InitializeSerializer.h"

#include "utils/Event.h"

class QCloseEvent;

namespace Idogep {

	class Document;
	class Editor;

	class Preloader;

	class QGrafkitContextWidget;

	class LoggerQTProxy;

	class EditorApplication : public QObject,
		protected Grafkit::ResourcePreloader, private Grafkit::ClonableInitializer
	{
		Q_OBJECT

	public:
		EditorApplicationQt(int argc, char **argv);
		~EditorApplicationQt();

		void StartLoaderThread() override;

		// --- 
		private slots:
	    // ReSharper disable CppInconsistentNaming
		void loaderFinishedSlot() { onLoaderFinished(); }
		void focusChangedSlot(QWidget*from, QWidget* to) { onFocusChanged(from, to); }
		void mainloopSlot() { Mainloop(); }
	    // ReSharper restore CppInconsistentNaming

	private:
	    int ExecuteParentFramework() override;
	    void NextTick() override;

		void BuildEditorModules();

	public:
		Event<> onLoaderFinished;
		Event<QWidget* const&, QWidget* const&> onFocusChanged;

	private:
		QApplication m_qApp;
		Grafkit::Renderer m_render;
		Grafkit::IAssetFactory *m_assetFactory;
		Grafkit::IAssetFactory *m_projectFileLoader;

		Idogep::LoggerQTProxy *m_logger;
		Idogep::QGrafkitContextWidget *m_renderWidget;

		Idogep::MainWindow *m_mainWindow;

	};


}

