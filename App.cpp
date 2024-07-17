#include <list>
#include "Component.h"
#include "Line.h"
#include "App.h"

struct {
	std::list<Component*> Componenti;
	std::list<Connection*> Connessioni;
	GtkDrawingArea *Dwa;
	PointInt* StartLine;
	PointInt* CurrentPosition;	
	Component *LastSelected=NULL;
	Connection *buildingConnection;
	gboolean DragComponent;
	gboolean DrawLine;
}AppCtx;

void DrawSheet(GtkDrawingArea* self, cairo_t *cr, int width, int height, gpointer user_data);
void RButton_clicked(GtkButton* self, gpointer user_data);
void CButton_clicked(GtkButton* self, gpointer user_data);
void LButton_clicked(GtkButton* self, gpointer user_data);
void DwaMotion(GtkEventControllerMotion* self, gdouble x, gdouble y, gpointer user_data);
void DwaClick(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, gpointer user_data);
void DwaRelease(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, gpointer user_data);

void InitApp(GtkApplication *self, gpointer user_data){
	AppCtx.CurrentPosition=new PointInt();
	
	GtkBuilder *builder=gtk_builder_new_from_file("./resources/UI/MainWin.ui");
	GtkWindow *Root=GTK_WINDOW(gtk_builder_get_object(builder, "RootWindow"));
	AppCtx.Dwa=GTK_DRAWING_AREA(gtk_builder_get_object(builder, "DrawSheet"));
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
	gtk_widget_add_controller(GTK_WIDGET(AppCtx.Dwa), motionDwaController);
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
	if(AppCtx.DrawLine){
		PointInt middlePoint, endPoint;
		if(!AppCtx.buildingConnection->Empty()){
			//Stampa delle precedenti linee
			AppCtx.buildingConnection->Draw(cr);
			
			Line* lastLine=AppCtx.buildingConnection->GetPath().back();
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
			middlePoint={.x=AppCtx.StartLine->x, .y=AppCtx.CurrentPosition->y};
			endPoint={.x=AppCtx.CurrentPosition->x, .y=AppCtx.CurrentPosition->y};
		}
				
		double cx, cy;
		cairo_get_current_point(cr, &cx, &cy);
		if(( (int)cx != AppCtx.StartLine->x ) || ( (int)cy != AppCtx.StartLine->y ))
			cairo_move_to(cr, AppCtx.StartLine->x, AppCtx.StartLine->y);
		
		cairo_line_to(cr, middlePoint.x, middlePoint.y);
		cairo_line_to(cr, endPoint.x, endPoint.y);
	}
	cairo_stroke(cr);
	
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

void DwaMotion(GtkEventControllerMotion* self, gdouble x, gdouble y, gpointer user_data){
	AppCtx.CurrentPosition->x=(int)x;
	AppCtx.CurrentPosition->y=(int)y;
	if(AppCtx.DragComponent){
		AppCtx.LastSelected->Drag(x,y);
		gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
	}else if(AppCtx.DrawLine){
		gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
	}
}

void DwaClick(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, gpointer user_data){

	for(std::list<Component*>::iterator it=AppCtx.Componenti.begin(); it != AppCtx.Componenti.end(); it++){
		if((*it)->PointerOn(x, y)){
			AppCtx.LastSelected=*it;
			Hotpoint *hp=(*it)->HotpointOver(x,y);
			if(hp){
				AppCtx.DrawLine = !AppCtx.DrawLine;
				if(AppCtx.DrawLine){
					AppCtx.StartLine=dynamic_cast<PointInt*>(hp);
					AppCtx.buildingConnection=new Connection();
				}else{
					PointInt *middlePoint=new PointInt();
					PointInt *endPoint=dynamic_cast<PointInt*>(hp);
					PointInt *startPoint;
					if(!AppCtx.buildingConnection->Empty()){
						Line* lastLine=AppCtx.buildingConnection->GetPath().back();
						startPoint=lastLine->GetEnd();
						switch(lastLine->GetDirection()){
						case Direction::Up:
							middlePoint->x=startPoint->x;
							middlePoint->y=endPoint->y;
							/*endPoint->x=(int)x;
							endPoint->y=std::min( (int)y, startPoint->y );*/
							break;
						case Direction::Down:
							middlePoint->x=startPoint->x;
							middlePoint->y=endPoint->y;
							/*endPoint->x=(int)x;
							endPoint->y=std::max( (int)y, startPoint->y );*/
							break;
						case Direction::Left:
							middlePoint->x=endPoint->x;
							middlePoint->y=startPoint->y;
							/*endPoint->x=std::min( (int)x, startPoint->x );
							endPoint->y=(int)y;*/
							break;
						case Direction::Right:
							middlePoint->x=endPoint->x;
							middlePoint->y=startPoint->y;
							/*endPoint->x=std::max( (int)x, startPoint->x );
							endPoint->y=(int)y;*/
							break;
						}
						gtk_widget_queue_draw(GTK_WIDGET(AppCtx.Dwa));
						
					}else{
						startPoint=new PointInt();
						
						middlePoint->x=AppCtx.StartLine->x;
						middlePoint->y=(int)y;		
						endPoint->x=(int)x;
						endPoint->y=(int)y;
						startPoint->x=AppCtx.StartLine->x;
						startPoint->y=AppCtx.StartLine->y;
					}
					Line *l1=new Line(startPoint, middlePoint);
					Line *l2=new Line(middlePoint, endPoint);
					AppCtx.buildingConnection->AppendLine(l1);
					AppCtx.buildingConnection->AppendLine(l2);
					
					//Aggiunta della nuova connessione alla lista
					AppCtx.Connessioni.push_back(AppCtx.buildingConnection);
				}
				return;
			}
			else{
				AppCtx.DragComponent = AppCtx.DrawLine ? FALSE : TRUE;
			}
			break;
		}
	}
	
	if(AppCtx.DrawLine){
		PointInt *middlePoint=new PointInt();
		PointInt *endPoint=new PointInt();
		PointInt *startPoint;
		if(!AppCtx.buildingConnection->Empty()){
			Line* lastLine=AppCtx.buildingConnection->GetPath().back();
			startPoint=lastLine->GetEnd();
			switch(lastLine->GetDirection()){
			case Direction::Up:
				middlePoint->x=startPoint->x;
				middlePoint->y=std::min( (int)y, startPoint->y );
				endPoint->x=(int)x;
				endPoint->y=std::min( (int)y, startPoint->y );
				break;
			case Direction::Down:
				middlePoint->x=startPoint->x;
				middlePoint->y=std::max( (int)y, startPoint->y );
				endPoint->x=(int)x;
				endPoint->y=std::max( (int)y, startPoint->y );
				break;
			case Direction::Left:
				middlePoint->x=std::min( (int)x, startPoint->x );
				middlePoint->y=startPoint->y;
				endPoint->x=std::min( (int)x, startPoint->x );
				endPoint->y=(int)y;
				break;
			case Direction::Right:
				middlePoint->x=std::max( (int)x, startPoint->x );
				middlePoint->y=startPoint->y;
				endPoint->x=std::max( (int)x, startPoint->x );
				endPoint->y=(int)y;
				break;
			}
			
		}else{
			//startPoint=new PointInt();
			middlePoint->x=AppCtx.StartLine->x;
			middlePoint->y=(int)y;		
			endPoint->x=(int)x;
			endPoint->y=(int)y;
			startPoint=AppCtx.StartLine;
		}
		Line *l1=new Line(startPoint, middlePoint);
		Line *l2=new Line(middlePoint, endPoint);
		AppCtx.buildingConnection->AppendLine(l1);
		AppCtx.buildingConnection->AppendLine(l2);
		AppCtx.StartLine=endPoint;
	}
}

void DwaRelease(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, gpointer user_data){
	AppCtx.DragComponent=FALSE;
}

