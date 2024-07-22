#pragma once

#include <cairo/cairo.h>

class IDrawable{
public:
	virtual void Draw(cairo_t *cr)=0;	//funzione virtuale per disegnare
};

class IDragable{
public:
	virtual void Drag(int x, int y, int *dx, int *dy)=0; 	//funzione virtuale per Drag 'n' Drop
	virtual bool PointerOn(int x, int y){return false;};	//funzione virtuale per il cursore
};

