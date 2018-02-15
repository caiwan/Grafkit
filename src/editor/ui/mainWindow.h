#pragma once

#include <QMainWindow>
#include "Event.h"

namespace Idogep {

	class EditorApplication;

	class MainWindow : public QMainWindow {
	public:
		MainWindow(EditorApplication *const& app);

	private:
		void closeEvent(QCloseEvent *event);

		Event<QCloseEvent *> onMainWindowClose;

	private:
		void createActions(EditorApplication *const& app);
		void createStatusBar(EditorApplication *const& app);
		void createDockWindows(EditorApplication *const& app);

		QMenu *viewMenu;
	};

}
