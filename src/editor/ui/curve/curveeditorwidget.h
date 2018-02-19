#pragma once 

#include <QtGui>
#include <qdockwidget.h>

#include "Event.h"

namespace Ui {
	class CurveEditorWidget;
}

namespace Idogep {

	class CurveEditorScene;
	class CurvePointItem;

	class CurveEditorWidget : public QDockWidget
	{
		Q_OBJECT

	public:
		explicit CurveEditorWidget(QWidget *parent = 0);
		~CurveEditorWidget();

		// onAddPoint
		// onModifyPoint
		// onDeletePoint
		// onCut
		// onPaste
		
		Event<float, float, int, int, QImage*> onRequestAudiogram;

	protected:
		virtual void resizeEvent(QResizeEvent* event);
		Event<QResizeEvent*> onResize;

	private:
		Ui::CurveEditorWidget *ui;
		CurveEditorScene* m_ces;
	};

}
