#include <list>
#include <typeinfo>
#include <complex>
#include "Componenti/BaseComp.h"
#include "Circuit.h"
#include "App.h"

using namespace std;

struct {
	Circuit *circuito=NULL;
	GtkDrawingArea *Dwa=NULL;
	PointInt* StartLine=NULL;
	PointInt* CurrentPosition=NULL;	
	PointInt* SpawnPosition=NULL;	
	Component *LastSelected=NULL;
	ConnectionBuilder *cb=NULL;
	HotpointDrawable *drawingHotpoint=NULL;
	gboolean DragComponent=false;
	gboolean DrawLine=false;
	gboolean DrawHotpoint=false;
}AppCtx;

void DrawSheet(GtkDrawingArea* self, cairo_t *cr, int width, int height, gpointer user_data);
void RButton_clicked(GtkButton* self, gpointer user_data);
void CButton_clicked(GtkButton* self, gpointer user_data);
void LButton_clicked(GtkButton* self, gpointer user_data);
void BatteryButton_clicked(GtkButton* self, gpointer user_data);
void GCButton_clicked(GtkButton* self, gpointer user_data);
void GNDButton_clicked(GtkButton* self, gpointer user_data);
void DwaMotion(GtkEventControllerMotion* self, gdouble x, gdouble y, gpointer user_data);
void DwaClick(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, gpointer user_data);
void DwaRelease(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, gpointer user_data);
gboolean DwaKeyPressed(GtkEventControllerKey *self, guint keyval, guint keycode, GdkModifierType state, gpointer user_data);
void LinePreview(cairo_t *cr);

void InitApp(GtkApplication *self, gpointer user_data){
	/*Inizializzazione widget della finestra e bind delle azioni*/
	AppCtx.CurrentPosition=new PointInt();
	AppCtx.SpawnPosition=new PointInt();
	AppCtx.SpawnPosition->x=60;
	AppCtx.SpawnPosition->y=60;
	AppCtx.circuito= new Circuit();
	AppCtx.cb=new ConnectionBuilder();
	
	GtkBuilder *builder=gtk_builder_new_from_file("./resources/UI/MainWin.ui");
	GtkWindow *Root=GTK_WINDOW(gtk_builder_get_object(builder, "RootWindow"));
	AppCtx.Dwa=GTK_DRAWING_AREA(gtk_builder_get_object(builder, "DrawSheet"));
	GtkButton *RButton=GTK_BUTTON(gtk_builder_get_object(builder, "ResistorButton"));
	GtkButton *CButton=GTK_BUTTON(gtk_builder_get_object(builder, "CapacitorButton"));
	GtkButton *LButton=GTK_BUTTON(gtk_builder_get_object(builder, "InductorButton"));
	GtkButton *BatteryButton=GTK_BUTTON(gtk_builder_get_object(builder, "BatteryButton"));
	GtkButton *GCButton=GTK_BUTTON(gtk_builder_get_object(builder, "CurrentSourceButton"));	
	GtkButton *GNDButton=GTK_BUTTON(gtk_builder_get_object(builder, "GNDButton"));
	
	g_signal_connect(G_OBJECT(RButton), "clicked", G_CALLBACK(RButton_clicked), NULL);
	g_signal_connect(G_OBJECT(CButton), "clicked", G_CALLBACK(CButton_clicked), NULL);
	g_signal_connect(G_OBJECT(LButton), "clicked", G_CALLBACK(LButton_clicked), NULL);
	g_signal_connect(G_OBJECT(BatteryButton), "clicked", G_CALLBACK(BatteryButton_clicked), NULL);
	g_signal_connect(G_OBJECT(GCButton), "clicked", G_CALLBACK(GCButton_clicked), NULL);
	g_signal_connect(G_OBJECT(GNDButton), "clicked", G_CALLBACK(GNDButton_clicked), NULL);
	
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
	for(list<Connection*>::iterator it=AppCtx.circuito->Connessioni.begin(); it != AppCtx.circuito->Connessioni.end(); it++){
		(*it)->Draw(cr);
	}
	if(AppCtx.DrawLine)
		LinePreview(cr);
	cairo_stroke(cr);
	
	//Stampa degli hotpoint sulle linee
	cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);
	for(list<HotpointDrawable*>::iterator it=AppCtx.circuito->HotpointLinee.begin(); it != AppCtx.circuito->HotpointLinee.end(); it++){
		(*it)->Draw(cr);
	}
	if(AppCtx.DrawHotpoint){
		cairo_move_to(cr, AppCtx.CurrentPosition->x, AppCtx.CurrentPosition->y);
		cairo_arc(cr, AppCtx.CurrentPosition->x, AppCtx.CurrentPosition->y, HOTPOINT_RADIUS, 0.0, 2*M_PI);
	}
	cairo_fill(cr);
	
	//Stampa dei componenti	
	for(list<Component*>::iterator it=AppCtx.circuito->Componenti.begin(); it != AppCtx.circuito->Componenti.end(); it++){
		(*it)->Draw(cr);
	}
	//cairo_save(cr);
}

void RButton_clicked(GtkButton* self, gpointer user_data){
	/*Aggiunge un bipolo resistore alla lista dello schema*/
	Resistor *r=new Resistor(*AppCtx.SpawnPosition, 100.0);
	AppCtx.circuito->Componenti.push_back(r);
	gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
}

void CButton_clicked(GtkButton* self, gpointer user_data){
	/*Aggiunge un bipolo condensatore alla lista dello schema*/
	Capacitor *c=new Capacitor(*AppCtx.SpawnPosition, 1.0);
	AppCtx.circuito->Componenti.push_back(c);
	gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
}

void LButton_clicked(GtkButton* self, gpointer user_data){
	/*Aggiunge un bipolo induttore alla lista dello schema*/
	Inductor *l=new Inductor(*AppCtx.SpawnPosition, 40.0);
	AppCtx.circuito->Componenti.push_back(l);
	gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
}

void BatteryButton_clicked(GtkButton* self, gpointer user_data){
	/*Aggiunge una batteria allo schema*/
	Battery *b=new Battery(*AppCtx.SpawnPosition, 5.0, 0.0);
	AppCtx.circuito->Componenti.push_back(b);
	gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
}

void GCButton_clicked(GtkButton* self, gpointer user_data){
	CurrentSource *gc=new CurrentSource(*AppCtx.SpawnPosition, 1.0, 200000.0);
	AppCtx.circuito->Componenti.push_back(gc);
	gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
}

void GNDButton_clicked(GtkButton* self, gpointer user_data){
	GND *gnd=new GND(*AppCtx.SpawnPosition);
	AppCtx.circuito->Componenti.push_back(gnd);
	gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
}

void DwaMotion(GtkEventControllerMotion* self, gdouble x, gdouble y, gpointer user_data){
	/*Aggiorno sempre la posizione corrente, disegno solo quando c'è una azione di disegno*/
	AppCtx.CurrentPosition->x=(int)x;
	AppCtx.CurrentPosition->y=(int)y;
	if(AppCtx.DragComponent){
		AppCtx.LastSelected->Drag(*AppCtx.CurrentPosition);
		gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
	}else if(AppCtx.DrawLine){
		gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
	}else if(AppCtx.DrawHotpoint){
		gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
	}
}

void DwaClick(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, gpointer user_data){
	AppCtx.CurrentPosition->x=(int)x;
	AppCtx.CurrentPosition->y=(int)y;

	/*Ciclo sui componenti per vedere se posso eseguire una azione su di essi*/
	for(list<Component*>::iterator it=AppCtx.circuito->Componenti.begin(); !AppCtx.DrawHotpoint && it != AppCtx.circuito->Componenti.end(); it++){
		if((*it)->PointerOn(*AppCtx.CurrentPosition)){
			AppCtx.LastSelected=*it;
			Hotpoint *hp=(*it)->HotpointOver(*AppCtx.CurrentPosition); //Se disegno o termino una connessione
			if(hp && hp->Disponibile){
				AppCtx.DrawLine = !AppCtx.DrawLine;
				if(AppCtx.DrawLine){
					AppCtx.StartLine=static_cast<PointInt*>(hp);
					AppCtx.cb->NewConnection(*it, hp);
				}else{
					//Aggiunta della nuova connessione alla lista
					Component* end=*it;
					AppCtx.cb->EndConnection(end, hp);					
					hp->Disponibile=false;
					
					//Aggiunta della connessione al componente
					Connection* newConn=AppCtx.cb->GetConnection();
					end->AddConnection(newConn);
					
					Component* s=newConn->GetStart().first;
					if(s){
						s->AddConnection(newConn);
						newConn->GetStart().second->Disponibile=false;
					}else{
						int dirDelete=newConn->GetPath().front()->GetDirection();
						HotpointDrawable *start=static_cast<HotpointDrawable*>(newConn->GetStart().second);
						start->AddConnection(dirDelete, newConn);
						
						if(start->GetDirection()==0)start->Disponibile=false;
					}
					AppCtx.circuito->Connessioni.push_back(newConn);
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
	for(list<HotpointDrawable*>::iterator it=AppCtx.circuito->HotpointLinee.begin(); !AppCtx.DrawHotpoint && it != AppCtx.circuito->HotpointLinee.end(); it++){
		if((*it)->PointerOn(*AppCtx.CurrentPosition) && (*it)->Disponibile){ //Inizio o termino una connessione
			Hotpoint *hp=static_cast<Hotpoint*>(*it);
			AppCtx.DrawLine = !AppCtx.DrawLine;
			if(AppCtx.DrawLine){
				AppCtx.StartLine=static_cast<PointInt*>(hp);
				AppCtx.cb->NewConnection(NULL, hp);
			}else{
				//Aggiunta della nuova connessione alla lista
				AppCtx.cb->EndConnection(NULL, hp);
				
				//Aggiunta della connessione al componente
				Connection* newConn=AppCtx.cb->GetConnection();
				int dirDelete=newConn->GetPath().back()->GetDirection();
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
				(*it)->AddConnection(dirDelete, newConn);
				if( (*it)->GetDirection()==0 ) (*it)->Disponibile=false;
				
				Component* s=newConn->GetStart().first;
				if(s){
					s->AddConnection(newConn);
					newConn->GetStart().second->Disponibile=false;
				}else{
					dirDelete=newConn->GetPath().front()->GetDirection();
					HotpointDrawable *start=static_cast<HotpointDrawable*>(newConn->GetStart().second);
					start->AddConnection(dirDelete, newConn);
					
					if(start->GetDirection()==0) start->Disponibile=false;
				}
				AppCtx.circuito->Connessioni.push_back(newConn);
			}
			gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
			return;
		}
	}
	
	if(AppCtx.DrawLine){ //Punti intermedi del disegno delle nuove connessioni		
		//Se clicco su una connessione creo un hotpoint disegnabile
		list<Connection*>::iterator itConnessioni=AppCtx.circuito->Connessioni.begin();
		for(; itConnessioni != AppCtx.circuito->Connessioni.end(); ++itConnessioni){
			Line* l=(*itConnessioni)->LineOn(*AppCtx.CurrentPosition);
			if(l){
				
				HotpointDrawable* newHD;
				int direzioni;
				PointInt* point;
				//se seleziono un angolo
				PointInt *angolo=l->EdgeOn(*AppCtx.CurrentPosition);
				if(angolo){
					AppCtx.cb->AppendPoint(angolo);
					
					//devo prendere la linea successiva per vedere quali direzioni eliminare
					list<Line*> percorso=(*itConnessioni)->GetPath();
					Line* successiva=NULL;
					for(list<Line*>::iterator it_percorso=percorso.begin(); it_percorso != percorso.end(); it_percorso++){
						if(*it_percorso == l){
							successiva=(*(++it_percorso));
							break;
						}
					}
					switch(successiva->GetDirection()){
					case DIR_UP:
						direzioni=DIR_DOWN;
						break;
					case DIR_DOWN:
						direzioni=DIR_UP;
						break;
					case DIR_LEFT:
						direzioni=DIR_RIGHT;
						break;
					case DIR_RIGHT:
						direzioni=DIR_LEFT;
						break;
					}
					
					point=angolo;
					direzioni |= l->GetDirection();					
				}else{
					point=new PointInt();
					//attrazione verso la linea
					if(l->GetDirection() == DIR_UP || l->GetDirection() == DIR_DOWN){
						point->x=l->GetStart()->x;
						point->y=y;
						direzioni=DIR_LEFT | DIR_RIGHT;
					}else{
						point->x=x;
						point->y=l->GetStart()->y;
						direzioni=DIR_UP | DIR_DOWN;
					}
					AppCtx.cb->AppendPoint(point);
				}
				
				//Levo la direzione dall'hotpoint
				Connection *createdConn=AppCtx.cb->GetConnection();
				Component* s=createdConn->GetStart().first;
				if(s){
					s->AddConnection(createdConn);
					createdConn->GetStart().second->Disponibile=false;
				}else{
					int dirDelete=createdConn->GetPath().front()->GetDirection();
					HotpointDrawable *start=static_cast<HotpointDrawable*>(createdConn->GetStart().second);
					start->AddConnection(dirDelete, createdConn);
					
					if(start->GetDirection()==0) start->Disponibile=false;
				}
				
				//Levo la direzione della connessione creata
				switch(createdConn->GetPath().back()->GetDirection()){
				case DIR_UP:
					direzioni &= ~DIR_DOWN;
					break;
				case DIR_DOWN:
					direzioni &= ~DIR_UP;
					break;
				case DIR_LEFT:
					direzioni &= ~DIR_RIGHT;
					break;
				case DIR_RIGHT:
					direzioni &= ~DIR_LEFT;
					break;
				}
				
				AppCtx.circuito->Connessioni.push_back(AppCtx.cb->GetConnection());
				newHD=new HotpointDrawable(*point, direzioni);
				
				//split della linea
				AppCtx.cb->SetConnection(*itConnessioni);
				Connection *nc=AppCtx.cb->SplitConnection(newHD, l);
				
				//Aggiunta delle nuove connessioni all'hotpoint
				newHD->AddConnection(0, *itConnessioni);
				newHD->AddConnection(0, nc);
				AppCtx.circuito->HotpointLinee.push_back(newHD);
				AppCtx.circuito->Connessioni.push_back(nc);
				
				AppCtx.DrawLine=false;
				gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
				return;
				
			}
		}
		
		if(itConnessioni == AppCtx.circuito->Connessioni.end()){ //Se non clicco alcuna connessione
			PointInt *current= new PointInt();
			current->x=(int)x;
			current->y=(int)y;
			AppCtx.cb->AppendPoint(current);
			AppCtx.StartLine=current;
		}
	}
}

void DwaRelease(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, gpointer user_data){
	AppCtx.DragComponent=FALSE;
}

void LinePreview(cairo_t *cr){
	PointInt middlePoint, endPoint;
	int direzione;
	Connection *currentConn=AppCtx.cb->GetConnection();
	if(!currentConn->GetPath().empty()){
		//Stampa delle precedenti linee
		currentConn->Draw(cr);
		direzione=currentConn->GetPath().back()->GetDirection();
	}else{
		Hotpoint* hp=static_cast<Hotpoint*>(AppCtx.StartLine);
		direzione=hp->GetDirection();
	}

	switch(direzione){
	case DIR_UP:
		middlePoint={.x=AppCtx.StartLine->x, .y=min( AppCtx.CurrentPosition->y, AppCtx.StartLine->y )};
		endPoint={.x=AppCtx.CurrentPosition->x, .y=min( AppCtx.CurrentPosition->y, AppCtx.StartLine->y )};
		break;
	case DIR_DOWN:
		middlePoint={.x=AppCtx.StartLine->x, .y=max( AppCtx.CurrentPosition->y, AppCtx.StartLine->y )};
		endPoint={.x=AppCtx.CurrentPosition->x, .y=max( AppCtx.CurrentPosition->y, AppCtx.StartLine->y )};
		break;
	case DIR_LEFT:
		middlePoint={.x=min( AppCtx.CurrentPosition->x, AppCtx.StartLine->x ), .y=AppCtx.StartLine->y};
		endPoint={.x=min( AppCtx.CurrentPosition->x, AppCtx.StartLine->x ), .y=AppCtx.CurrentPosition->y};
		break;
	case DIR_RIGHT:
		middlePoint={.x=max( AppCtx.CurrentPosition->x, AppCtx.StartLine->x ), .y=AppCtx.StartLine->y};
		endPoint={.x=max( AppCtx.CurrentPosition->x, AppCtx.StartLine->x ), .y=AppCtx.CurrentPosition->y};
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
	//printf("%d\n", keycode);
	switch(keycode){
	case 9:		//ESC
		AppCtx.LastSelected=NULL;
		if(AppCtx.DrawHotpoint){
			AppCtx.DrawHotpoint=FALSE;
		}else if(AppCtx.DrawLine){
			AppCtx.DrawLine=FALSE;
			AppCtx.cb->ClearConnection();
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

