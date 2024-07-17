#include "Line.h"

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

void Connection::Draw(cairo_t *cr){
	for(std::list<Line*>::iterator it=this->path.begin(); it != this->path.end(); it++){
		(*it)->Draw(cr);
	}
}

