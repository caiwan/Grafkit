#pragma once

#include <functional>

#include <QMainWindow>

#include "common.h"

#include "models/ViewModule.h"
#include "utils/Event.h"

#include "mediators/EditingMediator.h"
#include "mediators/OutlineViewMediator.h"

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

	class CommandStack;

	class View;

	// ---

	class MainWindow : public QMainWindow, public View,
		public Roles::ManageCommandStackRole
	{
	public:
		MainWindow(Editor * const & editor);

		void DocumentChanged(Document * const & document);
		virtual void SetModel(Ref<Referencable> model){}

	protected:
		virtual void RefreshView(bool force){ /**/ }

		// internal events (called from inside)		
		Event<> onSave;

	protected:
		virtual void ToggleUndo(bool enabled);
		virtual void ToggleRedo(bool enabled);

	private:
		void closeEvent(QCloseEvent *event);

		private slots:
		void undoSlot() { onUndo(); }
		void redoSlot() { onRedo(); }
		void saveSlot() { onSave(); }

	private:
		void createActions();
		void createStatusBar();
		void createDockWindows();

		void ConnectEvents(Editor * const & editor);

		QMenu *m_viewMenu;

		CurveEditorWidget * m_curveEditor;
		SceneGraphViewWidget *m_outlineViewer;
		PropertyEditorWidget *m_propertyVewer;

		Roles::MediateOutlineViewRole m_outlineMediator;

		LogWidget* m_logWidget;
	};

}
