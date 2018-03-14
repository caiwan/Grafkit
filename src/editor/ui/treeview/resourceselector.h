#ifndef RESOURCESELECTOR_H
#define RESOURCESELECTOR_H

#include <QDialog>

namespace Ui {
class ResourceSelector;
}

class ResourceSelector : public QDialog
{
    Q_OBJECT

public:
    explicit ResourceSelector(QWidget *parent = 0);
    ~ResourceSelector();

private:
    Ui::ResourceSelector *ui;
};

#endif // RESOURCESELECTOR_H
