#include <list>
#include "Component.h"
#include "Line.h"
#include "App.h"

struct {
	std::list<Component*> Componenti;
	std::list<Connection*> Connessioni;
	std::list<HotpointDrawable*> HotpointLinee;
	GtkDrawingArea *Dwa;
	PointInt* StartLine;
	PointInt* CurrentPosition;	
	Component *LastSelected=NULL;
	Connection *buildingConnection;
	HotpointDrawable *drawingHotpoint;
	cairo_t *contestoDisegno;
	gboolean DragComponent;
	gboolean DrawLine;
	gboolean DrawHotpoint;
}AppCtx;

void DrawSheet(GtkDrawingArea* self, cairo_t *cr, int width, int height, gpointer user_data);
void RButton_clicked(GtkButton* self, gpointer user_data);
void CButton_clicked(GtkButton* self, gpointer user_data);
void LButton_clicked(GtkButton* self, gpointer user_data);
void HPButton_clicked(GtkButton* self, gpointer user_data);
void DwaMotion(GtkEventControllerMotion* self, gdouble x, gdouble y, gpointer user_data);
void DwaClick(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, gpointer user_data);
void DwaRelease(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, gpointer user_data);
gboolean DwaKeyPressed(GtkEventControllerKey *self, guint keyval, guint keycode, GdkModifierType state, gpointer user_data);
void LinePreview(cairo_t *cr);

void InitApp(GtkApplication *self, gpointer user_data){
	AppCtx.CurrentPosition=new PointInt();
	
	GtkBuilder *builder=gtk_builder_new_from_file("./resources/UI/MainWin.ui");
	GtkWindow *Root=GTK_WINDOW(gtk_builder_get_object(builder, "RootWindow"));
	AppCtx.Dwa=GTK_DRAWING_AREA(gtk_builder_get_object(builder, "DrawSheet"));
	GtkButton *RButton=GTK_BUTTON(gtk_builder_get_object(builder, "ResistorButton"));
	GtkButton *CButton=GTK_BUTTON(gtk_builder_get_object(builder, "CapacitorButton"));
	GtkButton *LButton=GTK_BUTTON(gtk_builder_get_object(builder, "InductorButton"));
	GtkButton *HPButton=GTK_BUTTON(gtk_builder_get_object(builder, "HotpointButton"));
	
	g_signal_connect(G_OBJECT(RButton), "clicked", G_CALLBACK(RButton_clicked), NULL);
	g_signal_connect(G_OBJECT(CButton), "clicked", G_CALLBACK(CButton_clicked), NULL);
	g_signal_connect(G_OBJECT(LButton), "clicked", G_CALLBACK(LButton_clicked), NULL);
	g_signal_connect(G_OBJECT(HPButton), "clicked", G_CALLBACK(HPButton_clicked), NULL);
	
	GtkEventController* motionDwaController=gtk_event_controller_motion_new();
	GtkGesture* clickDwaController=gtk_gesture_click_new();
	GtkEventController* keyDwaController=gtk_event_controller_key_new();
	g_signal_connect(G_OBJECT(motionDwaController), "motion", G_CALLBACK(DwaMotion), NULL);
	g_signal_connect(G_OBJECT(clickDwaController), "pressed", G_CALLBACK(DwaClick), NULL);
	g_signal_connect(G_OBJECT(clickDwaController), "released", G_CALLBACK(DwaRelease), NULL);
	g_signal_connect(G_OBJECT(keyDwaController), "key-pressed", G_CALLBACK(DwaKeyPressed), NULL);
	gtk_widget_add_controller(GTK_WIDGET(AppCtx.Dwa), motionDwaController);
	gtk_widget_add_controller(GTK_WIDGET(Root), keyDwaController);
	gtk_widget_add_controller(GTK_WIDGET(AppCtx.Dwa), GTK_EVENT_CONTROLLER(clickDwaController));
	
	gtk_drawing_area_set_draw_func(AppCtx.Dwa, &DrawSheet, NULL, NULL);
	gtk_application_add_window(self,Root);
	gtk_window_present(Root);
}

void DrawSheet(GtkDrawingArea* self, cairo_t *cr, int width, int height, gpointer user_data){
	//Stampa delle connessioni	
	cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);
	for(std::list<Connection*>::iterator it=AppCtx.Connessioni.begin(); it != AppCtx.Connessioni.end(); it++){
		(*it)->Draw(cr);
	}
	//Stampa della linea che sto creando
	if(AppCtx.DrawLine)
		LinePreview(cr);
	cairo_stroke(cr);
	
	//Stampa degli hotpoint sulle linee
	cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);
	for(std::list<HotpointDrawable*>::iterator it=AppCtx.HotpointLinee.begin(); it != AppCtx.HotpointLinee.end(); it++){
		(*it)->Draw(cr);
	}
	
	if(AppCtx.DrawHotpoint){
		AppCtx.drawingHotpoint->Draw(cr);
	}
	cairo_fill(cr);
	
	//Stampa dei componenti	
	for(std::list<Component*>::iterator it=AppCtx.Componenti.begin(); it != AppCtx.Componenti.end(); it++){
		(*it)->Draw(cr);
	}
	//cairo_save(cr);
}

void RButton_clicked(GtkButton* self, gpointer user_data){
	/*Aggiunge un bipolo resistore alla lista dello schema*/
	AppCtx.Componenti.push_back(new Resistor(60.0, 60.0, 100.0));
	gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
}

void CButton_clicked(GtkButton* self, gpointer user_data){
	/*Aggiunge un bipolo condensatore alla lista dello schema*/
	AppCtx.Componenti.push_back(new Capacitor(60.0, 60.0, 1.0));
	gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
}

void LButton_clicked(GtkButton* self, gpointer user_data){
	/*Aggiunge un bipolo induttore alla lista dello schema*/
	AppCtx.Componenti.push_back(new Inductor(60.0, 60.0, 40.0));
	gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
}

void HPButton_clicked(GtkButton* self, gpointer user_data){
	/*Aggiunge un hotpoint alle linee dello schema*/
	AppCtx.DrawHotpoint=TRUE;
	AppCtx.drawingHotpoint=new HotpointDrawable(AppCtx.CurrentPosition->x, AppCtx.CurrentPosition->y);
}

void DwaMotion(GtkEventControllerMotion* self, gdouble x, gdouble y, gpointer user_data){
	AppCtx.CurrentPosition->x=(int)x;
	AppCtx.CurrentPosition->y=(int)y;
	if(AppCtx.DragComponent){
		AppCtx.LastSelected->Drag(x,y);
		gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
	}else if(AppCtx.DrawLine){
		gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
	}else if(AppCtx.DrawHotpoint){
		int a, b;
		AppCtx.drawingHotpoint->Drag(x,y, &a, &b);
		gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
	}
}

void DwaClick(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, gpointer user_data){
	for(std::list<Component*>::iterator it=AppCtx.Componenti.begin(); !AppCtx.DrawHotpoint && it != AppCtx.Componenti.end(); it++){
		if((*it)->PointerOn(x, y)){
			AppCtx.LastSelected=*it;
			Hotpoint *hp=(*it)->HotpointOver(x, y);
			if(hp){
				AppCtx.DrawLine = !AppCtx.DrawLine;
				if(AppCtx.DrawLine){
					AppCtx.StartLine=dynamic_cast<PointInt*>(hp);
					AppCtx.buildingConnection=new Connection(*it, hp);
				}else{
					//Aggiunta della nuova connessione alla lista
					AppCtx.buildingConnection->EndConnection(*it, hp);					
					AppCtx.Connessioni.push_back(AppCtx.buildingConnection);
				}
				gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
				return;
			}
			else{
				AppCtx.DragComponent = AppCtx.DrawLine | AppCtx.DrawHotpoint ? FALSE : TRUE;
			}
			break;
		}
	}
	
	for(std::list<HotpointDrawable*>::iterator it=AppCtx.HotpointLinee.begin(); !AppCtx.DrawHotpoint && it != AppCtx.HotpointLinee.end(); it++){
		if((*it)->HotpointOn(x, y)){
			Hotpoint *hp=dynamic_cast<Hotpoint*>(*it);
			AppCtx.DrawLine = !AppCtx.DrawLine;
			if(AppCtx.DrawLine){
				AppCtx.StartLine=dynamic_cast<PointInt*>(hp);
				AppCtx.buildingConnection=new Connection(NULL, hp);
			}else{
				//Aggiunta della nuova connessione alla lista
				AppCtx.buildingConnection->EndConnection(NULL, hp);					
				AppCtx.Connessioni.push_back(AppCtx.buildingConnection);
			}
			gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
			return;
		}
	}
	
	if(AppCtx.DrawLine){
		PointInt *current= new PointInt();
		current->x=(int)x;
		current->y=(int)y;
		AppCtx.buildingConnection->AppendPoint(current);
		AppCtx.StartLine=current;
	}else if(AppCtx.DrawHotpoint){
		for(std::list<Connection*>::iterator it=AppCtx.Connessioni.begin(); it != AppCtx.Connessioni.end(); it++){
			Line* l=(*it)->PointerOn(x, y);
			if(l){
				//attrazione verso la linea
				if(l->GetDirection() == Direction::Up || l->GetDirection() == Direction::Down)
					AppCtx.drawingHotpoint->Drag(l->GetStart()->x, y, NULL, NULL);
				else
					AppCtx.drawingHotpoint->Drag(x, l->GetStart()->y, NULL, NULL);
					
				//split della linea
				PointInt *np= dynamic_cast<PointInt*>(AppCtx.drawingHotpoint);
				Line *firstPart= new Line(l->GetStart(), np);
				Line *secondPart= new Line(np, l->GetEnd());
				
				//creazione della nuova connessione, dalla fine fino al nuovo punto
				Connection* c=new Connection(NULL, dynamic_cast<Hotpoint*>(AppCtx.drawingHotpoint));
				Line* tmp=(*it)->GetPath()->back();
				while(tmp != l){	//passo tutte le linee nella nuova connessione
					c->AddLine(tmp, true);
					(*it)->GetPath()->pop_back();
					tmp=(*it)->GetPath()->back();
				}
				c->AddLine(secondPart, true);	//fine nuova connessione e rimpiazzamento con firstPart
				(*it)->GetPath()->pop_back();
				(*it)->AddLine(firstPart, false);
				
				AppCtx.HotpointLinee.push_back(AppCtx.drawingHotpoint);
				AppCtx.drawingHotpoint=new HotpointDrawable(x, y);
				AppCtx.Connessioni.push_back(c);
				
				gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
				break;
			}
		}
	}
}

void DwaRelease(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, gpointer user_data){
	AppCtx.DragComponent=FALSE;
}

void LinePreview(cairo_t *cr){
	PointInt middlePoint, endPoint;
	if(!AppCtx.buildingConnection->GetPath()->empty()){
		//Stampa delle precedenti linee
		AppCtx.buildingConnection->Draw(cr);
		
		Line* lastLine=AppCtx.buildingConnection->GetPath()->back();
		switch(lastLine->GetDirection()){
		case Direction::Up:
			middlePoint={.x=AppCtx.StartLine->x, .y=std::min( AppCtx.CurrentPosition->y, AppCtx.StartLine->y )};
			endPoint={.x=AppCtx.CurrentPosition->x, .y=std::min( AppCtx.CurrentPosition->y, AppCtx.StartLine->y )};
			break;
		case Direction::Down:
			middlePoint={.x=AppCtx.StartLine->x, .y=std::max( AppCtx.CurrentPosition->y, AppCtx.StartLine->y )};
			endPoint={.x=AppCtx.CurrentPosition->x, .y=std::max( AppCtx.CurrentPosition->y, AppCtx.StartLine->y )};
			break;
		case Direction::Left:
			middlePoint={.x=std::min( AppCtx.CurrentPosition->x, AppCtx.StartLine->x ), .y=AppCtx.StartLine->y};
			endPoint={.x=std::min( AppCtx.CurrentPosition->x, AppCtx.StartLine->x ), .y=AppCtx.CurrentPosition->y};
			break;
		case Direction::Right:
			middlePoint={.x=std::max( AppCtx.CurrentPosition->x, AppCtx.StartLine->x ), .y=AppCtx.StartLine->y};
			endPoint={.x=std::max( AppCtx.CurrentPosition->x, AppCtx.StartLine->x ), .y=AppCtx.CurrentPosition->y};
			break;
		}
	}else{
		middlePoint={.x=AppCtx.CurrentPosition->x, .y=AppCtx.StartLine->y};
		endPoint={.x=AppCtx.CurrentPosition->x, .y=AppCtx.CurrentPosition->y};
	}
			
	double cx, cy;
	cairo_get_current_point(cr, &cx, &cy);
	if(( (int)cx != AppCtx.StartLine->x ) || ( (int)cy != AppCtx.StartLine->y ))
		cairo_move_to(cr, (double)AppCtx.StartLine->x, (double)AppCtx.StartLine->y);
	
	cairo_line_to(cr, middlePoint.x, middlePoint.y);
	cairo_line_to(cr, endPoint.x, endPoint.y);
}

gboolean DwaKeyPressed(GtkEventControllerKey *self, guint keyval, guint keycode, GdkModifierType state, gpointer user_data){
	gboolean ret;
	switch(keycode){
	case 9:		//ESC
		AppCtx.LastSelected=NULL;
		if(AppCtx.DrawHotpoint){
			AppCtx.DrawHotpoint=FALSE;
			delete AppCtx.drawingHotpoint;
		}else if(AppCtx.DrawLine){
			AppCtx.DrawLine=FALSE;
			delete AppCtx.buildingConnection;
		}
		ret=true;
		gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
		break;
	default:
		ret=false;
		break;
	}
	return ret;
}

