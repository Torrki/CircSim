#include <list>
#include "Component.h"
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
	/*Inizializzazione widget della finestra e bind delle azioni*/
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
	if(AppCtx.DrawLine)
		LinePreview(cr);
	cairo_stroke(cr);
	
	//Stampa degli hotpoint sulle linee
	cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);
	for(std::list<HotpointDrawable*>::iterator it=AppCtx.HotpointLinee.begin(); it != AppCtx.HotpointLinee.end(); it++){
		(*it)->Draw(cr);
	}
	if(AppCtx.DrawHotpoint){
		cairo_move_to(cr, AppCtx.CurrentPosition->x, AppCtx.CurrentPosition->y);
		cairo_arc(cr, AppCtx.CurrentPosition->x, AppCtx.CurrentPosition->y, HOTPOINT_RADIUS, 0.0, 2*M_PI);
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
	Resistor *r=new Resistor(60.0, 60.0, 100.0);
	//r->Rotate((5.0/2.0)*M_PI);
	//r->Rotate((5.0/2.0)*M_PI);
	AppCtx.Componenti.push_back(r);
	gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
}

void CButton_clicked(GtkButton* self, gpointer user_data){
	/*Aggiunge un bipolo condensatore alla lista dello schema*/
	Capacitor *c=new Capacitor(60.0, 60.0, 1.0);
	//c->Rotate(3.0*M_PI);
	AppCtx.Componenti.push_back(c);
	gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
}

void LButton_clicked(GtkButton* self, gpointer user_data){
	/*Aggiunge un bipolo induttore alla lista dello schema*/
	Inductor *l=new Inductor(60.0, 60.0, 40.0);
	//l->Rotate((7.0/2.0)*M_PI);
	AppCtx.Componenti.push_back(l);
	gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
}

void HPButton_clicked(GtkButton* self, gpointer user_data){
	/*Aggiunge un hotpoint alle linee dello schema*/
	AppCtx.DrawHotpoint=TRUE;
}

void DwaMotion(GtkEventControllerMotion* self, gdouble x, gdouble y, gpointer user_data){
	/*Aggiorno sempre la posizione corrente, disegno solo quando c'è una azione di disegno*/
	AppCtx.CurrentPosition->x=(int)x;
	AppCtx.CurrentPosition->y=(int)y;
	if(AppCtx.DragComponent){
		AppCtx.LastSelected->Drag(x,y);
		gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
	}else if(AppCtx.DrawLine){
		gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
	}else if(AppCtx.DrawHotpoint){
		gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
	}
}

void DwaClick(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, gpointer user_data){

	/*Ciclo sui componenti per vedere se posso eseguire una azione su di essi*/
	for(std::list<Component*>::iterator it=AppCtx.Componenti.begin(); !AppCtx.DrawHotpoint && it != AppCtx.Componenti.end(); it++){
		if((*it)->PointerOn(x, y)){
			AppCtx.LastSelected=*it;
			Hotpoint *hp=(*it)->HotpointOver(x, y); //Se disegno o termino una connessione
			if(hp && hp->Disponibile){
				AppCtx.DrawLine = !AppCtx.DrawLine;
				if(AppCtx.DrawLine){
					AppCtx.StartLine=static_cast<PointInt*>(hp);
					AppCtx.buildingConnection=new Connection(*it, hp);
				}else{
					//Aggiunta della nuova connessione alla lista
					AppCtx.buildingConnection->EndConnection(*it, hp);
					hp->Disponibile=false;
					
					//Aggiunta della connessione al componente
					(*it)->AddConnection(AppCtx.buildingConnection);
					
					Component* s=AppCtx.buildingConnection->GetCompStart();
					if(s){
						s->AddConnection(AppCtx.buildingConnection);
						AppCtx.buildingConnection->GetStart()->Disponibile=false;
					}else{
						int dirDelete=AppCtx.buildingConnection->GetPath()->front()->GetDirection();
						HotpointDrawable *start=static_cast<HotpointDrawable*>(AppCtx.buildingConnection->GetStart());
						start->AddConnection(dirDelete, AppCtx.buildingConnection);
						
						if(start->GetDirection()==0)start->Disponibile=false;
					}
					AppCtx.Connessioni.push_back(AppCtx.buildingConnection);
				}
				gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
				return;
			}
			else{	//DND
				AppCtx.DragComponent = AppCtx.DrawLine | AppCtx.DrawHotpoint ? FALSE : TRUE;
			}
			break;
		}
	}
	
	/*Ciclo sugli hotpoint disegnabili per vedere se posso eseguire una azione su di essi*/
	for(std::list<HotpointDrawable*>::iterator it=AppCtx.HotpointLinee.begin(); !AppCtx.DrawHotpoint && it != AppCtx.HotpointLinee.end(); it++){
		if((*it)->HotpointOn(x, y) && (*it)->Disponibile){ //Inizio o termino una connessione
			Hotpoint *hp=static_cast<Hotpoint*>(*it);
			AppCtx.DrawLine = !AppCtx.DrawLine;
			if(AppCtx.DrawLine){
				AppCtx.StartLine=static_cast<PointInt*>(hp);
				AppCtx.buildingConnection=new Connection(NULL, hp);
			}else{
				//Aggiunta della nuova connessione alla lista
				AppCtx.buildingConnection->EndConnection(NULL, hp);
				
				//Aggiunta della connessione al componente
				int dirDelete=AppCtx.buildingConnection->GetPath()->back()->GetDirection();
				switch(dirDelete){
				case DIR_UP:
					dirDelete=DIR_DOWN;
					break;
				case DIR_DOWN:
					dirDelete=DIR_UP;
					break;
				case DIR_LEFT:
					dirDelete=DIR_RIGHT;
					break;
				case DIR_RIGHT:
					dirDelete=DIR_LEFT;
					break;
				}
				(*it)->AddConnection(dirDelete, AppCtx.buildingConnection);
				if( (*it)->GetDirection()==0 ) (*it)->Disponibile=false;
				
				Component* s=AppCtx.buildingConnection->GetCompStart();
				if(s){
					s->AddConnection(AppCtx.buildingConnection);
					AppCtx.buildingConnection->GetStart()->Disponibile=false;
				}else{
					dirDelete=AppCtx.buildingConnection->GetPath()->front()->GetDirection();
					HotpointDrawable *start=static_cast<HotpointDrawable*>(AppCtx.buildingConnection->GetStart());
					start->AddConnection(dirDelete, AppCtx.buildingConnection);
					
					if(start->GetDirection()==0) start->Disponibile=false;
				}
				AppCtx.Connessioni.push_back(AppCtx.buildingConnection);
			}
			gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
			return;
		}
	}
	
	if(AppCtx.DrawLine){ //Punti intermedi del disegno delle nuove connessioni
		PointInt *current= new PointInt();
		current->x=(int)x;
		current->y=(int)y;
		AppCtx.buildingConnection->AppendPoint(current);
		AppCtx.StartLine=current;
	}else if(AppCtx.DrawHotpoint){ //Se sto piazzando un nuovo hotpoint su una linea
		for(std::list<Connection*>::iterator it=AppCtx.Connessioni.begin(); it != AppCtx.Connessioni.end(); it++){
			Line* l=(*it)->PointerOn(x, y);
			if(l){
				//attrazione verso la linea
				HotpointDrawable* newHD;
				if(l->GetDirection() == DIR_UP || l->GetDirection() == DIR_DOWN)
					newHD=new HotpointDrawable(l->GetStart()->x, y, DIR_LEFT | DIR_RIGHT);
				else
					newHD=new HotpointDrawable(x, l->GetStart()->y, DIR_UP | DIR_DOWN);
					
				//split della linea
				PointInt *np= static_cast<PointInt*>(newHD);
				Line *firstPart= new Line(l->GetStart(), np);
				Line *secondPart= new Line(np, l->GetEnd());
				
				//creazione della nuova connessione, dalla fine fino al nuovo punto
				Connection* c=new Connection(NULL, static_cast<Hotpoint*>(newHD));
				Line* tmp=(*it)->PopLine(false);
				while(tmp != l){	//passo tutte le linee nella nuova connessione
					c->AddLine(tmp, true);
					tmp=(*it)->PopLine(false);
				}
				c->AddLine(secondPart, true);	//fine nuova connessione e rimpiazzamento con firstPart
				(*it)->AddLine(firstPart, false);
				
				//Aggiunta delle nuove connessioni all'hotpoint
				newHD->AddConnection(0, *it);
				newHD->AddConnection(0, c);
				AppCtx.HotpointLinee.push_back(newHD);
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
	int direzione;
	if(!AppCtx.buildingConnection->GetPath()->empty()){
		//Stampa delle precedenti linee
		AppCtx.buildingConnection->Draw(cr);
		direzione=AppCtx.buildingConnection->GetPath()->back()->GetDirection();
	}else{
		Hotpoint* hp=static_cast<Hotpoint*>(AppCtx.StartLine);
		direzione=hp->GetDirection();
	}
	switch(direzione){
	case DIR_UP:
		middlePoint={.x=AppCtx.StartLine->x, .y=std::min( AppCtx.CurrentPosition->y, AppCtx.StartLine->y )};
		endPoint={.x=AppCtx.CurrentPosition->x, .y=std::min( AppCtx.CurrentPosition->y, AppCtx.StartLine->y )};
		break;
	case DIR_DOWN:
		middlePoint={.x=AppCtx.StartLine->x, .y=std::max( AppCtx.CurrentPosition->y, AppCtx.StartLine->y )};
		endPoint={.x=AppCtx.CurrentPosition->x, .y=std::max( AppCtx.CurrentPosition->y, AppCtx.StartLine->y )};
		break;
	case DIR_LEFT:
		middlePoint={.x=std::min( AppCtx.CurrentPosition->x, AppCtx.StartLine->x ), .y=AppCtx.StartLine->y};
		endPoint={.x=std::min( AppCtx.CurrentPosition->x, AppCtx.StartLine->x ), .y=AppCtx.CurrentPosition->y};
		break;
	case DIR_RIGHT:
		middlePoint={.x=std::max( AppCtx.CurrentPosition->x, AppCtx.StartLine->x ), .y=AppCtx.StartLine->y};
		endPoint={.x=std::max( AppCtx.CurrentPosition->x, AppCtx.StartLine->x ), .y=AppCtx.CurrentPosition->y};
		break;
	case DIR_UP | DIR_DOWN:
		middlePoint={.x=AppCtx.StartLine->x, .y=AppCtx.CurrentPosition->y};
		endPoint={.x=AppCtx.CurrentPosition->x, .y=AppCtx.CurrentPosition->y};
		break;
	case DIR_LEFT | DIR_RIGHT:
		middlePoint={.x=AppCtx.CurrentPosition->x, .y=AppCtx.StartLine->y};
		endPoint={.x=AppCtx.CurrentPosition->x, .y=AppCtx.CurrentPosition->y};
		break;
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
	printf("%d\n", keycode);
	switch(keycode){
	case 9:		//ESC
		AppCtx.LastSelected=NULL;
		if(AppCtx.DrawHotpoint){
			AppCtx.DrawHotpoint=FALSE;
		}else if(AppCtx.DrawLine){
			AppCtx.DrawLine=FALSE;
			delete AppCtx.buildingConnection;
		}
		ret=true;
		gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
		break;
	case 27: //R
		if(AppCtx.LastSelected){
			AppCtx.LastSelected->Rotate(M_PI/2.0);
		}
		ret=true;
		gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
	default:
		ret=false;
		break;
	}
	return ret;
}

