#include <cmath>
#include "DrawObjs.h"

/*SURFACE DRAWABLE IMPLEMENTATION*/

void SurfaceDrawable::Draw(cairo_t* cr){
	cairo_set_source_surface(cr, this->surface, this->drawPoint->x, this->drawPoint->y);
	cairo_paint(cr);
}

/*SURFACE DND IMPLEMENTATION*/

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

/*LINE IMPLEMENTATION*/

Line::Line(PointInt* start, PointInt* end){
	this->start=start;
	this->end=end;
	cairo_rectangle_int_t rect;
	
	if(start->x==end->x){
		rect={.x=start->x, .y=std::min(start->y, end->y), .width=LINE_STROKE, .height=std::abs(end->y-start->y)};
		if(end->y > start->y) this->dir=DIR_DOWN;
		else this->dir=DIR_UP;
	}else if(start->y==end->y){
		rect={.x=std::min(start->x, end->x), .y=start->y, .width=std::abs(end->x-start->x), .height=LINE_STROKE};
		if(end->x > start->x) this->dir=DIR_RIGHT;
		else this->dir=DIR_LEFT;
	}
	//TODO: else che genera una eccezione
	this->region=cairo_region_create_rectangle(&rect);
}

void Line::Draw(cairo_t *cr){
	//Se non sono nel punto di start ci vado, linea discontinua
	double cx, cy;
	cairo_get_current_point(cr, &cx, &cy);
	if(( (int)cx != this->start->x ) || ( (int)cy != this->start->y ))
	{
		cairo_move_to(cr, this->start->x, this->start->y);
	}
	cairo_line_to(cr, this->end->x, this->end->y);
}

Line::~Line(){
	cairo_region_destroy(this->region);
}
