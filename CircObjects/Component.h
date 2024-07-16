#pragma once
#include <gtk/gtk.h>
#include "IDrawable.h"

class Component: public SurfaceDND{
public:
	Component(const char *path_symbol, double xi, double yi);
};

class Resistor: public Component{
public:
	float Value;
	Resistor(double xi, double yi, float ohm);
};

class Capacitor: public Component{
public:
	float Value;
	Capacitor(double xi, double yi, float fahrad);
};

class Inductor: public Component{
public:
	float Value;
	Inductor(double xi, double yi, float henry);
};

