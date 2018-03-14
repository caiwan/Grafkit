#pragma once

#include <QMainWindow>
#include "Event.h"

namespace Idogep {

	class Editor;
	class Document;

	class EditorApplication;

	class QGrafkitContextWidget;

	class CurveEditorWidget;
	class SceneGraphViewWidget;

	class PlaybackOptionDialog;
	//class AboutDialog;

	// ---

	class MainWindow : public QMainWindow {
	public:
		MainWindow(
			Editor * const & editor
		);

		// external events (called from outside)
		void documentChanged(Document * const & document);

		// internal events (called from inside)		
		// ...

	private:
		void closeEvent(QCloseEvent *event);
		void playbackOptions();

	private:
		void createActions();
		void createStatusBar();
		void createDockWindows();

		void connectEvents(Editor * const & editor);

		QMenu *m_viewMenu;

		CurveEditorWidget * m_curveEditor;
		SceneGraphViewWidget *m_sceneGraphViewer;

		//AboutDialog *m_aboutDlg;

		//Document * m_document;
		//Editor * const & m_editor;
	};

}
