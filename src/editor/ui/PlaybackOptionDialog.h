#pragma once

#include <QDialog>

namespace Ui {
	class PlaybackOptionDialog;
}

namespace Idogep {

	class PlaybackOptionDialog : public QDialog
	{
		Q_OBJECT

	public:
		explicit PlaybackOptionDialog(QWidget *parent = 0);
		~PlaybackOptionDialog();

		QString fileName() { return m_filename; }

		private slots:
		void openFileSlot();

	private:
		Ui::PlaybackOptionDialog *ui;

		QString m_filename;

	};

}

