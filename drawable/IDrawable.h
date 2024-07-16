#pragma once
#include <cairo/cairo.h>
#include "Point.h"

class IDrawable{
public:
	virtual void Draw(cairo_t *cr)=0;	//funzione virtuale per disegnare
};

class IDragable{
public:
	virtual void Drag(double x, double y)=0; 			//funzione virtuale per Drag n Drop
	virtual bool PointerOn(double x, double y)=0;	//funzione virtuale per il cursore
};

class SurfaceDrawable: public IDrawable{
protected:
	PointDouble drawPoint;
	cairo_surface_t* surface;
public:
	double GetX(){return this->drawPoint.x;};
	double GetY(){return this->drawPoint.y;};
	void Draw(cairo_t* cr);
};

class SurfaceDND: public SurfaceDrawable, public IDragable{
protected:
	cairo_region_t* regionBound;
public:
	void Drag(double x, double y);
	bool PointerOn(double x, double y);
};

