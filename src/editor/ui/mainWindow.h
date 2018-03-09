#pragma once

#include <QMainWindow>
#include "Event.h"

namespace Idogep {

	class Editor;
	class Document;

	class EditorApplication;

	class QGrafkitContextWidget;
	
	class CurveEditorWidget;

	class PlaybackOptionDialog;
	class AboutDialog;

	// ---

	class MainWindow : public QMainWindow {
	public:
		MainWindow(Editor * const & editor);

		// external events (called from outside)
		// ... 

		// internal events (called from inside)		
		// ...

	private:
		void closeEvent(QCloseEvent *event);
		void playbackOptions();

	private:
		void createActions();
		void createStatusBar();
		void createDockWindows();

		Document * m_document;

		QMenu *m_viewMenu;

		CurveEditorWidget * m_curveEditor;
		AboutDialog *m_aboutDlg;

		Editor * const & m_editor;
	};

}
