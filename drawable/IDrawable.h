#pragma once
#include <cairo/cairo.h>
#include "Point.h"

class IDrawable{
public:
	virtual void Draw(cairo_t *cr)=0;	//funzione virtuale per disegnare
};

class IDragable{
public:
	virtual void Drag(int x, int y, int *dx, int *dy)=0; 	//funzione virtuale per Drag n Drop
	virtual bool PointerOn(int x, int y){return false;};	//funzione virtuale per il cursore
};

class SurfaceDrawable: public IDrawable{
protected:
	PointInt *drawPoint;
	cairo_surface_t* surface;
public:
	~SurfaceDrawable();
	int GetX(){return this->drawPoint->x;};
	int GetY(){return this->drawPoint->y;};
	int GetWidth() {return cairo_image_surface_get_width(this->surface);};
	int GetHeight() {return cairo_image_surface_get_height(this->surface);};
	void Draw(cairo_t* cr);
};

class SurfaceDND: public SurfaceDrawable, public IDragable{
protected:
	cairo_region_t* regionBound;
public:
	~SurfaceDND();
	virtual void Drag(int x, int y, int *dx, int *dy);
	bool PointerOn(int x, int y);
};

