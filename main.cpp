#include "cubewireframe.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CubeWireframe w;
    w.show();
    return a.exec();
}
