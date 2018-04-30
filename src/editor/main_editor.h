#pragma once

#include <qapplication.h>
#include <qthread.h>

#include "Application.h"

class QCloseEvent;

namespace Idogep {

	class EditorApplicationQt : public QObject, public EditorApplication
	{
		Q_OBJECT

	public:
		EditorApplicationQt(int argc, char **argv);
		~EditorApplicationQt();

		void StartLoaderThread() override;

		// --- 
		private slots:
	    // ReSharper disable CppInconsistentNaming
		void loaderFinishedSlot() { onLoaderFinished(); }
		void focusChangedSlot(QWidget*from, QWidget* to) { onFocusChanged(from, to); }
		void mainloopSlot() { Mainloop(); }
	    // ReSharper restore CppInconsistentNaming

	private:
	    int ExecuteParentFramework() override;
	    void NextTick() override;

		QApplication m_qApp;

	};


}

