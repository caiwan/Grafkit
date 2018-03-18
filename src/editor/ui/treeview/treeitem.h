#pragma once 

#include <qlist.h>
#include <qvariant.h>

namespace Idogep {

	class TreeItem
	{
	public:
		explicit TreeItem(TreeItem *parentItem = nullptr);
		explicit TreeItem(const QList<QVariant> &data, TreeItem *parentItem = nullptr);
		virtual ~TreeItem();

		void addChild(TreeItem *child);

		TreeItem *child(int row);
		const int childCount() const { return m_childItems.size(); }
		const int columnCount() const { return m_itemData.size(); }
		const QVariant data(int column) const;
		const int row() const;
		
		TreeItem *parentItem() { return m_parentItem; }
		void setParentItem(TreeItem * parentItem) { m_parentItem = parentItem; }

	protected:
		QList<TreeItem*> m_childItems;
		QList<QVariant> m_itemData;
		TreeItem *m_parentItem;
	};

	class TreeHeaderItem  : public TreeItem{
	public:
		explicit TreeHeaderItem(const QStringList &data, TreeItem *parentItem = nullptr);

	};

}
