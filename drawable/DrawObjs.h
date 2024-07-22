#pragma once

#include "Point.h"
#include "IDrawable.h"

#define LINE_STROKE 3

enum Direction{DIR_UP=1, DIR_DOWN=2, DIR_LEFT=4, DIR_RIGHT=8};

class Line: public IDrawable, public IDragable{
	cairo_region_t *region;
	PointInt *start, *end;
	int dir;
public:
	Line(PointInt* start, PointInt* end);
	~Line();
	int GetDirection() {return this->dir;};
	PointInt* GetStart() {return this->start;};
	PointInt* GetEnd() {return this->end;};
	void Drag(int x, int y, int *dx, int *dy){};
	bool PointerOn(int x, int y){ return (bool)cairo_region_contains_point(this->region, x, y); };
	void Draw(cairo_t *cr);
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

