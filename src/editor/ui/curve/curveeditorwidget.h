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
	class CurveDocument;

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

		void setDocument(CurveDocument* doc) { m_document = doc; onDocumentChanged(doc); }

		Event<QImage**, float, float, int, int> onRequestAudiogram;
		Event<CurveDocument*> onDocumentChanged;

	protected:
		virtual void resizeEvent(QResizeEvent* event);
		Event<QResizeEvent*> onResize;

	private:
		Ui::CurveEditorWidget *ui;
		CurveEditorScene* m_ces;

		CurveDocument *m_document;
		
	};

}
