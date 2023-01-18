#ifndef SCENEGRAPHVIEW_H
#define SCENEGRAPHVIEW_H

#include <qdockwidget.h>

#include "common.h"

#include "outlineModule.h"

namespace Ui {
	class SceneGraphViewWidget;
}

class QAbstractItemModel;

namespace Idogep {

	class TreeModel;
	class TreeItem;

	class SceneGraphViewWidgetModel {

	public:
	    SceneGraphViewWidgetModel();
        ~SceneGraphViewWidgetModel();

		QAbstractItemModel* GetModel() const { return m_model; }
		void SetModel(QAbstractItemModel* const & model) {m_model = model; }

	private:
		QAbstractItemModel *m_model;

	};

    /// ------------------------------------------------------------------

	class SceneGraphViewWidget : public QDockWidget, public OutlineView
	{
		Q_OBJECT

	public:
		explicit SceneGraphViewWidget(QWidget *parent = nullptr);
		~SceneGraphViewWidget();

	    void SetModel(SceneGraphViewWidgetModel * modelResource) override;

	protected:
		void RefreshView(bool force) override;

        // -- qt specific stuff 
	    // ReSharper disable CppInconsistentNaming
	private slots:
		void itemClickedSlot(const QModelIndex &index) const;
		void itemDoubleClickedSlot(const QModelIndex &index) const;

	private:
		Ui::SceneGraphViewWidget *ui;
	    // ReSharper restore CppInconsistentNaming
	};
}

#endif // SCENEGRAPHVIEW_H
