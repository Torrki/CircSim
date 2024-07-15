#include "App.h"

int main(int argc, char* argv[]){
	GtkApplication *app=gtk_application_new("Gtk.CircSim", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(G_APPLICATION(app), "activate", G_CALLBACK(InitApp), NULL);
	int status=g_application_run(G_APPLICATION(app), argc, argv);
	return status;
}

