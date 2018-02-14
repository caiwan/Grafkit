#pragma once

#include <QMainWindow>

namespace Idogep {

	class EditorApplication;

	class MainWindow : public QMainWindow {
	public:
		MainWindow(EditorApplication *const& app);

	private:
		void createActions(EditorApplication *const& app);
		void createStatusBar(EditorApplication *const& app);
		void createDockWindows(EditorApplication *const& app);

		QMenu *viewMenu;
	};

}
