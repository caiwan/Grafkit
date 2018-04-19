#include "treemodel.h"
#include "treeitem.h"

#include <qdebug>

using namespace Idogep;

Idogep::TreeModel::TreeModel(QObject *parent) : QAbstractItemModel(parent), m_rootItem(nullptr)
{
}

Idogep::TreeModel::~TreeModel()
{
	delete m_rootItem;
}

QVariant Idogep::TreeModel::data(const QModelIndex & index, int role) const
{
	if (!index.isValid())
		return QVariant();

	TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

	switch (role) {
	case Qt::DisplayRole:
		return item->data(index.column());

	case Qt::DecorationRole:
		return item->icon();

	default:
		return QVariant();
	}

}

Qt::ItemFlags Idogep::TreeModel::flags(const QModelIndex & index) const
{
	if (!index.isValid())
		return 0;

	return QAbstractItemModel::flags(index);
}

QVariant Idogep::TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return m_rootItem->data(section);

	return QVariant();
}

QModelIndex Idogep::TreeModel::index(int row, int column, const QModelIndex & parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	TreeItem *parentItem;

	if (!parent.isValid())
		parentItem = m_rootItem;
	else
		parentItem = static_cast<TreeItem*>(parent.internalPointer());

	TreeItem *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

QModelIndex Idogep::TreeModel::parent(const QModelIndex & index) const
{
	if (!index.isValid())
		return QModelIndex();

	TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
	TreeItem *parentItem = childItem->parentItem();

	if (parentItem == m_rootItem)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

int Idogep::TreeModel::rowCount(const QModelIndex & parent) const
{
	TreeItem *parentItem;
	if (parent.column() > 0)
		return 0;

	if (!parent.isValid())
		parentItem = m_rootItem;
	else
		parentItem = static_cast<TreeItem*>(parent.internalPointer());

	if (parentItem)
		return parentItem->childCount();
	else
		return 0;
}

int Idogep::TreeModel::columnCount(const QModelIndex & parent) const
{
	if (parent.isValid())
		return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
	else if (m_rootItem)
		return m_rootItem->columnCount();
	else
		return 0;
}

void Idogep::TreeModel::BuildModel()
{
	if (m_rootItem)
		delete m_rootItem;

	const QStringList headerString = Header();
	m_rootItem = new TreeHeaderItem(headerString);
	Build(m_rootItem);
}
