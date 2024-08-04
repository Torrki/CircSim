#include <cmath>
#include "DrawObjs.h"

/*SURFACE DRAWABLE IMPLEMENTATION*/

void SurfaceDrawable::Draw(cairo_t* cr){
	cairo_translate(cr, (double)this->drawPoint->x, (double)this->drawPoint->y);
	cairo_rotate(cr, this->rotation);
	cairo_set_source_surface(cr, this->surface, 0, 0);
	cairo_paint(cr);
	cairo_rotate(cr, -this->rotation);
	cairo_translate(cr, (double)-this->drawPoint->x, (double)-this->drawPoint->y);
}

SurfaceDrawable::~SurfaceDrawable(){
	delete this->drawPoint;
	cairo_surface_destroy(this->surface);
}

SurfaceDrawable::SurfaceDrawable(PointInt *p, cairo_surface_t* s){
	this->drawPoint=p;
	this->surface=s;
	this->rotation=0.0;
}

SurfaceDrawable::SurfaceDrawable(PointInt *p, const char* png){
	this->drawPoint=p;
	this->surface=cairo_image_surface_create_from_png(png);
	this->rotation=0.0;
}

SurfaceDrawable::SurfaceDrawable(PointInt p, const char* png){
	PointInt *tmp=new PointInt();
	tmp->x=p.x;
	tmp->y=p.y;
	this->drawPoint=tmp;
	this->surface=cairo_image_surface_create_from_png(png);
	this->rotation=0.0;
}

SurfaceDrawable::SurfaceDrawable(PointInt p, cairo_surface_t *s){
	PointInt *tmp=new PointInt();
	tmp->x=p.x;
	tmp->y=p.y;
	this->drawPoint=tmp;
	this->surface=s;
	this->rotation=0.0;
}

void SurfaceDrawable::Rotate(double ang){
	if(ang > 0.0){
		double nGiri=std::floor( ang/(2.0*M_PI) );
		double rotPrec=this->rotation;
		this->rotation += ang - (nGiri * 2.0*M_PI);
	}
}

/*SURFACE DND IMPLEMENTATION*/

SurfaceDND::SurfaceDND(PointInt p, cairo_surface_t* s):SurfaceDrawable(p, s){
	cairo_rectangle_int_t rect={.x=p.x, .y=p.y, .width=cairo_image_surface_get_width(s), .height=cairo_image_surface_get_height(s)};
	this->regionBound=cairo_region_create_rectangle(&rect);
}

SurfaceDND::SurfaceDND(PointInt p, const char* png):SurfaceDrawable(p, png){
	cairo_rectangle_int_t rect={.x=p.x, .y=p.y, .width=this->GetWidth(), .height=this->GetHeight()};
	this->regionBound=cairo_region_create_rectangle(&rect);
}

void SurfaceDND::Drag(PointInt p, int *dx, int *dy){
	*dx= p.x-this->drawPoint->x;
	*dy= p.y-this->drawPoint->y;
	cairo_region_translate(this->regionBound, *dx, *dy);
	
	this->drawPoint->x=p.x;
	this->drawPoint->y=p.y;
}

bool SurfaceDND::PointerOn(PointInt p){
	return (bool)cairo_region_contains_point(this->regionBound, p.x, p.y);
}

void SurfaceDND::Rotate(double ang){
	if(ang != 0.0){
		double rotPrec=this->rotation;
		if(rotPrec + ang >= 2.0*M_PI){
			double nGiri=std::floor( (rotPrec+ang)/(2.0*M_PI) );
			this->rotation = (rotPrec+ang) - nGiri*2.0*M_PI;
		}else{
			this->rotation += ang;
		}
		
		int x1=this->drawPoint->x, x2;
		int y1=this->drawPoint->y, y2;
		int w=this->GetWidth();
		int h=this->GetHeight();
		x2 = x1 + (int)( std::cos(this->rotation)*(double)w - std::sin(this->rotation)*(double)h );
		y2 = y1 + (int)( std::cos(this->rotation)*(double)h + std::sin(this->rotation)*(double)w );
		
		cairo_rectangle_int_t rect;
		rect.x=std::min(x1, x2);
		rect.y=std::min(y1, y2);
		rect.width=std::abs(x2-x1);
		rect.height=std::abs(y2-y1);
		
		cairo_region_destroy(this->regionBound);
		this->regionBound=cairo_region_create_rectangle(&rect);
		this->regionBound=cairo_region_reference(this->regionBound);
	}
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

