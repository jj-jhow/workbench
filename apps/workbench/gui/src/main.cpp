#include <QApplication>
#include <QSurfaceFormat>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    // Set the desired OpenGL version for Hydra.
    // This needs to be done before the QApplication is created.
    QSurfaceFormat format;
    format.setVersion(4, 5);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}
