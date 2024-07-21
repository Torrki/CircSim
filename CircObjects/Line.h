#pragma once

#include <cairo/cairo.h>
#include <list>
#include "Point.h"
#include "Component.h"

#define LINE_STROKE 3
#define HOTPOINT_RADIUS 5.0

enum class Direction{Up, Down, Left, Right};

class Line: public IDrawable, public IDragable{
	cairo_region_t *region;
	PointInt *start, *end;
	Direction dir;
public:
	Line(PointInt* start, PointInt* end);
	~Line();
	Direction GetDirection(){return this->dir;};
	PointInt* GetStart(){return this->start;};
	PointInt* GetEnd(){return this->end;};
	void Drag(int x, int y, int *dx, int *dy){};
	bool PointerOn(int x, int y){ return (bool)cairo_region_contains_point(this->region, x, y); };
	void Draw(cairo_t *cr);
};

class HotpointDrawable: public Hotpoint , public IDrawable , public IDragable{
public:
	HotpointDrawable(int x, int y): Hotpoint(x, y, HOTPOINT_RADIUS*2){};
	~HotpointDrawable(){this->~Hotpoint();};
	void Draw(cairo_t *cr);
	void Drag(int x, int y, int *dx, int *dy);
};

class Connection: public IDrawable{
	std::list<Line*> path;
	Hotpoint *start, *end;
	Component *startComp, *endComp;
public:
	Connection(Component* sc, Hotpoint *sp){this->start=sp; this->startComp=sc;};
	~Connection();
	Hotpoint* GetStart(){return this->start;};
	Hotpoint* GetEnd(){return this->end;};
	std::list<Line*>* GetPath(){return &(this->path);};
	void Draw(cairo_t* cr);
	void Drag(double x, double y, int *dx, int *dy){};
	Line* PointerOn(int x, int y);
	
	void AddLine(Line* l, bool front);
	void AppendPoint(PointInt *p);
	void EndConnection(Component* e, Hotpoint* hp);
};

