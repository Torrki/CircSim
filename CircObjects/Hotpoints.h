#pragma once

class Connection;
class Hotpoint;

#include <cairo/cairo.h>
#include "Point.h"
#include "IDrawable.h"
#include "Component.h"

#define HOTPOINT_RADIUS 5.0
#define HOTPOINT_EDGE 18

class Hotpoint : public PointInt{
protected:
	int direzione;
	cairo_region_t* region;
public:
	Hotpoint(int x, int y, int dim_bound, int dir);
	~Hotpoint();
	int GetX(){return this->x;};
	int GetY(){return this->y;};
	cairo_region_t* GetRegion(){return this->region;};
	int GetDirection(){return this->direzione;};
	bool HotpointOn(int x, int y);
};

class HotpointDrawable: public Hotpoint , public IDrawable , public IDragable{
	std::list<Connection*> connections;
public:
	HotpointDrawable(int x, int y, int dirs);
	~HotpointDrawable(){this->~Hotpoint();};
	void Draw(cairo_t *cr);
	void Drag(int x, int y, int *dx, int *dy);
};

