#pragma once
#include "CurvePointEditor.h"

#include <QWidget>

namespace Ui {
    class PointEditorWidget;
}

namespace Idogep
{
    class CurvePointEditor;

    class PointEditorWidget : public QWidget, public PointEditorView
    {
        friend class CurvePointEditor;
        Q_OBJECT

    public:
        explicit PointEditorWidget(QWidget *parent = 0);
        ~PointEditorWidget();
        
        void UpdatePointEditor(bool isSet) override;

    protected:
        void RefreshView(bool force) override;

    private slots:
        void keyReturnPressedSlot();
        void valueReturnPressedSlot();
        void radiusReturnPressedSlot();
        void angleReturnPressedSlot();
        void knotTypeChangedSlot(int idex);

    private:
        void SetupPointTypes();
        Ui::PointEditorWidget *ui;
    };
}
