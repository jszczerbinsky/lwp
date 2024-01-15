#ifndef MAIN_H
#define MAIN_H

#include <gtk/gtk.h>

#include "../common.h"

extern GtkApplication *app;

extern GtkWidget *mainWnd;
extern GtkWidget *exitDialog;
extern GtkWidget *wallpaperMgrWnd;
extern GtkWidget *monitorWnd;
extern GtkWidget *wallpaperListBox;

void runWlp();
void killWlp();

#endif
