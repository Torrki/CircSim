#pragma once

#include "Component.h"

class Resistor: public Component{
public:
	float Value;
	Resistor(int xi, int yi, float ohm);
};

