#ifndef SCENEGRAPHVIEW_H
#define SCENEGRAPHVIEW_H

#include <qdockwidget.h>

#include "common.h"

#include "outlinemodel.h"

namespace Ui {
	class SceneGraphViewWidget;
}

namespace Idogep {

	class TreeModel;
	class TreeItem;

	class SceneGraphViewWidget : public QDockWidget, public OutlineView
	{
		Q_OBJECT

	public:
		explicit SceneGraphViewWidget(QWidget *parent = 0);
		~SceneGraphViewWidget();

	protected:
		void RefreshView(bool force);

	private slots:
		void itemClickedSlot(const QModelIndex &index);
		void itemDoubleClickedSlot(const QModelIndex &index);

	private:
		Ui::SceneGraphViewWidget *ui;
	};
}

#endif // SCENEGRAPHVIEW_H
