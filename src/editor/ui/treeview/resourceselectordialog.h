#ifndef RESOURCESELECTOR_H
#define RESOURCESELECTOR_H

#include <QDialog>

namespace Ui {
class ResourceSelectorDialog;
}

class ResourceSelectorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResourceSelectorDialog(QWidget *parent = 0);
    ~ResourceSelectorDialog();

private:
    Ui::ResourceSelectorDialog *ui;
};

#endif // RESOURCESELECTOR_H
