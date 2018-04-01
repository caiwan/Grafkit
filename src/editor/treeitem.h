#pragma once 

#include <qlist.h>
#include <qvariant.h>
#include <qicon.h>

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

		QPixmap icon() { return m_icon; }
		void setIcon(QPixmap icon) { m_icon = icon; }

	protected:
		QList<TreeItem*> m_childItems;
		QList<QVariant> m_itemData;
		TreeItem *m_parentItem;
		QPixmap m_icon;
	};

	class TreeHeaderItem  : public TreeItem{
	public:
		explicit TreeHeaderItem(const QStringList &data, TreeItem *parentItem = nullptr);

	};

}
