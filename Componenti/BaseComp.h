#include "Component.h"

class Battery: public Component{
public:
	float Ri;
	float Value;
	Battery(PointInt pi, float ddp, float ri);
};

class Capacitor: public Component{
public:
	float Value;
	Capacitor(PointInt pi, float fahrad);
};

class CurrentSource: public Component{
public:
	float Ri;
	float Value;
	CurrentSource(PointInt pi, float i, float ri);
};

class GND: public Component{
public:
	GND(PointInt pi);
};

class Inductor: public Component{
public:
	float Value;
	Inductor(PointInt pi, float henry);
};

class Resistor: public Component{
public:
	float Value;
	Resistor(PointInt pi, float ohm);
};

