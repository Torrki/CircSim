#include "Bipol.h"

Bipol::Bipol(Bipol_Type t, GtkDrawingArea* sheet, int x, int y){
	switch(t){
	case Bipol_Type::Resistor:
		this->symbolImg=cairo_image_surface_create_from_png("./resources/imgs/resistor.png");
		break;
	case Bipol_Type::Capacitor:
		this->symbolImg=cairo_image_surface_create_from_png("./resources/imgs/capacitor.png");
		break;
	case Bipol_Type::Inductor:
		this->symbolImg=cairo_image_surface_create_from_png("./resources/imgs/inductor.png");
		break;
	}
	int w=cairo_image_surface_get_width(this->symbolImg);
	int h=cairo_image_surface_get_height(this->symbolImg);
	
	this->type=t;
	this->position={.x=x, .y=y};
	cairo_rectangle_int_t rect={.x=x, .y=y, .width=w, .height=h};
	this->bound=cairo_region_create_rectangle(&rect);
}

Bipol_Type Bipol::GetType(){
	return this->type;
}

cairo_surface_t* Bipol::GetSymbol(){
	return this->symbolImg;
}

gboolean Bipol::PointerOn(int x, int y){
	return (gboolean)cairo_region_contains_point(this->bound, x, y);
}

void Bipol::SetPosition(int x, int y){
	cairo_region_translate(this->bound, x-this->position.x, y-this->position.y);
	this->position.x=x;
	this->position.y=y;
}

int Bipol::GetY(){
	return this->position.y;
}

int Bipol::GetX(){
	return this->position.x;
}

