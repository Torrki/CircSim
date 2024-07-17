#pragma once

#include <cairo/cairo.h>
#include <list>
#include "Point.h"
#include "Component.h"

#define LINE_STROKE 3

enum class Direction{Up, Down, Left, Right};

class Line: public IDrawable{
	cairo_region_t *region;
	PointInt *start, *end;
	Direction dir;
public:
	Line(PointInt* start, PointInt* end);
	Direction GetDirection(){return this->dir;};
	PointInt* GetStart(){return this->start;};
	PointInt* GetEnd(){return this->end;};
	void Draw(cairo_t *cr);
};

class Connection: public IDrawable{
	std::list<Line*> path;
	Component *start, *end;
public:
	void Draw(cairo_t *cr);
	void AppendLine(Line* nl){this->path.push_back(nl);};
	std::list<Line*> GetPath(){return this->path;};
	bool Empty(){return this->path.empty();};
};

