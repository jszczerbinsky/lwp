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
  SDL_Rect     dest;
  SDL_Texture *buffTex;
  SDL_Rect     finalDest;
  SDL_Texture *finalTex;
} Instance;

typedef struct
{
  int           srcWidth;
  int           srcHeight;
  SDL_Window   *window;
  SDL_Renderer *renderer;
#ifndef __WIN32
  Display *display;
#endif
} App;

#endif
