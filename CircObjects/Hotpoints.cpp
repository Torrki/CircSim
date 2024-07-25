#include <cmath>
#include "Hotpoints.h"
#include <stdio.h>

/*HOTPOINT IMPLEMENTATION*/

Hotpoint::Hotpoint(int x, int y, int dim_bound, int dir){
	this->x=x;
	this->y=y;
	this->direzione=dir;
	this->Disponibile=true;
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

void Hotpoint::SetDirection(int dir){
	int eccesso=dir/16;
	if(eccesso > 0){
		unsigned int nBit=std::log2(eccesso);
		unsigned int nBitEffettivi = nBit % 4;
		this->direzione = (unsigned int)1 << nBitEffettivi;
	}else{
		this->direzione=dir;
	}
}

/*HOTPOINT_DRAWABLE IMPLEMENTATION*/

HotpointDrawable::HotpointDrawable(int x, int y, int dirs): Hotpoint(x, y, HOTPOINT_RADIUS*2, dirs){
	
}

void HotpointDrawable::Draw(cairo_t *cr){
	cairo_move_to(cr, this->x, this->y);
	cairo_arc(cr, this->x, this->y, HOTPOINT_RADIUS, 0.0, 2.0*M_PI);
}

void HotpointDrawable::Drag(int x, int y, int *dx, int *dy){
	int var_dx, var_dy;
	var_dx= x-this->x;
	var_dy= y-this->y;
	cairo_region_translate(this->region, var_dx, var_dy);
	this->x=x;
	this->y=y;
	
	if(dx) *dx=var_dx;
	if(dy) *dy=var_dy;
}

void HotpointDrawable::AddConnection(int dir, Connection* c){
	if(c){
		this->connections.push_back(c);
		this->direzione &= ~( (unsigned int)dir );
	}
}

