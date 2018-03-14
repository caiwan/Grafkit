#ifndef PROPERTYEDITOR_H
#define PROPERTYEDITOR_H

#include <QDockWidget>

namespace Ui {
class PropertyEditor;
}

class PropertyEditor : public QDockWidget
{
    Q_OBJECT

public:
    explicit PropertyEditor(QWidget *parent = 0);
    ~PropertyEditor();

private:
    Ui::PropertyEditor *ui;
};

#endif // PROPERTYEDITOR_H
