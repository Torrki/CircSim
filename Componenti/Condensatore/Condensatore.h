#pragma once

#include "Component.h"

class Capacitor: public Component{
public:
	float Value;
	Capacitor(int xi, int yi, float fahrad);
};

