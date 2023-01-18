#pragma once

#include <QtGui>
#include <QDockWidget>

#include "AnimationView.h"

namespace Ui
{
    class AnimationEditorWidget;
}

class QTreeView;
class QGraphicsScene;

namespace Idogep
{
    class CurveCursor;

    // ========================================================================================================

    class AnimationEditorWidget : public QDockWidget, public AnimationEditorView, public PointEditorView
    {
        Q_OBJECT
    public:
        explicit AnimationEditorWidget(QWidget* parent = nullptr);
        ~AnimationEditorWidget();

        /* METHODS */
        void SetGraphicsScene(QGraphicsScene* graphicsScene) const;

    protected:

        void RefreshView(bool force) override;
        void UpdateAnimationModel(TreeModel* model) override;
        void UpdatePointEditor(bool isSet) override;

    protected:
        void resizeEvent(QResizeEvent* event) override;
        // ReSharper restore CppInconsistentNaming
        void SetupPointTypes();

        // QT specific
        // ReSharper disable CppInconsistentNaming
    protected slots:
        void playPressedSlot() { onTogglePlayback(); }
        void stopPressedSlot() { onStopPlayback(); }

        void keyReturnPressedSlot();
        void valueReturnPressedSlot();
        void knotTypeChangedSlot(int index);

        void itemClickedSlot(const QModelIndex& index);
        void itemDoubleClickedSlot(const QModelIndex& index);

    private:
        Ui::AnimationEditorWidget* ui;

    };
}
