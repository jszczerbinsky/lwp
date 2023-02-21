#ifndef MAIN_H
#define MAIN_H

#include <SDL2/SDL.h>

#ifdef __WIN32
#include <SDL2/SDL_syswm.h>
#include <stdio.h>
#include <windows.h>
#else
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <linux/limits.h>
#include <pwd.h>
#include <unistd.h>
#endif

typedef struct
{
  float        sensitivityX;
  float        sensitivityY;
  SDL_Texture *tex;
} Layer;

typedef struct
{
  char         dirPath[PATH_MAX];
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
typedef struct
{
  int      reloadRootWnd;
  int      monitorsCount;
  float    smooth;
  Monitor *monitors;
} Config;

typedef struct
{
  SDL_Window   *window;
  SDL_Renderer *renderer;
#ifndef __WIN32
  Display *display;
#endif
} App;

#endif
