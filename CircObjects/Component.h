#pragma once

#include <list>
#include <utility>
#include "Hotpoints.h"
#include "DrawObjs.h"

class Connection;		//forward declaration
using namespace std;

class Component: public SurfaceDND{
protected:
	list<Hotpoint*> hotpoints;
	list<Connection*> connections;
public:
	Component(const char *path_symbol, PointInt pi);
	~Component();
	Hotpoint* HotpointOver(PointInt p);
	void Drag(PointInt p);
	void AddConnection(Connection* c);
	void Rotate(double ang);
};

class ConnectionBuilder{
	Connection* c;
public:
	void NewConnection(Component* sc, Hotpoint *sp);
	void ClearConnection();
	void AddLine(Line* l, bool front);
	Line* PopLine(bool front);
	void AppendPoint(PointInt *p);
	void EndConnection(Component* e, Hotpoint* hp);
	Connection* SplitConnection(Hotpoint* p, Line* l);
	
	void SetConnection(Connection* c){this->c=c;};
	Connection* GetConnection() const {return this->c;};
};

class Connection: public IDrawable{
	list<Line*> path;
	pair<Component*, Hotpoint*> start, end;
public:
	float Corrente;
	Connection(Component* sc, Hotpoint *sp);
	~Connection();
	pair<Component*, Hotpoint*> GetStart() const {return this->start;};
	pair<Component*, Hotpoint*> GetEnd() const {return this->end;};
	const list<Line*> GetPath() const {return this->path;};
	void Draw(cairo_t* cr);
	Line* LineOn(PointInt p);
	
	friend void 	ConnectionBuilder::NewConnection(Component* sc, Hotpoint *sp);
	friend void 	ConnectionBuilder::AddLine(Line* l, bool front);
	friend Line* 	ConnectionBuilder::PopLine(bool front);
	friend void 	ConnectionBuilder::AppendPoint(PointInt *p);
	friend void 	ConnectionBuilder::EndConnection(Component* e, Hotpoint* hp);
	friend Connection* ConnectionBuilder::SplitConnection(Hotpoint* p, Line* l);
	
};

