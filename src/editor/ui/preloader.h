#pragma once

#include <QDialog>

#include "utils/ResourceManager.h"

namespace Ui {
	class Preloader;
}

namespace Idogep {

	class Preloader : public QDialog, public Grafkit::IPreloadEvents
	{
		Q_OBJECT

	public:
		explicit Preloader(QWidget *parent = nullptr);
		~Preloader();

		virtual void OnBeginLoad();
		virtual void OnElemLoad(size_t actual, size_t count);
		virtual void OnEndLoad();

		void FocusChanged(QWidget * const & from, QWidget * const & to);

	private:
		Ui::Preloader *ui;
	};

}

