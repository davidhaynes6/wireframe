#include "wireframe.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    wireframe w;
    w.show();
    return a.exec();
}
