#ifndef MAIN_H
#define MAIN_H

#include <gtk/gtk.h>

#define MONITOR_NAME_MAX 100

typedef struct
{
  int x;
  int y;
  int w;
  int h;
} Bounds;

typedef struct
{
  int    set;
  char   wlpPath[PATH_MAX];
  Bounds wlpBounds;
} MonitorConfig;

typedef struct
{
  char          name[MONITOR_NAME_MAX];
  Bounds        bounds;
  MonitorConfig config;
} Monitor;

extern GtkApplication *app;

extern GtkWidget *mainWnd;
extern GtkWidget *exitDialog;
extern GtkWidget *wallpaperMgrWnd;
extern GtkWidget *monitorWnd;

void runWlp();
void killWlp();

Monitor *scanMonitors(int *count);

#endif
