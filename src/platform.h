#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL3/SDL.h>

#include "types/monitor.h"

#ifdef OS_LINUX
void	 platform_init_wnd(SDL_Window** sdl_wnd, SDL_Renderer** sdl_ren);
Monitor* platform_scan_monitors(int* count);

#define DIR_SEP "/"
#endif
#ifdef OS_WINDOWS

#define DIR_SEP "\\"
#endif

#endif
