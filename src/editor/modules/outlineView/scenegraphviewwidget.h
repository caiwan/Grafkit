#ifndef SCENEGRAPHVIEW_H
#define SCENEGRAPHVIEW_H

#include <qdockwidget.h>

#include "Event.h"

namespace Ui {
	class SceneGraphViewWidget;
}

namespace Idogep {

	class TreeModel;
	class TreeItem;

	class SceneGraphViewWidget : public QDockWidget
	{
		Q_OBJECT

	public:
		explicit SceneGraphViewWidget(QWidget *parent = 0);
		~SceneGraphViewWidget();

		void setModel(TreeModel* model);

		Event<TreeItem*> onItemSelected;
		//Event<TreeItem*> onItemHighlighted;

	private slots:
		void itemClickedSlot(const QModelIndex &index);
		void itemDoubleClickedSlot(const QModelIndex &index);

	private:
		Ui::SceneGraphViewWidget *ui;
	};
}

#endif // SCENEGRAPHVIEW_H
