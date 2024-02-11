#pragma once
#include "basic3d.h"

class CubeWireframe : public Basic3D
{
	Q_OBJECT
public:
	CubeWireframe(QWidget* parent = nullptr);
protected:
	void paintEvent(QPaintEvent* e) override;
	void defineObject();
};
