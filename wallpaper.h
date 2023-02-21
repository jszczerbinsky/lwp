#ifndef WALLPAPER_H
#define WALLPAPER_H

#include <SDL2/SDL.h>

#include "main.h"

int  loadMonitors(App *app, Config *cfg);
void freeMonitor(Monitor *monitor);

#endif
