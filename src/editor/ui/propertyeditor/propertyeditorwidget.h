#ifndef PropertyEditorWidget_H
#define PropertyEditorWidget_H

#include <QDockWidget>

namespace Ui {
class PropertyEditorWidget;
}

namespace Idogep {

class PropertyEditorWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit PropertyEditorWidget(QWidget *parent = 0);
    ~PropertyEditorWidget();

private:
    Ui::PropertyEditorWidget *ui;
};
}

#endif // PropertyEditorWidget_H
