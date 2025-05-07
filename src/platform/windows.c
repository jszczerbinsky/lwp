#include <windows.h>
#include <SDL3/SDL.h>

#include "../types/monitor.h"

static HWND	icon_workerw;

static BOOL CALLBACK getIconWorkerw(HWND hwnd, LPARAM lParam)
{
  char buff[10];
  GetClassName(hwnd, buff, 10);

  if (strcmp(buff, "WorkerW") == 0)
  {
    HWND defView = FindWindowEx(hwnd, NULL, "SHELLDLL_DefView", NULL);
    if (defView)
    {
      icon_workerw = hwnd;
      return FALSE;
    }
  }
  return TRUE;
}

void platform_init_wnd(SDL_Window** sdl_wnd, SDL_Renderer** sdl_ren) {
	*sdl_wnd = SDL_CreateWindow("JPaper", 0, 0, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
	

	SDL_PropertiesID prop = SDL_GetWindowProperties(*sdl_wnd);
	
	HWND hwnd = (HWND)SDL_GetPointerProperty(prop, SDL_PROP_WINDOW_WIN32_HWND_POINTER, 0);

	HWND progman = FindWindow("Progman", NULL);
	icon_workerw  = progman;
	SendMessageTimeout(progman, 0x052C, 0, 0, SMTO_NORMAL, 1000, NULL);
	if (!FindWindowEx(progman, NULL, "SHELLDLL_DefView", NULL))
	EnumWindows(getIconWorkerw, 0);

	HWND wallpaperWorkerw = GetWindow(icon_workerw, GW_HWNDNEXT);
	SetParent(hwnd, wallpaperWorkerw);
	SetWindowLongPtr(
		hwnd,
		GWL_EXSTYLE,
		WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOACTIVATE
	);
	SetWindowLongPtr(hwnd, GWL_STYLE, WS_CHILDWINDOW | WS_VISIBLE);

	SetWindowPos(
		hwnd,
		NULL,
		0,
		0,
		1920,
		1080,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW
	);
	
	*sdl_ren = SDL_CreateRenderer(*sdl_wnd, NULL);
}

Monitor* platform_scan_monitors(int* count) {
	*count = 0;
	return NULL;
}
