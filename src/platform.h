#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef OS_LINUX

#define DIR_SEP "/"
#include <limits.h>

#endif
#ifdef OS_WINDOWS

#define NTDDI_VERSION 0x0A000000
#define DIR_SEP		  "\\"
#include <windows.h>

#endif

#include <SDL3/SDL.h>

#include "types/monitor.h"
#include "types/wlpinfo.h"

void	 platform_init_wnd(SDL_Window** sdl_wnd, SDL_Renderer** sdl_ren);
Monitor* platform_scan_monitors(int* count);
WlpInfo* platform_scan_wlps(int* count);
void	 platform_get_schema_path(char path[PATH_MAX], const char* file_name);

#ifdef OS_WINDOWS
void win_get_program_files_path(char path[MAX_PATH]);
#endif

#endif
