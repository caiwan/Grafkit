#include "resouceselector.h"

ResouceSelector::ResouceSelector(QObject *parent)
    : QAbstractItemModel(parent)
{
}

QVariant ResouceSelector::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
}

QModelIndex ResouceSelector::index(int row, int column, const QModelIndex &parent) const
{
    // FIXME: Implement me!
}

QModelIndex ResouceSelector::parent(const QModelIndex &index) const
{
    // FIXME: Implement me!
}

int ResouceSelector::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

int ResouceSelector::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

QVariant ResouceSelector::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    return QVariant();
}
