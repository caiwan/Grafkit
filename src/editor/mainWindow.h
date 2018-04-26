#pragma once

#include <functional>

#include <QMainWindow>

#include "common.h"

#include "ViewModule.h"
#include "Editor.h"
#include "utils/Event.h"

class QMenu;
class QAction;

namespace Idogep {

	class Document;
	class View;

	// ---

	class MainWindow : public QMainWindow, public EditorView
	{
	public:
		MainWindow();
		~MainWindow();

		void Initialize();

		virtual void SetModel(Grafkit::IResource * modelResource){}

	protected:
		virtual void RefreshView(bool force) { /**/ }

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
		//void createDockWindows();

		QMenu *m_viewMenu;

	private:
		QAction * m_undoAct;
		QAction * m_redoAct;
	};

}
