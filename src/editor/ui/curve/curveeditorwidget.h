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

		void setDocument(CurveDocument* doc) { m_document = doc; onDocumentChanged(doc); }

		Event<float**, float, float> onRequestAudiogram;
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
