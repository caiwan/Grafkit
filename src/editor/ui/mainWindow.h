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
	class LogWidget;

	class PropertyEditorWidget;
	//class AboutDialog;

	// ---

	class MainWindow : public QMainWindow {
	public:
		MainWindow(
			Editor * const & editor
		);

		// external events (called from outside)
		void DocumentChanged(Document * const & document);

		// internal events (called from inside)		
		// ...

	private:
		void closeEvent(QCloseEvent *event);
		//void playbackOptions();

	private:
		void createActions();
		void createStatusBar();
		void createDockWindows();

		void connectEvents(Editor * const & editor);

		QMenu *m_viewMenu;

		CurveEditorWidget * m_curveEditor;
		SceneGraphViewWidget *m_outlineViewer;
		PropertyEditorWidget *m_propertyVewer;

		LogWidget* m_logWidget;
	};

}
