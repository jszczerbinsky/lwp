#ifndef WALLPAPER_H
#define WALLPAPER_H

#include "main.h"

int  applyWallpapers(Config *cfg);
void freeMonitor(WallpaperDest *monitor);
void runWallpaperLoop(Config *cfg);

#endif
