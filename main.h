#ifndef MAIN_H
#define MAIN_H

#include <SDL2/SDL.h>
#ifdef __WIN32
	#include <windows.h>
	#include <stdio.h>
	#include <SDL2/SDL_syswm.h>
#else
	#include <X11/Xlib.h>
	#include <linux/limits.h>
	#include <unistd.h>
	#include <pwd.h>
#endif

typedef struct
{
	SDL_Rect dest;
	SDL_Texture *buffTex;
} Instance;

typedef struct
{
	int srcWidth;
	int srcHeight;
	SDL_Window *window;
	SDL_Renderer *renderer;
	#ifndef __WIN32
		Display *display;
	#endif
} App;

extern App app;

#endif
