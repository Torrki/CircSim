#include "IDrawable.h"

void SurfaceDrawable::Draw(cairo_t* cr){
	cairo_set_source_surface(cr, this->surface, this->drawPoint.x, this->drawPoint.y);
	cairo_paint(cr);
}

void SurfaceDND::Drag(double x, double y){
	int dx= (int)x-(int)this->drawPoint.x;
	int dy= (int)y-(int)this->drawPoint.y;
	cairo_region_translate(this->regionBound, dx, dy);
	
	this->drawPoint.x=x;
	this->drawPoint.y=y;
}

bool SurfaceDND::PointerOn(double x, double y){
	return (bool)cairo_region_contains_point(this->regionBound, (int)x, (int)y);
}

