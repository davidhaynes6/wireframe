#include "basic3d.h"
#include <QGuiApplication>

Basic3D::Basic3D(QWidget *parent) : QMainWindow(parent)
{
    //ui.setupUi(this);
    QSize rect = QGuiApplication::primaryScreen()->geometry();
    auto screenWidth = rect.width(), screenHeight = rect.height(),
        wD = screenWidth / 2, hD = 2 * wD / 3,
        wMargin = (screenWidth = wD) / 2,
        hMargin = (screenHeight = hD) / 2;
    setGeometry(wMargin, hMargin, wD, hD);
    fileMenu = menuBar()->addMenu("&File");
    viewMenu = menuBar()->addMenu("&View");

    openAct = new QAction("Open...", this);
    openAct->setShortcut(Qt::CTRL | Qt::Key_O);
    connect(openAct, &QAction::triggered, this, &Basic3D::open);

    exitAct = new QAction("Exit", this);
    exitAct->setShortcut(Qt::CTRL | Qt::Key_Q);
    connect(exitAct, &QAction::trigger, this, &Basic3D::close);

    fileMenu->addAction(openAct);
    fileMenu->addAction(exitAct);

    incrDistAct = new QAction("Increase viewing distance", this);
    incrDistAct->setShortcut(Qt::Key_I);
    connect(incrDistAct, &QAction::triggered, this, &Basic3D::incrDist);

    decrDistAct = new QAction("Decrease viewing distance", this);
    decrDistAct->setShortcut(Qt::Key_D);
    connect(decrDistAct, &QAction::triggered, this, &Basic3D::decrDist);
}

Basic3D::~Basic3D()
{}
