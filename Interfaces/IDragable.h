#pragma once

#include "Point.h"

class IDragable{
public:
	virtual void Drag(PointInt p, int *dx, int *dy)=0; 	//funzione virtuale per Drag 'n' Drop
};

