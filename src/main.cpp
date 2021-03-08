#include "mainwindow.h"
#include "datawidget.h"

#include <QApplication>
#include <QSurfaceFormat>

// Global Qt settings for OpenGL Contexts
// Has to be initialized before application gets created.
void setupGlobalRenderingSettings() {
    QSurfaceFormat format{};
    format.setVersion(4, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setOptions(QSurfaceFormat::DebugContext);
    format.setSwapInterval(1);

    QSurfaceFormat::setDefaultFormat(format);

    // Set all rendering to use same opengl-context
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
}

int main(int argc, char *argv[])
{
    setupGlobalRenderingSettings();

    QApplication a(argc, argv);

    //Main window
    MainWindow w;
    w.show();

    //Separate data window
    DataWidget dw;
    dw.show();

    return a.exec();
}
