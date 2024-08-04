#include "Component.h"
#include <complex>
#include <stdio.h>

/*COMPONENT IMPLEMENTATION*/

Component::Component(const char *path_symbol, PointInt pi): SurfaceDND(pi, path_symbol){
	this->regionBound=cairo_region_reference(this->regionBound);
	this->surface=cairo_surface_reference(this->surface);
}

Hotpoint* Component::HotpointOver(PointInt p){
	for(list<Hotpoint*>::iterator it=this->hotpoints.begin(); it != this->hotpoints.end(); it++){
		if((*it)->PointerOn(p)) return *it;
	}
	return NULL;
}

void Component::Drag(PointInt p){
	int dx, dy;
	SurfaceDND::Drag(p, &dx, &dy);
	for(list<Hotpoint*>::iterator it=this->hotpoints.begin(); it != this->hotpoints.end(); it++){
		cairo_region_translate((*it)->GetRegion(), dx, dy);
		(*it)->x += dx;
		(*it)->y += dy;
	}
}

Component::~Component(){
	for(list<Hotpoint*>::iterator it=this->hotpoints.begin(); it != this->hotpoints.end(); it++){
		this->hotpoints.erase(it);
		delete *it;
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
	if(ang != 0.0){
		double nGiri=std::trunc(ang/(2.0*M_PI));
		double rotazioneEffettiva=ang - nGiri*2.0*M_PI;
		
		//Spostamento Hotpoint
		for(list<Hotpoint*>::iterator it=this->hotpoints.begin(); it != this->hotpoints.end(); it++){
			int Xr=(*it)->x;
			int Yr=(*it)->y;
			int x_1=Xr-this->drawPoint->x;
			int y_1=Yr-this->drawPoint->y;
			
			complex<double> c((double)x_1, (double)y_1);	//tramite i complessi lavoro sulle fasi per semplificare codice e calcoli
			complex<double> res=c*std::polar(1.0, ang);
			complex<double> diff=res-c;
			int dx=(int)diff.real();
			int dy=(int)diff.imag();
			
			cairo_region_translate((*it)->GetRegion(), dx, dy);
			(*it)->x += dx;
			(*it)->y += dy;
			
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
			cairo_region_union(this->regionBound, (*it)->GetRegion() );
		}
		
	}
}

/*CONNECTION IMPLEMENTATION*/

void Connection::Draw(cairo_t *cr){
	for(list<Line*>::iterator it=this->path.begin(); it != this->path.end(); it++){
		(*it)->Draw(cr);
	}
}

Line* Connection::LineOn(PointInt p){
	for(list<Line*>::iterator it=this->path.begin(); it != this->path.end(); it++){
		if((*it)->PointerOn(p)) return *it;
	}
	return NULL;
}

Connection::Connection(Component* sc, Hotpoint *sp){
	this->start.first=sc;
	this->start.second=sp;
	this->end.first=NULL;
	this->end.second=NULL;
}

Connection::~Connection(){
	while(!this->path.empty()){
		Line *l=this->path.back();
		if(l->GetEnd() != this->end.second)
			delete l->GetEnd();
			
		this->path.pop_back();
		delete l;
	}
}

/*BUILDER CONNECTION IMPLEMENTATION*/

void ConnectionBuilder::AddLine(Line* l, bool front){
	if(front){
		this->c->path.push_front(l);
	}else{
		this->c->path.push_back(l);
	}
}

Line* ConnectionBuilder::PopLine(bool front){
	Line* l;
	if(front){
		l=this->c->path.front();
		this->c->path.pop_front();
	}else{
		l=this->c->path.back();
		this->c->path.pop_back();
	}
	return l;
}

void ConnectionBuilder::EndConnection(Component* e, Hotpoint* hp){	
	this->c->end.first=e;
	this->c->end.second=hp;
	
	PointInt *middlePoint=new PointInt();
	PointInt *endPoint=static_cast<PointInt*>(hp);
	PointInt *startPoint;
	int direzione;
	if(!this->c->path.empty()){
		Line* lastLine=this->c->path.back();
		startPoint=lastLine->GetEnd();
		direzione=lastLine->GetDirection();
	}else{
		startPoint=static_cast<PointInt*>(this->c->start.second);
		direzione=this->c->start.second->GetDirection();
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

void ConnectionBuilder::AppendPoint(PointInt *p){
	PointInt *middlePoint=new PointInt();
	PointInt *endPoint=p;
	PointInt *startPoint;
	int direzione;
	if(!this->c->path.empty()){
		Line* lastLine=this->c->path.back();
		startPoint=lastLine->GetEnd();
		direzione=lastLine->GetDirection();
	}else{
		startPoint=static_cast<PointInt*>(this->c->start.second);
		direzione=this->c->start.second->GetDirection();
	}
	switch(direzione){
	case DIR_UP:
		middlePoint->x=startPoint->x;
		middlePoint->y=min( p->y, startPoint->y );
		endPoint->y=min( p->y, startPoint->y );
		break;
	case DIR_DOWN:
		middlePoint->x=startPoint->x;
		middlePoint->y=max( p->y, startPoint->y );
		endPoint->y=max( p->y, startPoint->y );
		break;
	case DIR_LEFT:
		middlePoint->x=min( p->x, startPoint->x );
		middlePoint->y=startPoint->y;
		endPoint->x=min( p->x, startPoint->x );
		break;
	case DIR_RIGHT:
		middlePoint->x=max( p->x, startPoint->x );
		middlePoint->y=startPoint->y;
		endPoint->x=max( p->x, startPoint->x );
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

void ConnectionBuilder::ClearConnection(){
	if(this->c){
		delete this->c;
		this->c=NULL;
	}
}

void ConnectionBuilder::NewConnection(Component* sc, Hotpoint *sp){
	if(this->c && ! this->c->end.second){
		delete this->c;
	}
	this->c = new Connection(sc, sp);
}

void ConnectionBuilder::SetEnd(Component* ec, Hotpoint *ep){
	this->c->end.first=ec;
	this->c->end.second=ep;	
}

void ConnectionBuilder::SetStart(Component* sc, Hotpoint *sp){
	this->c->start.first=sc;
	this->c->start.second=sp;	
}

