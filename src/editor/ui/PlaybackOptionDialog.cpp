#include <qfiledialog.h>

#include "PlaybackOptionDialog.h"
#include "ui_PlaybackOptionDialog.h"

Idogep::PlaybackOptionDialog::PlaybackOptionDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PlaybackOptionDialog)
{
	ui->setupUi(this);

	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	connect(ui->btn_open, SIGNAL(clicked()), this, SLOT(openFileSlot()));

}

Idogep::PlaybackOptionDialog::~PlaybackOptionDialog()
{
	delete ui;
}

void Idogep::PlaybackOptionDialog::openFileSlot()
{
	QFileDialog dialog(this, tr("Open music"), "", tr("Audio Files (*.mp3 *.ogg *.wav)"));
	dialog.setFileMode(QFileDialog::ExistingFile);

	QStringList fileNames;
	if (dialog.exec()) {
		fileNames = dialog.selectedFiles();
		m_filename.clear();
		if (!fileNames.empty()) {
			m_filename = fileNames.at(0);
			ui->btn_open->setText(m_filename);
		}
	}

}
