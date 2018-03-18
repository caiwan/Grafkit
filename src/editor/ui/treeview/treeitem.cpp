#include "treeitem.h"

using namespace Idogep;

Idogep::TreeItem::TreeItem(TreeItem * parentItem) : m_parentItem(parentItem)
{
}

Idogep::TreeItem::TreeItem(const QList<QVariant>& data, TreeItem * parentItem) : m_parentItem(parentItem), m_itemData(data)
{
}

Idogep::TreeItem::~TreeItem()
{
	qDeleteAll(m_childItems);
}

void Idogep::TreeItem::addChild(TreeItem * child)
{
	m_childItems.append(child);
	child->m_parentItem = this;
}

TreeItem * Idogep::TreeItem::child(int row)
{
	if (m_childItems.size() <= row)
		return nullptr;
	return m_childItems.at(row);
}

const QVariant Idogep::TreeItem::data(int column) const
{
	if (m_itemData.size() <= column)
		return QVariant();
	return m_itemData.at(column);
}

const int Idogep::TreeItem::row() const
{
	if (m_parentItem)
		return m_parentItem->m_childItems.indexOf(const_cast<TreeItem*>(this));

	return 0;
}

// ---------------------------------------------------------------------------------------

Idogep::TreeHeaderItem::TreeHeaderItem(const QStringList & data, TreeItem * parentItem) : TreeItem(parentItem)
{
	for (size_t i = 0; i < data.size(); i++) {
		m_itemData.push_back(data.at(i));
	}
}
