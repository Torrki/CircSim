#include "Component.h"
#include <stdio.h>

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
	this->hotpoints.push_back( new Hotpoint(xi, yi+(int)((float)h/2.0f), HOTPOINT_EDGE, DIR_LEFT) );
	this->hotpoints.push_back( new Hotpoint(xi+w, yi+(int)((float)h/2.0f), HOTPOINT_EDGE, DIR_RIGHT) );
}

Capacitor::Capacitor(int xi, int yi, float fahrad):Component("./resources/imgs/capacitor.png", xi, yi){
	this->Value=fahrad;
	
	int w=this->GetWidth();
	int h=this->GetHeight();
	this->hotpoints.push_back( new Hotpoint(xi, yi+(int)((float)h/2.0f), HOTPOINT_EDGE, DIR_LEFT) );
	this->hotpoints.push_back( new Hotpoint(xi+w, yi+(int)((float)h/2.0f), HOTPOINT_EDGE, DIR_RIGHT) );
}

Inductor::Inductor(int xi, int yi, float henry):Component("./resources/imgs/inductor.png", xi, yi){
	this->Value=henry;
	
	int w=this->GetWidth();
	int h=this->GetHeight();
	this->hotpoints.push_back( new Hotpoint(xi, yi+h, HOTPOINT_EDGE, DIR_LEFT) );
	this->hotpoints.push_back( new Hotpoint(xi+w, yi+h, HOTPOINT_EDGE, DIR_RIGHT) );
}

/*CONNECTION IMPLEMENTATION*/

void Connection::Draw(cairo_t *cr){
	for(std::list<Line*>::iterator it=this->path.begin(); it != this->path.end(); it++){
		(*it)->Draw(cr);
	}
}

Line* Connection::PointerOn(int x, int y){
	for(std::list<Line*>::iterator it=this->path.begin(); it != this->path.end(); it++){
		if((*it)->PointerOn(x, y)) return *it;
	}
	return NULL;
}

void Connection::AddLine(Line* l, bool front){
	if(front){
		this->path.push_front(l);
	}else{
		this->path.push_back(l);
	}
}

void Connection::EndConnection(Component* e, Hotpoint* hp){	
	this->endComp=e;
	this->end=hp;
	
	PointInt *middlePoint=new PointInt();
	PointInt *endPoint=dynamic_cast<PointInt*>(hp);
	PointInt *startPoint;
	int direzione;
	if(!this->path.empty()){
		Line* lastLine=this->path.back();
		startPoint=lastLine->GetEnd();
		direzione=lastLine->GetDirection();
	}else{
		startPoint=dynamic_cast<PointInt*>(this->start);
		direzione=this->start->GetDirection();
	}
	switch(direzione){
	case DIR_UP :
		middlePoint->x=startPoint->x;
		middlePoint->y=endPoint->y;
		break;
	case DIR_DOWN:
		middlePoint->x=startPoint->x;
		middlePoint->y=endPoint->y;
		break;
	case DIR_LEFT:
		middlePoint->x=endPoint->x;
		middlePoint->y=startPoint->y;
		break;
	case DIR_RIGHT:
		middlePoint->x=endPoint->x;
		middlePoint->y=startPoint->y;
		break;
	case DIR_UP | DIR_DOWN:
		middlePoint->x=startPoint->x;
		middlePoint->y=endPoint->y;
		break;
	case DIR_LEFT | DIR_RIGHT:
		middlePoint->x=endPoint->x;
		middlePoint->y=startPoint->y;
		break;
	}
	this->AddLine( new Line(startPoint, middlePoint), false );
	this->AddLine( new Line(middlePoint, endPoint), false );
}

void Connection::AppendPoint(PointInt *p){
	PointInt *middlePoint=new PointInt();
	PointInt *endPoint=p;
	PointInt *startPoint;
	int direzione;
	if(!this->path.empty()){
		Line* lastLine=this->path.back();
		startPoint=lastLine->GetEnd();
		direzione=lastLine->GetDirection();
	}else{
		startPoint=dynamic_cast<PointInt*>(this->start);
		direzione=this->start->GetDirection();
	}
	switch(direzione){
	case DIR_UP:
		middlePoint->x=startPoint->x;
		middlePoint->y=std::min( p->y, startPoint->y );
		endPoint->y=std::min( p->y, startPoint->y );
		break;
	case DIR_DOWN:
		middlePoint->x=startPoint->x;
		middlePoint->y=std::max( p->y, startPoint->y );
		endPoint->y=std::max( p->y, startPoint->y );
		break;
	case DIR_LEFT:
		middlePoint->x=std::min( p->x, startPoint->x );
		middlePoint->y=startPoint->y;
		endPoint->x=std::min( p->x, startPoint->x );
		break;
	case DIR_RIGHT:
		middlePoint->x=std::max( p->x, startPoint->x );
		middlePoint->y=startPoint->y;
		endPoint->x=std::max( p->x, startPoint->x );
		break;
	case DIR_UP | DIR_DOWN:
		middlePoint->x=startPoint->x;
		middlePoint->y=p->y;
		break;
	case DIR_LEFT | DIR_RIGHT:
		middlePoint->y=startPoint->y;
		middlePoint->x=p->x;
		break;
		
	}
	
	this->AddLine( new Line(startPoint, middlePoint), false );
	this->AddLine( new Line(middlePoint, endPoint), false );
}

Connection::~Connection(){
	while(!this->path.empty()){
		Line *l=this->path.back();
		if(l->GetEnd() != this->end)
			delete l->GetEnd();
			
		this->path.pop_back();
		delete l;
	}
}

