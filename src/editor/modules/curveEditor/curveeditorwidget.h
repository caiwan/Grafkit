#pragma once

#include <QtGui>
#include <qdockwidget.h>

#include "animation/animation.h"

#include "CurveView.h"

#include "curvedoc.h"

namespace Ui
{
    class CurveEditorWidget;
}

class QTreeView;
class QGraphicsScene;

namespace Idogep
{
    class CurveCursor;

    // ========================================================================================================

    class CurveEditorWidget : public QDockWidget, public CurveEditorView
    {
    Q_OBJECT

    public:
        explicit CurveEditorWidget(QWidget* parent = nullptr);
        ~CurveEditorWidget();

        /* METHODS */
        void SetGraphicsScene(QGraphicsScene* graphicsScene);

    protected:

        void RefreshView(bool force) override;

   
		void UpdateAnimationModel(TreeModel* model) override;

        // QT specific
        // ReSharper disable CppInconsistentNaming
    protected slots:
        void playPressedSlot() { onTogglePlayback(); }
        void stopPressedSlot() { onStopPlayback(); }

		void itemClickedSlot(const QModelIndex& index);
        void itemDoubleClickedSlot(const QModelIndex& index);
        
    protected:
        void resizeEvent(QResizeEvent* event) override;
        // ReSharper restore CppInconsistentNaming

    private:
        Ui::CurveEditorWidget* ui;
  
	};
}
