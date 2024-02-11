#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_wireframe.h"
#include "points.h"
#include <vector>
#include <set>

struct IntPair {
	int i, j;
	IntPair(int i, int j) {
		if (i < j) {
			this->i = i; this->j = j;
		}
		else {
			this->i = j; this->j = i;
		}
	}

	// It might be beneficial to ensure the operator< is friend to allow access to private members if any
	friend bool operator<(const IntPair& lhs, const IntPair& rhs) {
		if (lhs.i < rhs.i) return true;
		else if (lhs.i == rhs.i) return lhs.j < rhs.j;
		return false;
	}
};


struct Polygon3D {
	std::vector<int> nrs; // vertex numbers (n elements)
	qreal a, b, c, h;     // of the plane equation ax + by + cz = h
};

typedef std::set<IntPair, std::less<IntPair> > EdgeSet;

class Basic3D : public QMainWindow
{
	Q_OBJECT

public:
	Basic3D(QWidget* parent = nullptr);
	~Basic3D();

	qreal xsMin, xsMax, ysMin, ysMax, xsRange = 0, ysRange,
		theta0Deg = 30, theta0 = theta0Deg * M_PI / 180,
		thetaDeg = theta0Deg, theta = theta0,
		phi0Deg = 70, phi0 = phi0Deg * M_PI / 180,
		phiDeg = phi0Deg, phi = phi0,
		xwMin, xwMax, ywMin, ywMax, zwMin, zwMax,
		xC, yC, zC, rho, rho0, rhoMin, rhoMax, d;

	const int hMenu = 21;
	int wDraw, hDraw, xDevCenter, yDevCenter;
	bool fileDialogDone = false;
	QString fileName = "";
	QMenu* viewMenu, *fileMenu;
	QAction* openAct, *exitAct, *incrDistAct, *decrDistAct,
		*resetAct, *angleChangeAct;
	int xPress, yPress;
	void newObjectTest();
	void computePerspCoord();
	void boundsCenterDistance();
	qreal xsCenter, ysCenter;
	void inputObject();
	inline int xDev(qreal x) const {
		return xDevCenter + int(d * (x - xsCenter));
	}
	inline int yDev(qreal y) const {
		return yDevCenter - int(d * (y - ysCenter));
	}
	bool different(int i, int j) const;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void resizeEvent(QResizeEvent* e) override;

public:
	void simpleMessage(const QString& title, const QString& mes);
	std::vector<Point3D> world, eye;
	std::vector<Point2D> scr;
	EdgeSet edges;
	std::vector<Polygon3D> polyList;
	bool defaultPaint = true;
	qreal zNear, zFar, eps1;

public slots:
	void open();
	void incrDist();
	void decrDist();
	void resetViewpoint();
	void angleChange();

private:
    //Ui::wireframeClass ui;
};
