#pragma once

#include <QtGui>
#include <QDockWidget>

#include "AnimationModule.h"

namespace Ui
{
    class AnimationEditorWidget;
}

class QTreeView;
class QGraphicsScene;

namespace Idogep
{
    class CurveCursor;

    class AnimationEditor;

    // ========================================================================================================

    class AnimationEditorWidget : public QDockWidget, public AnimationEditorView
    {
        friend class AnimationEditor;
        Q_OBJECT
    public:
        explicit AnimationEditorWidget(QWidget* parent = nullptr);
        ~AnimationEditorWidget();

        /* METHODS */
        void SetGraphicsScene(QGraphicsScene* graphicsScene) const;
        void SetPointEditorWidget(QWidget * widget);

    protected:
        void RefreshView(bool force) override;
        void UpdateAnimationModel(TreeModel* model) override;

        void resizeEvent(QResizeEvent* event) override;

        // QT specific
        // ReSharper disable CppInconsistentNaming
    protected slots:
        void playPressedSlot() { onTogglePlayback(); }
        void stopPressedSlot() { onStopPlayback(); }

        void itemClickedSlot(const QModelIndex& index);
        void itemDoubleClickedSlot(const QModelIndex& index);

    private:
        Ui::AnimationEditorWidget* ui;
        // ReSharper restore CppInconsistentNaming

    };
}
