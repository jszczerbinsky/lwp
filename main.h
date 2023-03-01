#ifndef MAIN_H
#define MAIN_H

#include <SDL2/SDL.h>

#ifdef __WIN32
#include <SDL2/SDL_syswm.h>
#include <stdio.h>
#include <windows.h>
// MSVC doesn't have PATH_MAX
#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
#endif
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
  Monitor *monitors;
} Config;

typedef struct
{
  SDL_Window   *window;
  SDL_Renderer *renderer;
} App;

#endif // MAIN_H
