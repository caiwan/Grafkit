#pragma once

#include <QMainWindow>
#include "Event.h"

namespace Idogep {

	class EditorDocument;

	class EditorApplication;
	class CurveEditorWidget;

	class MainWindow : public QMainWindow {
	public:
		MainWindow(EditorApplication *const& app);

		void setDocument(EditorDocument* document);
		Event<EditorDocument*> onDocumentChanged;

	private:
		void closeEvent(QCloseEvent *event);

		Event<QCloseEvent *> onMainWindowClose;

	private:
		void createActions(EditorApplication *const& app);
		void createStatusBar(EditorApplication *const& app);
		void createDockWindows(EditorApplication *const& app);

		QMenu *viewMenu;

		CurveEditorWidget * m_curveEditor;
	};

}
