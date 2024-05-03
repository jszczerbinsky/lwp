#ifndef MAIN_H
#define MAIN_H

#include <gtk/gtk.h>

#include "../common.h"

extern GtkApplication *app;

extern GtkWidget *mainWnd;
extern GtkWidget *exitDialog;
extern GtkWidget *wallpaperMgrWnd;
extern GtkWidget *monitorWnd;
extern GtkWidget *monitorListBox;
extern GtkWidget *wallpaperListBox;
extern GtkWidget *wallpaperComboBox;
extern GtkWidget *xPosSpinBtn;
extern GtkWidget *yPosSpinBtn;
extern GtkWidget *widthSpinBtn;
extern GtkWidget *heightSpinBtn;
extern GtkWidget *monitorNameLabel;
extern GtkWidget *appSettingsWnd;
extern GtkWidget *targetFpsComboBox;
extern GtkWidget *renderQualityComboBox;
extern GtkWidget *unfocusedComebackComboBox;

void runWlp();
void killWlp();

#ifdef __WIN32
void initTrayIcon();
void removeTrayIcon();
#endif

#endif
