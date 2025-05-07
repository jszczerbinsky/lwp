#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef OS_LINUX

#define DIR_SEP "/"
#endif
#ifdef OS_WINDOWS

#define NTDDI_VERSION 0x0A000000
#define DIR_SEP "\\"
#endif

#include <SDL3/SDL.h>

#include "types/monitor.h"

void	 platform_init_wnd(SDL_Window** sdl_wnd, SDL_Renderer** sdl_ren);
Monitor* platform_scan_monitors(int* count);

#endif
