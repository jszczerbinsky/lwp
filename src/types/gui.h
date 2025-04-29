#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>

#define GUI_MAIN_WND_NAME			"MainWindow"
#define GUI_MON_LSTBOX_NAME			"MonitorListBox"
#define GUI_MON_RES_LABEL_NAME		"MonitorResolutionLabel"
#define GUI_MON_ITEM_NAME			"MonitorItem"
#define GUI_MON_ITEM_LABEL_NAME		"MonitorItemLabel"
#define GUI_MON_ITEM_RES_LABEL_NAME "MonitorItemResLabel"

typedef struct {
	int gui_built;

	GtkApplication* gtk_app;

	GtkWidget* main_wnd;
	GtkWidget* mon_lstbox;
	GtkWidget* mon_res_label;
} AppGUI;

typedef struct {
	AppGUI gui;
} App;

void gui_init(AppGUI* app, int argc, char** argv);

#endif
