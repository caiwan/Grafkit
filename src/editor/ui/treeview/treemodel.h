#pragma once 

#include <qabstractitemmodel.h>

#include "Event.h"

namespace Idogep {

	class TreeItem;

	class TreeModel : public QAbstractItemModel
	{
		Q_OBJECT

	public:
		explicit TreeModel(QObject *parent = nullptr);
		virtual ~TreeModel();

		// events
		Event<> onDataChanged;
		Event<TreeModel*> onSelected; // self

		// overrides
		QVariant data(const QModelIndex &index, int role) const override;
		Qt::ItemFlags flags(const QModelIndex &index) const override;
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
		QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
		QModelIndex parent(const QModelIndex &index) const override;
		int rowCount(const QModelIndex &parent = QModelIndex()) const override;
		int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	public:
		void BuildModel();

	protected:
		// pure virtuals
		virtual QStringList Header() = 0;
		virtual	void Build(TreeItem * parentItem) = 0;

		TreeItem *m_rootItem;
	};
}