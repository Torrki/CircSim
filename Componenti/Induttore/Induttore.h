#pragma once

#include "Component.h"

class Inductor: public Component{
public:
	float Value;
	Inductor(int xi, int yi, float henry);
};

