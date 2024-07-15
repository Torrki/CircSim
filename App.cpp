#include <list>
#include "Bipol.h"
#include "App.h"

struct {
	std::list<Bipol> bipols;
	GtkDrawingArea *dwa;
	gboolean DragBipol;
	Bipol *lastSelected=NULL;
}AppCtx;

void DrawSheet(GtkDrawingArea* self, cairo_t *cr, int width, int height, gpointer user_data);
void RButton_clicked(GtkButton* self, gpointer user_data);
void CButton_clicked(GtkButton* self, gpointer user_data);
void LButton_clicked(GtkButton* self, gpointer user_data);
void DwaMotion(GtkEventControllerMotion* self, gdouble x, gdouble y, gpointer user_data);
void DwaClick(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, gpointer user_data);
void DwaRelease(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, gpointer user_data);

void InitApp(GtkApplication *self, gpointer user_data){
	GtkBuilder *builder=gtk_builder_new_from_file("./resources/UI/MainWin.ui");
	GtkWindow *Root=GTK_WINDOW(gtk_builder_get_object(builder, "RootWindow"));
	AppCtx.dwa=GTK_DRAWING_AREA(gtk_builder_get_object(builder, "DrawSheet"));
	GtkButton *RButton=GTK_BUTTON(gtk_builder_get_object(builder, "ResistorButton"));
	GtkButton *CButton=GTK_BUTTON(gtk_builder_get_object(builder, "CapacitorButton"));
	GtkButton *LButton=GTK_BUTTON(gtk_builder_get_object(builder, "InductorButton"));
	
	g_signal_connect(G_OBJECT(RButton), "clicked", G_CALLBACK(RButton_clicked), NULL);
	g_signal_connect(G_OBJECT(CButton), "clicked", G_CALLBACK(CButton_clicked), NULL);
	g_signal_connect(G_OBJECT(LButton), "clicked", G_CALLBACK(LButton_clicked), NULL);
	
	GtkEventController* motionDwaController=gtk_event_controller_motion_new();
	GtkGesture* clickDwaController=gtk_gesture_click_new();
	g_signal_connect(G_OBJECT(motionDwaController), "motion", G_CALLBACK(DwaMotion), NULL);
	g_signal_connect(G_OBJECT(clickDwaController), "pressed", G_CALLBACK(DwaClick), NULL);
	g_signal_connect(G_OBJECT(clickDwaController), "released", G_CALLBACK(DwaRelease), NULL);
	gtk_widget_add_controller(GTK_WIDGET(AppCtx.dwa), motionDwaController);
	gtk_widget_add_controller(GTK_WIDGET(AppCtx.dwa), GTK_EVENT_CONTROLLER(clickDwaController));
	
	gtk_drawing_area_set_draw_func(AppCtx.dwa, &DrawSheet, NULL, NULL);
	gtk_application_add_window(self,Root);
	gtk_window_present(Root);
}

void DrawSheet(GtkDrawingArea* self, cairo_t *cr, int width, int height, gpointer user_data){
	for(std::list<Bipol>::iterator it=AppCtx.bipols.begin(); it != AppCtx.bipols.end(); it++){
		cairo_set_source_surface(cr, it->GetSymbol(), it->GetX(), it->GetY());
		cairo_paint(cr);
	}
	//cairo_save(cr);
}

void RButton_clicked(GtkButton* self, gpointer user_data){
	/*Aggiunge un bipolo resistore alla lista dello schema*/
	AppCtx.bipols.push_back(Bipol(Bipol_Type::Resistor,AppCtx.dwa, 60, 60));
	gtk_widget_queue_draw(GTK_WIDGET(AppCtx.dwa));
}

void CButton_clicked(GtkButton* self, gpointer user_data){
	/*Aggiunge un bipolo condensatore alla lista dello schema*/
	AppCtx.bipols.push_back(Bipol(Bipol_Type::Capacitor,AppCtx.dwa, 60, 60));
	gtk_widget_queue_draw(GTK_WIDGET(AppCtx.dwa));
}

void LButton_clicked(GtkButton* self, gpointer user_data){
	/*Aggiunge un bipolo induttore alla lista dello schema*/
	AppCtx.bipols.push_back(Bipol(Bipol_Type::Inductor,AppCtx.dwa, 60, 60));
	gtk_widget_queue_draw(GTK_WIDGET(AppCtx.dwa));
}

void DwaMotion(GtkEventControllerMotion* self, gdouble x, gdouble y, gpointer user_data){
	if(AppCtx.DragBipol){
		int x_int=(int)x, y_int=(int)y;
		AppCtx.lastSelected->SetPosition(x_int, y_int);
		gtk_widget_queue_draw(GTK_WIDGET(AppCtx.dwa));
		//g_print("X: %d\nY: %d\n", (int)x_int, (int)y_int);
	}
}

void DwaClick(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, gpointer user_data){
	for(std::list<Bipol>::iterator it=AppCtx.bipols.begin(); it != AppCtx.bipols.end(); it++){
		if(it->PointerOn((int)x, (int)y)){
			AppCtx.DragBipol=TRUE;
			AppCtx.lastSelected=&(*it);
			break;
		}
	}
}

void DwaRelease(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, gpointer user_data){
	AppCtx.DragBipol=FALSE;
	//g_print("Release\n");
}

