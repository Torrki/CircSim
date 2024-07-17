#include "Component.h"
#include <stdio.h>

Hotpoint::Hotpoint(int x, int y){
	this->x=x;
	this->y=y;
	cairo_rectangle_int_t rect={.x=x-(HOTPOINT_EDGE/2), .y=y-(HOTPOINT_EDGE/2), .width=HOTPOINT_EDGE, .height=HOTPOINT_EDGE};
	this->region=cairo_region_create_rectangle(&rect);
}

bool Hotpoint::HotpointOn(int x, int y){
	return (bool)cairo_region_contains_point(this->region, x, y);
}

Component::Component(const char *path_symbol, double xi, double yi){
	this->surface=cairo_image_surface_create_from_png(path_symbol);
	this->drawPoint={.x=xi, .y=yi};

	int w=this->GetWidth();
	int h=this->GetHeight();
	cairo_rectangle_int_t rect={.x=(int)xi, .y=(int)yi, .width=w, .height=h};	
	this->regionBound=cairo_region_create_rectangle(&rect);
}

Hotpoint* Component::HotpointOver(double x, double y){
	for(std::list<Hotpoint>::iterator it=this->hotpoints.begin(); it != this->hotpoints.end(); it++){
		if(it->HotpointOn((int)x, (int)y)) return &(*it);
	}
	return NULL;
}

void Component::Drag(double x, double y){
	int dx, dy;
	SurfaceDND::Drag(x,y, &dx, &dy);
	//printf("%d\t%d\n", dx, dy);
	for(std::list<Hotpoint>::iterator it=this->hotpoints.begin(); it != this->hotpoints.end(); it++){
		cairo_region_translate(it->GetRegion(), dx, dy);
		it->x += dx;
		it->y += dy;
	}
}

Resistor::Resistor(double xi, double yi, float ohm):Component("./resources/imgs/resistor.png", xi, yi){
	this->Value=ohm;
	
	int w=this->GetWidth();
	int h=this->GetHeight();
	this->hotpoints.push_back( Hotpoint((int)xi, (int)yi+(int)((float)h/2.0f)) );
	this->hotpoints.push_back( Hotpoint((int)xi+w, (int)yi+(int)((float)h/2.0f)) );
}

Capacitor::Capacitor(double xi, double yi, float fahrad):Component("./resources/imgs/capacitor.png", xi, yi){
	this->Value=fahrad;
	
	int w=this->GetWidth();
	int h=this->GetHeight();
	this->hotpoints.push_back( Hotpoint((int)xi, (int)yi+(int)((float)h/2.0f)) );
	this->hotpoints.push_back( Hotpoint((int)xi+w, (int)yi+(int)((float)h/2.0f)) );
}

Inductor::Inductor(double xi, double yi, float henry):Component("./resources/imgs/inductor.png", xi, yi){
	this->Value=henry;
	
	int w=this->GetWidth();
	int h=this->GetHeight();
	this->hotpoints.push_back( Hotpoint((int)xi, (int)yi+(int)h) );
	this->hotpoints.push_back( Hotpoint((int)xi+w, (int)yi+(int)h) );
}

