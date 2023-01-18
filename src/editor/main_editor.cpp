#include <QApplication>
#include <QTimer>
#include <QFile>

#include "main_editor.h"

#include "Application.h"
#include "proxies/LoggerProxy.h"

using namespace Idogep;

EditorApplicationQt::EditorApplicationQt(int argc, char** argv) : EditorApplication(argc, argv), m_qApp(argc, argv)
{
    QCoreApplication::setOrganizationName("IndustrialRevolutioners");
    QCoreApplication::setOrganizationDomain("caiwan.github.io");
    QCoreApplication::setApplicationName(APP_NAME);

    QFile f(":/css/global.css");
    assert(f.open(QFile::ReadOnly));
    m_qApp.setStyleSheet(f.readAll());
    f.close();
}

EditorApplicationQt::~EditorApplicationQt()
{
    //... 
}

int EditorApplicationQt::ExecuteParentFramework()
{
    return m_qApp.exec();
}

void EditorApplicationQt::StartLoaderThread()
{
    auto *loader = new LoaderThread();
    onLoaderFinished += Delegate(loader, &LoaderThread::deleteLater);
    connect(loader, SIGNAL(finished()), this, SLOT(loaderFinishedSlot()));
    loader->start();
}

void EditorApplicationQt::NextTick()
{
    QTimer::singleShot(0, this, SLOT(mainloopSlot()));
}

// ========================================================================================

int main(int argc, char **argv)
{
    Q_INIT_RESOURCE(resources);

    qInstallMessageHandler(myMessageOutput);

    auto app = new EditorApplicationQt(argc, argv);
    return app->Execute();
}

