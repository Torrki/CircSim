#include "IDrawable.h"

void SurfaceDrawable::Draw(cairo_t* cr){
	cairo_set_source_surface(cr, this->surface, this->drawPoint->x, this->drawPoint->y);
	cairo_paint(cr);
}

void SurfaceDND::Drag(int x, int y, int *dx, int *dy){
	*dx= x-this->drawPoint->x;
	*dy= y-this->drawPoint->y;
	cairo_region_translate(this->regionBound, *dx, *dy);
	
	this->drawPoint->x=x;
	this->drawPoint->y=y;
}

SurfaceDrawable::~SurfaceDrawable(){
	delete this->drawPoint;
	cairo_surface_destroy(this->surface);
}

bool SurfaceDND::PointerOn(int x, int y){
	return (bool)cairo_region_contains_point(this->regionBound, x, y);
}

SurfaceDND::~SurfaceDND(){
	cairo_region_destroy(this->regionBound);
	this->~SurfaceDrawable();
}

