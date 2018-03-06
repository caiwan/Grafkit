#pragma once

#include <QMainWindow>
#include "Event.h"

namespace Idogep {

	class EditorDocument;

	class EditorApplication;
	
	class CurveEditorWidget;

	class PlaybackOptionDialog;
	class AboutDialog;

	// ---

	class MainWindow : public QMainWindow {
	public:
		MainWindow(EditorApplication *const& app);

		// external events (called from outside)
		void setDocument(EditorDocument* document);

		// internal events (called from inside)
		Event<QCloseEvent *> onMainWindowClose;

	private:
		void closeEvent(QCloseEvent *event);
		void playbackOptions();

	private:
		void createActions(EditorApplication *const& app);
		void createStatusBar(EditorApplication *const& app);
		void createDockWindows(EditorApplication *const& app);

		QMenu *m_viewMenu;

		CurveEditorWidget * m_curveEditor;
		AboutDialog *m_aboutDlg;

		//MainWindowEventWrapper *m_myEventWrapper;
		EditorDocument* m_document;
	};

}
