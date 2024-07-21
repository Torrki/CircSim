#pragma once
#include <gtk/gtk.h>
#include <list>
#include "IDrawable.h"

#define HOTPOINT_EDGE 18

class Hotpoint : public PointInt{
protected:
	cairo_region_t* region;
public:
	Hotpoint(int x, int y, int dim_bound);
	~Hotpoint();
	int GetX(){return this->x;};
	int GetY(){return this->y;};
	cairo_region_t* GetRegion(){return this->region;};
	bool HotpointOn(int x, int y);
};

class Component: public SurfaceDND{
protected:
	std::list<Hotpoint*> hotpoints;
public:
	Component(const char *path_symbol, int xi, int yi);
	~Component();
	Hotpoint* HotpointOver(int x, int y);
	void Drag(int x, int y);
};

class Resistor: public Component{
public:
	float Value;
	Resistor(int xi, int yi, float ohm);
};

class Capacitor: public Component{
public:
	float Value;
	Capacitor(int xi, int yi, float fahrad);
};

class Inductor: public Component{
public:
	float Value;
	Inductor(int xi, int yi, float henry);
};

