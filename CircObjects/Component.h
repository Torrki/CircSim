#pragma once

#include <list>
#include <unordered_map>
#include "IDrawable.h"
#include "Hotpoints.h"
#include "DrawObjs.h"

class Connection;		//forward declaration

class Component: public SurfaceDND{
protected:
	std::list<Hotpoint*> hotpoints;
public:
	Component(const char *path_symbol, int xi, int yi);
	~Component();
	Hotpoint* HotpointOver(int x, int y);
	void Drag(int x, int y);
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
	Component* GetCompStart(){return this->startComp;};
	Component* GetCompEnd(){return this->endComp;};
	void Draw(cairo_t* cr);
	void Drag(double x, double y, int *dx, int *dy){};
	Line* PointerOn(int x, int y);
	
	void AddLine(Line* l, bool front);
	void AppendPoint(PointInt *p);
	void EndConnection(Component* e, Hotpoint* hp);
	
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

