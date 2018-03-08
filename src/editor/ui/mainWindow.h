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
		MainWindow();

		// external events (called from outside)
		//void setDocument(EditorDocument* document);

		// internal events (called from inside)
		//Event<QCloseEvent *> onMainWindowClose;

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

		Editor *m_editor;
	};

}
