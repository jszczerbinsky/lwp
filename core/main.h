#ifndef MAIN_H
#define MAIN_H

#ifdef _MSC_VER
#define PATH_MAX MAX_PATH
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#ifdef __WIN32
#include <stdio.h>
#include <windows.h>
#elif __DARWIN
#include <objc/runtime.h>
#include <objc/message.h>
#include <Carbon/Carbon.h>
#include <CoreGraphics/CGDisplayConfiguration.h>
#include <limits.h>
#include <pwd.h>
#elif __LINUX
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
  int      targetFPS;
  Monitor *monitors;
} Config;

typedef struct
{
  SDL_Window   *window;
  SDL_Renderer *renderer;
} App;

#endif // MAIN_H
