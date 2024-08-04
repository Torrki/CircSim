#pragma once

#include "Point.h"

class ISelectable{
public:
	virtual bool PointerOn(PointInt p)=0;		//funzione virtuale per il puntatore sull'oggetto
};

