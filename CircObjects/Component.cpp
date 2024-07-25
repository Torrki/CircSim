#include "Component.h"
#include <stdio.h>
#include <complex>

/*COMPONENT IMPLEMENTATION*/

Component::Component(const char *path_symbol, int xi, int yi): SurfaceDND(xi, yi, path_symbol){
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

void Component::AddConnection(Connection* c){
	if(c){
		this->connections.push_back(c);
	}
}

void Component::Rotate(double ang){
	SurfaceDND::Rotate(ang);
	if(ang > 0.0){
		double nGiri=std::trunc(ang/(2.0*M_PI));
		double rotazioneEffettiva=ang - nGiri*2.0*M_PI;
		
		//Spostamento Hotpoint
		for(std::list<Hotpoint*>::iterator it=this->hotpoints.begin(); it != this->hotpoints.end(); it++){
			int Xr=(*it)->x;
			int Yr=(*it)->y;
			int x_1=Xr-this->drawPoint->x;
			int y_1=Yr-this->drawPoint->y;
			
			std::complex<double> c(x_1, y_1);	//tramite i complessi lavoro sulle fasi per semplificare codice e calcoli
			std::complex<double> res=c*std::polar(1.0, ang);
			
			std::complex<double> diff=res-c;
			
			int dx=(int)diff.real();
			int dy=(int)diff.imag();
			
			cairo_region_translate((*it)->GetRegion(), dx, dy);
			(*it)->x=(int)res.real()+this->drawPoint->x;
			(*it)->y=(int)res.imag()+this->drawPoint->y;
			
			//Aggiornamento direzioni
			unsigned int dir=(*it)->GetDirection();
			unsigned int spostamento=0;
			if(rotazioneEffettiva==M_PI/2.0){
				spostamento=1;
			}else if(rotazioneEffettiva==M_PI){
				spostamento=2;
			}else if(rotazioneEffettiva==M_PI*(3.0/2.0)){
				spostamento=3;
			}
			(*it)->SetDirection(dir << spostamento);
		}
		
	}
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

Line* Connection::PopLine(bool front){
	Line* l;
	if(front){
		l=this->path.front();
		this->path.pop_front();
	}else{
		l=this->path.back();
		this->path.pop_back();
	}
	return l;
}

void Connection::EndConnection(Component* e, Hotpoint* hp){	
	this->endComp=e;
	this->end=hp;
	
	PointInt *middlePoint=new PointInt();
	PointInt *endPoint=static_cast<PointInt*>(hp);
	PointInt *startPoint;
	int direzione;
	if(!this->path.empty()){
		Line* lastLine=this->path.back();
		startPoint=lastLine->GetEnd();
		direzione=lastLine->GetDirection();
	}else{
		startPoint=static_cast<PointInt*>(this->start);
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
		startPoint=static_cast<PointInt*>(this->start);
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

