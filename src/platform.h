#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL3/SDL.h>

#ifdef OS_LINUX
void linux_init_wnd(SDL_Window** sdl_wnd, SDL_Renderer** sdl_ren);
#define platform_init_wnd linux_init_wnd
#endif
#ifdef OS_WINDOWS

#endif

#endif
