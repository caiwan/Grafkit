#pragma once

/*
QT widget that holds a context for GK Rendering context
*/

#include <QWidget>

#include "render/renderer.h"


#include "utils/Event.h"
#include "utils/ViewModule.h"

class QPaintEngine;

class QResizeEvent;
class QPaintEvent;

namespace Idogep {
    class QGrafkitContextWidget : public QWidget, public View

    {
        Q_OBJECT
    public:

        QGrafkitContextWidget(Grafkit::Renderer & render, QWidget * parent = nullptr);
        ~QGrafkitContextWidget();

        void Initialize();

        QPaintEngine* paintEngine();

        void paintEvent(QPaintEvent *event) override;
        void resizeEvent(QResizeEvent *event) override;

        Event<Grafkit::Renderer &> onResizeSurface;

    protected:
        void RefreshView(bool force) override;

    private:
        Grafkit::Renderer &m_render;
        bool m_isInited;
    };
}
