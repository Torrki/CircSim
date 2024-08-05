#pragma once

class Connection;
class Hotpoint;

#include "Point.h"
#include "IDrawable.h"
#include "IDragable.h"
#include "ISelectable.h"
#include "Component.h"

#define HOTPOINT_RADIUS 5.0
#define HOTPOINT_EDGE 10

using namespace std;

class Hotpoint : public PointInt, public ISelectable{
protected:
	int direzione;
	cairo_region_t* region;
public:
	float Poternziale;
	bool Disponibile;
	
	Hotpoint(PointInt p, int dim_bound, int dir);
	~Hotpoint();
	cairo_region_t* GetRegion() const {return this->region;};
	int GetDirection(){return this->direzione;};
	void SetDirection(int dir);
	bool PointerOn(PointInt p);
};

class HotpointDrawable: public Hotpoint , public IDrawable , public IDragable{
	list<Connection*> connections;
public:
	HotpointDrawable(PointInt p, int dirs): Hotpoint(p, HOTPOINT_RADIUS*2, dirs){};
	~HotpointDrawable(){this->~Hotpoint();};
	void Draw(cairo_t *cr);
	void Drag(PointInt p, int *dx, int *dy);
	void AddConnection(int dir, Connection* c);
};

