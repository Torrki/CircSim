#include "Line.h"

/*LINE IMPLEMENTATION*/

Line::Line(PointInt* start, PointInt* end){
	this->start=start;
	this->end=end;
	cairo_rectangle_int_t rect;
	
	if(start->x==end->x){
		rect={.x=start->x, .y=std::min(start->y, end->y), .width=LINE_STROKE, .height=std::abs(end->y-start->y)};
		if(end->y > start->y) this->dir=Direction::Down;
		else this->dir=Direction::Up;
	}else if(start->y==end->y){
		rect={.x=std::min(start->x, end->x), .y=start->y, .width=std::abs(end->x-start->x), .height=LINE_STROKE};
		if(end->x > start->x) this->dir=Direction::Right;
		else this->dir=Direction::Left;
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
		//this->start=reinterpret_cast<Hotpoint*>(l->GetStart());
		this->path.push_front(l);
	}else{
		//this->end=reinterpret_cast<Hotpoint*>(l->GetEnd());
		this->path.push_back(l);
	}
}

void Connection::EndConnection(Component* e, Hotpoint* hp){	
	this->endComp=e;
	this->end=hp;
	
	PointInt *middlePoint=new PointInt();
	PointInt *endPoint=dynamic_cast<PointInt*>(hp);
	PointInt *startPoint;
	if(!this->path.empty()){
		Line* lastLine=this->path.back();
		startPoint=lastLine->GetEnd();
		switch(lastLine->GetDirection()){
		case Direction::Up:
			middlePoint->x=startPoint->x;
			middlePoint->y=endPoint->y;
			break;
		case Direction::Down:
			middlePoint->x=startPoint->x;
			middlePoint->y=endPoint->y;
			break;
		case Direction::Left:
			middlePoint->x=endPoint->x;
			middlePoint->y=startPoint->y;
			break;
		case Direction::Right:
			middlePoint->x=endPoint->x;
			middlePoint->y=startPoint->y;
			break;
		}
		
	}else{
		startPoint=dynamic_cast<PointInt*>(this->start);
		middlePoint->y=startPoint->y;
		middlePoint->x=endPoint->x;
	}
	this->AddLine( new Line(startPoint, middlePoint), false );
	this->AddLine( new Line(middlePoint, endPoint), false );
}

void Connection::AppendPoint(PointInt *p){
	PointInt *middlePoint=new PointInt();
	PointInt *endPoint=p;
	PointInt *startPoint;
	if(!this->path.empty()){
		Line* lastLine=this->path.back();
		startPoint=lastLine->GetEnd();
		switch(lastLine->GetDirection()){
		case Direction::Up:
			middlePoint->x=startPoint->x;
			middlePoint->y=std::min( p->y, startPoint->y );
			endPoint->y=std::min( p->y, startPoint->y );
			break;
		case Direction::Down:
			middlePoint->x=startPoint->x;
			middlePoint->y=std::max( p->y, startPoint->y );
			endPoint->y=std::max( p->y, startPoint->y );
			break;
		case Direction::Left:
			middlePoint->x=std::min( p->x, startPoint->x );
			middlePoint->y=startPoint->y;
			endPoint->x=std::min( p->x, startPoint->x );
			break;
		case Direction::Right:
			middlePoint->x=std::max( p->x, startPoint->x );
			middlePoint->y=startPoint->y;
			endPoint->x=std::max( p->x, startPoint->x );
			break;
		}
		
	}else{
		startPoint=dynamic_cast<PointInt*>(this->start);
		middlePoint->x=p->x;
		middlePoint->y=startPoint->y;		
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

/*HOTPOINT_DRAWABLE IMPLEMENTATION*/

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

