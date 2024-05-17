#ifndef MAIN_H
#define MAIN_H

#include "../platform_guard.h"

#include <gtk/gtk.h>

#include "../common.h"

extern GtkApplication *app;

#define CONTROL_MAIN_WND                0
#define CONTROL_MON_LIST                1
#define CONTROL_MON_SWITCH              2
#define CONTROL_MON_WLP                 3
#define CONTROL_MON_OFFSET_X            4
#define CONTROL_MON_OFFSET_Y            5
#define CONTROL_MON_WIDTH               6
#define CONTROL_MON_HEIGHT              7
#define CONTROL_APP_TARGET_FPS          8
#define CONTROL_APP_TEX_FILTERING       9
#define CONTROL_APP_UNFOCUSED_BEHAVIOUR 10
#define CONTROL_APP_TARGET_POINT        11
#define CONTROL_VER_LABEL               12
#define CONTROL_MAIN_STACK              13
#define CONTROL_SIDEBAR                 14
#define CONTROL_MON_ASPECT_RATIO        15
#define CONTROL_LOG_WINDOW              16
#define CONTROL_LOG_TEXT                17

#define CONTROLS_MAX 18

typedef struct
{
  const char *name;
  GtkWidget  *widget;
} Control;

extern Control controls[];

#define WID(x) (controls[(x)].widget)

void runWlp();
void killWlp();

#ifdef __WIN32
void initTrayIcon();
void removeTrayIcon();
#endif

#endif
