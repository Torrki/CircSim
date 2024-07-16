#include "Component.h"

Component::Component(const char *path_symbol, double xi, double yi){
	this->surface=cairo_image_surface_create_from_png(path_symbol);
	this->drawPoint={.x=xi, .y=yi};

	int w=cairo_image_surface_get_width(this->surface);
	int h=cairo_image_surface_get_height(this->surface);
	cairo_rectangle_int_t rect={.x=(int)xi, .y=(int)yi, .width=w, .height=h};	
	this->regionBound=cairo_region_create_rectangle(&rect);
}

Resistor::Resistor(double xi, double yi, float ohm):Component("./resources/imgs/resistor.png", xi, yi){
	this->Value=ohm;
}

Capacitor::Capacitor(double xi, double yi, float fahrad):Component("./resources/imgs/capacitor.png", xi, yi){
	this->Value=fahrad;
}

Inductor::Inductor(double xi, double yi, float henry):Component("./resources/imgs/inductor.png", xi, yi){
	this->Value=henry;
}

