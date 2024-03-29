#pragma once
#include <QtWidgets>
struct Point2D {
	qreal x, y;
	Point2D(qreal x = 0, qreal y = 0) {
		this->x = x; this->y = y;
	}
};

struct Point3D {
	qreal x, y, z;
	Point3D(qreal x = 0, qreal y = 0, qreal z = 0) {
		this->x = x; this->y = y; this->z = z;
	}
};