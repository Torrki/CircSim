#pragma once
#include <gtk/gtk.h>

typedef struct {
	int x,y;
} Point;

enum class Bipol_Type { Resistor, Capacitor, Inductor };

class Bipol{
	cairo_surface_t* symbolImg;
	cairo_region_t* bound;
	Bipol_Type type;
	Point position;
public:
	Bipol(Bipol_Type t, GtkDrawingArea* sheet, int x, int y);
	Bipol_Type GetType();
	cairo_surface_t* GetSymbol();
	gboolean PointerOn(int x, int y);
	int GetX();
	int GetY();
	void SetPosition(int x, int y);
};

