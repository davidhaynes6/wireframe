#include "basic3d.h"
#include <QGuiApplication>

Basic3D::Basic3D(QWidget* parent) : QMainWindow(parent)
{
    //ui.setupUi(this);
    QRect rect = QGuiApplication::primaryScreen()->geometry();
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

    angleChangeAct = new QAction("&Change viewpoint...", this);
    connect(angleChangeAct, &QAction::triggered, this, &Basic3D::angleChange);

    resetAct = new QAction("&Restore default viewpoint", this);
    connect(resetAct, &QAction::triggered, this, &Basic3D::resetViewpoint);

    viewMenu->addAction(incrDistAct);
    viewMenu->addAction(decrDistAct);
    viewMenu->addAction(angleChangeAct);
    viewMenu->addAction(resetAct);
    menuBar()->addMenu(viewMenu);
}

Basic3D::~Basic3D()
{}

void Basic3D::simpleMessage(const QString& title, const QString& mes) {
    QMessageBox msgBox;
    msgBox.setWindowTitle(title);
    msgBox.setText(mes);
    msgBox.exec();
}

void Basic3D::boundsCenterDistance() {
    xwMin = 1e6;
    xwMax = -xwMin;
    ywMin = xwMin;
    ywMax = xwMax;
    zwMin = xwMin; 
    zwMax = xwMax;

    for (auto i = 0; i < world.size(); i++) {
        qreal x = world[i].x, y = world[i].y, z = world[i].z;
        if (x > 1e29)
            continue;
        xwMin = std::min(x, xwMin);
        xwMax = std::max(x, xwMax);

        ywMin = std::min(y, ywMin);
        ywMax = std::max(y, ywMax);

        zwMin = std::min(z, zwMin);
        zwMax = std::max(z, zwMax);
    }

    xC = (xwMin + xwMax) / 2;
    yC = (ywMin + ywMax) / 2;
    zC = (zwMin + zwMax) / 2;

    qreal xwRange = xwMax - xwMin + 0.001;
    qreal ywRange = ywMax - ywMin + 0.001;
    qreal zwRange = zwMax - zwMin + 0.001;

    rhoMin = sqrt(xwRange * xwRange + ywRange * ywRange + zwRange * zwRange);
    rhoMax = 40 * rhoMin;
    rho0 = rho = 3 * rhoMin;
}

bool Basic3D::different(int i, int j) const 
{
    return world[i].x != world[j].x || 
        world[i].y != world[j].y || 
        world[i].z != world[j].z;
}

void Basic3D::inputObject()
{
    QFile file(fileName);
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream in(&file);
    bool ok;
    QString stri, strx, stry, strz;
    world.clear();
    edges.clear();
    polyList.clear();

    //Find highest vertex number:
    auto maxnr = 0;
    while (true) {
        int i;
        in >> stri;
        i = stri.toInt(&ok);
        if (!ok || in.atEnd()) {
            break;
        }
        maxnr = std::max(i, maxnr);
        in >> strx >> stry >> strz;
    }

    world.resize(maxnr + 1);

    for (auto i = 0; i <= maxnr; ++i) {
        world[i].x = 1e30;
    }

    in.seek(0);

    for (;;) {
        int i;
        qreal x, y, z;
        in >> stri;
        i = stri.toInt(&ok);
        if (!ok || in.atEnd()) {
            break;
        }
        in >> strx >> stry >> strz;
        x = strx.toDouble(&ok);
        y = stry.toDouble(&ok);
        z = strz.toDouble(&ok);
        world[i] = Point3D(x, y, z);
    }

    boundsCenterDistance();
    if (stri.length() > 0 && stri.at(0) == 'F') {
        // Read polygons and use its edges i-j:
        QString stri;
        int j, len;
        auto errorfound = false;
        // Loop through the polygons:
        while (!errorfound) {
            Polygon3D polygon;
            // Loop through the vertices of a polygon:
            while (!errorfound) {
                in >> stri;
                len = stri.length();
                if (len > 0) {
                    bool finalvertex = (len > 1 && stri.at(len - 1) == '.');
                    if (finalvertex) {
                        len--;
                    }
                    j = stri.left(len).toInt(&ok);
                    if (ok) {
                        int nr = std::abs(j);
                        if (nr >= world.size() || world[nr].x > 1e29) {
                            simpleMessage("Error", "Undefined vertex: " + QString::number(nr));
                            errorfound = true;
                            break;
                        }
                        int size = polygon.nrs.size();
                        if (size == 0 ||
                            (different(nr, std::abs(polygon.nrs[size - 1])) &&
                                (!finalvertex ||
                                    different(nr, std::abs(polygon.nrs[0]))))) {
                            polygon.nrs.push_back(j);
                        }
                        if (!finalvertex) {
                            continue;
                        }
                    }
                    else {
                        break;
                    }
                }
                break;
            }
            if (polygon.nrs.size() >= 2) {
                polyList.push_back(polygon);
            }
            if (in.atEnd() || !ok) {
                break;
            }
        }
    }
    file.close();
}

void Basic3D::open() {
    fileName = QFileDialog::getOpenFileName(this, "Open File", fileName, "3D data files (*.dat)");
    fileDialogDone = true;
}

void Basic3D::newObjectTest() {
    if (fileDialogDone && fileName.length() > 0) {
        inputObject();
        computePerspCoord();
    }
    fileDialogDone = false;
}

void Basic3D::incrDist() {
    rho = std::min(rhoMax, rho * 1.5);
    computePerspCoord();
    if (defaultPaint) {
        repaint();
    }
}

void Basic3D::decrDist() {
    rho = std::min(rhoMax, rho / 1.5);
    computePerspCoord();
    if (defaultPaint) {
        repaint();
    }
}

void Basic3D::resetViewpoint() {
    rho = rho0;
    thetaDeg = theta0Deg;
    theta = theta0;
    phiDeg = phi0Deg;
    phi = phi0;
    computePerspCoord();
    if (defaultPaint) {
        repaint();
    }
}

void Basic3D::angleChange() {
    bool ok;
    qreal thetaDeg1;
    qreal phiDeg1;
    qreal rho1;

    thetaDeg1 = QInputDialog::getDouble(this, "theta (deg)", "Angle in hor. plane", phiDeg, -180, 180, 1, &ok);

    if (ok) {
        phiDeg1 = QInputDialog::getDouble(this, "phi (deg)", "Angle in vert. plane", phiDeg, 0, 180, 1, &ok);
    }
    if (ok) {
        rho1 = QInputDialog::getDouble(this, "rho", "Viewing Distance", phiDeg, 1, 1e9, 1, &ok);
    }
    if (ok) {
        thetaDeg = thetaDeg1;
        phiDeg = phiDeg1;
        rho = rho1;
        if (rho < rhoMin) {
            rho = rhoMin;
        }
        if (rho > rhoMax) {
            rho = rhoMax;
        }
        if (rho != rho1) {
            simpleMessage("Adapted to range", "Viewing Distance stet to " + QString::number(rho, 'f', 1));
        }
        theta = thetaDeg * M_PI / 180;
        phi = phiDeg * M_PI / 180;
        computePerspCoord();
        if (defaultPaint) {
            repaint();
        }
    }
}

void Basic3D::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        xPress = (event->pos()).x();
        yPress = (event->pos()).y();
    }
}

void Basic3D::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        QPoint release = event->pos();
        int dx = release.x() - xPress, dy = release.y() - yPress;
        if (std::abs(dx) > std::abs(dy)) {
            thetaDeg -= 0.001 * dx * width();
            theta = thetaDeg * M_PI / 180;
        }
        else {
            phiDeg -= 0.001 * dy * height();
            if (phiDeg > 180) {
                phiDeg = 180;
            }
            if (phiDeg < 0) {
                phiDeg = 0;
            }
            phi = phiDeg * M_PI / 180;
        }
        computePerspCoord();
        if (defaultPaint) {
            repaint();
        }
    }
}

void Basic3D::resizeEvent(QResizeEvent* e) {
    wDraw = width();
    hDraw = height() - hMenu;
    xDevCenter = wDraw / 2;
    yDevCenter = hMenu + hDraw / 2;
    if (xsRange != 0) {
        d = 0.85 * std::min(wDraw / xsRange, hDraw / ysRange);
    }
    repaint();
}

void Basic3D::computePerspCoord()
{
    auto n = world.size();
    xsMin = 1e6;
    xsMax = -xsMin;
    ysMin = xsMin;
    ysMax = xsMax;
    qreal costh = cos(theta);
    qreal sinth = sin(theta);
    qreal cosph = cos(phi);
    qreal sinph = sin(phi);

    qreal V11 = -sinth;
    qreal V12 = -cosph * cosph;
    qreal V13 = sinph * costh;

    qreal V21 = costh;
    qreal V22 = -cosph * sinth;
    qreal V23 = sinph * sinth;

    qreal V32 = sinph;
    qreal V33 = cosph;
    qreal V43 = -rho;

    eye.resize(n);
    scr.resize(n);
    zNear = -1e20;
    zFar = 1e20;

    for (auto i = 0; i < n; ++i) {
        if (world[i].x > 1e29) {
            continue;
        }
        // Eye coordinates
        Point3D p = world[i], pe;
        qreal x = p.x - xC; 
        qreal y = p.y - yC; 
        qreal z = p.z - zC;

        pe.x = V11 * x + V21 * y;
        pe.y = V12 * x + V22 * y + V32 * z;
        pe.z = V13 * x + V23 * y + V33 * z + V43;

        zNear = std::max(zNear, pe.z);
        zFar = std::min(zFar, pe.z);

        eye[i] = pe;

        // Screen Coordinates:
        qreal X = -pe.x / pe.z;
        qreal Y = -pe.y / pe.z;
        xsMin = std::min(X, xsMin);
        ysMin = std::min(Y, ysMin);

        xsMax = std::max(X, xsMax);
        ysMax = std::max(Y, ysMax);
        scr[i] = Point2D(X, Y);
    }
    eps1 = 0.001 * (zNear - zFar);
    if (n == 0) {
        return;
    }

    xsRange = xsMax - xsMin + 1e-9;
    ysRange = ysMax - ysMin + 1e-9;
    d = 0.85 * std::min(wDraw / xsRange, hDraw / ysRange);
    xsCenter = 0.5 * (xsMin + xsMax);
    ysCenter = 0.5 * (ysMin + ysMax);
}