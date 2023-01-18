#pragma once
#include <QMainWindow>

#include "common.h"
#include "EditorView.h"

#include "utils/ViewModule.h"
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

	protected:
		void RefreshView(bool force) override;

	    void ToggleUndo(bool enabled) override;
	    void ToggleRedo(bool enabled) override;

        // events
		void closeEvent(QCloseEvent *event) override;

		private slots:
	    // ReSharper disable CppInconsistentNaming
		void undoSlot() { onUndo(); }
		void redoSlot() { onRedo(); }
        void saveSlot() { onSave(); }
        void loadSlot() { onLoad(); }
	    // ReSharper restore CppInconsistentNaming


	public:
	    bool OpenFolder(std::string& outFilename) override;

	private:
		void CreateActions();
		void CreateStatusBar();

		QMenu *m_viewMenu;

		QAction * m_undoAct;
		QAction * m_redoAct;
	};

}
