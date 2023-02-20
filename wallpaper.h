#ifndef WALLPAPER_H
#define WALLPAPER_H

#include <SDL2/SDL.h>

#include "main.h"

typedef struct
{
  float        sensitivityX;
  float        sensitivityY;
  SDL_Texture *tex;
} Layer;

typedef struct
{
  int          repeatX, repeatY;
  int          layersCount;
  Layer       *layers;
  int          originalW, originalH;
  SDL_Texture *tex;
} Wallpaper;

typedef struct
{
  int          x, y, w, h;
  int          wallpaperX, wallpaperY, wallpaperW, wallpaperH;
  SDL_Texture *tex;
  Wallpaper    wallpaper;
} Monitor;

int  loadWallpaper(App *app, Monitor *monitor, Wallpaper *wallpaper, const char *dirPath);
void freeMonitor(Monitor *monitor);

#endif
