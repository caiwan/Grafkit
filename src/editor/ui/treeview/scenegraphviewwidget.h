#ifndef SCENEGRAPHVIEW_H
#define SCENEGRAPHVIEW_H

#include <qdockwidget.h>

namespace Ui {
	class SceneGraphViewWidget;
}

namespace Idogep {
	class SceneGraphViewWidget : public QDockWidget
	{
		Q_OBJECT

	public:
		explicit SceneGraphViewWidget(QWidget *parent = 0);
		~SceneGraphViewWidget();

	private:
		Ui::SceneGraphViewWidget *ui;
	};
}

#endif // SCENEGRAPHVIEW_H
