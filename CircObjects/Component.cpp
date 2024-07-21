#include "Component.h"
#include <stdio.h>

/*HOTPOINT IMPLEMENTATION*/

Hotpoint::Hotpoint(int x, int y, int dim_bound){
	this->x=x;
	this->y=y;
	cairo_rectangle_int_t rect={.x=x-(dim_bound/2), .y=y-(dim_bound/2), .width=dim_bound, .height=dim_bound};
	this->region=cairo_region_create_rectangle(&rect);
	this->region=cairo_region_reference(this->region);
}

Hotpoint::~Hotpoint(){
	cairo_region_destroy(this->region);
}

bool Hotpoint::HotpointOn(int x, int y){
	return (bool)cairo_region_contains_point(this->region, x, y);
}

/*COMPONENT IMPLEMENTATION*/

Component::Component(const char *path_symbol, int xi, int yi){
	this->surface=cairo_image_surface_create_from_png(path_symbol);
	this->drawPoint=new PointInt();
	this->drawPoint->x=xi;
	this->drawPoint->y=yi;

	int w=this->GetWidth();
	int h=this->GetHeight();
	cairo_rectangle_int_t rect={.x=xi, .y=yi, .width=w, .height=h};	
	this->regionBound=cairo_region_create_rectangle(&rect);
	
	this->regionBound=cairo_region_reference(this->regionBound);
	this->surface=cairo_surface_reference(this->surface);
}

Hotpoint* Component::HotpointOver(int x, int y){
	for(std::list<Hotpoint*>::iterator it=this->hotpoints.begin(); it != this->hotpoints.end(); it++){
		if((*it)->HotpointOn(x, y)) return *it;
	}
	return NULL;
}

void Component::Drag(int x, int y){
	int dx, dy;
	SurfaceDND::Drag(x,y, &dx, &dy);
	for(std::list<Hotpoint*>::iterator it=this->hotpoints.begin(); it != this->hotpoints.end(); it++){
		cairo_region_translate((*it)->GetRegion(), dx, dy);
		(*it)->x += dx;
		(*it)->y += dy;
	}
}

Component::~Component(){
	while(!this->hotpoints.empty()){
		Hotpoint* hp=this->hotpoints.back();
		this->hotpoints.pop_back();
		delete hp;
	}
	
	this->~SurfaceDND();
}

/*RESISTOR-CAPACITOR-INDUCTOR*/

Resistor::Resistor(int xi, int yi, float ohm):Component("./resources/imgs/resistor.png", xi, yi){
	this->Value=ohm;
	
	int w=this->GetWidth();
	int h=this->GetHeight();
	this->hotpoints.push_back( new Hotpoint(xi, yi+(int)((float)h/2.0f), HOTPOINT_EDGE) );
	this->hotpoints.push_back( new Hotpoint(xi+w, yi+(int)((float)h/2.0f), HOTPOINT_EDGE) );
}

Capacitor::Capacitor(int xi, int yi, float fahrad):Component("./resources/imgs/capacitor.png", xi, yi){
	this->Value=fahrad;
	
	int w=this->GetWidth();
	int h=this->GetHeight();
	this->hotpoints.push_back( new Hotpoint(xi, yi+(int)((float)h/2.0f), HOTPOINT_EDGE) );
	this->hotpoints.push_back( new Hotpoint(xi+w, yi+(int)((float)h/2.0f), HOTPOINT_EDGE) );
}

Inductor::Inductor(int xi, int yi, float henry):Component("./resources/imgs/inductor.png", xi, yi){
	this->Value=henry;
	
	int w=this->GetWidth();
	int h=this->GetHeight();
	this->hotpoints.push_back( new Hotpoint(xi, yi+h, HOTPOINT_EDGE) );
	this->hotpoints.push_back( new Hotpoint(xi+w, yi+h, HOTPOINT_EDGE) );
}

