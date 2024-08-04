#pragma once

#include <cairo/cairo.h>

class IDrawable{
public:
	virtual void Draw(cairo_t *cr)=0;	//funzione virtuale per disegnare
};

