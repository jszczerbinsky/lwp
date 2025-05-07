#include <Windows.h>
#include <SDL3/SDL.h>
#include <shellscalingapi.h>

#include "../types/monitor.h"

static HWND	icon_workerw;

static BOOL CALLBACK get_icon_workerw(HWND hwnd, LPARAM lParam)
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
	EnumWindows(get_icon_workerw, 0);

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

int monitorEnumIndex = 0;

int primaryX = 0;
int primaryY = 0;

static float get_scaling_factor(HMONITOR monitor)
{
  DEVICE_SCALE_FACTOR rawScaleFactor;
  HRESULT             hr = GetScaleFactorForMonitor(monitor, &rawScaleFactor);

  return rawScaleFactor / 100;
}

static void get_monitor_name(int index, char *name, char *display_name)
{
  DISPLAY_DEVICE dd;
  dd.cb = sizeof(DISPLAY_DEVICE);
  EnumDisplayDevices(NULL, index, &dd, EDD_GET_DEVICE_INTERFACE_NAME);
  DISPLAY_DEVICE dd2;
  dd2.cb = sizeof(DISPLAY_DEVICE);
  EnumDisplayDevices(dd.DeviceName, 0, &dd2, EDD_GET_DEVICE_INTERFACE_NAME);

  char *ptr = dd2.DeviceID;
  while (*ptr != '#') ptr++;
  ptr++;

  char *ptrEnd = ptr;
  int   hashes = 0;
  while (hashes < 2)
  {
    ptrEnd++;
    if (*ptrEnd == '#') hashes++;
  }
  *ptrEnd = '\0';

  strncpy(display_name, dd2.DeviceString, MONITOR_NAME_MAX);
  strncpy(name, ptr, MONITOR_NAME_MAX);
}

static void get_monitor_bounds(HMONITOR monitor, Bounds *original, Bounds *client, Bounds *virtual)
{
  MONITORINFOEX info;
  info.cbSize = sizeof(MONITORINFOEX);
  GetMonitorInfo(monitor, (LPMONITORINFO)&info);

  if (original) {
    original->x = info.rcMonitor.left;
    original->y = info.rcMonitor.top;
    original->w = info.rcMonitor.right - info.rcMonitor.left;
    original->h = info.rcMonitor.bottom - info.rcMonitor.top;
  }

  DEVMODE devmode;
  devmode.dmSize = sizeof(DEVMODE);
  EnumDisplaySettings(info.szDevice, ENUM_CURRENT_SETTINGS, &devmode);

  if (client) {
    client->x = devmode.dmPosition.x;
    client->y = devmode.dmPosition.y;
    client->w = devmode.dmPelsWidth;
    client->h = devmode.dmPelsHeight;
  }

  float scaleFactor = get_scaling_factor(monitor);

  if (virtual) {
    virtual->x = devmode.dmPosition.x * scaleFactor - primaryX;
    virtual->y = devmode.dmPosition.y * scaleFactor - primaryY;
    virtual->w = devmode.dmPelsWidth * scaleFactor;
    virtual->h = devmode.dmPelsHeight * scaleFactor;
  }
}

static BOOL monitorenumproc_find_primary_coords(HMONITOR monitor, HDC hdc, LPRECT rect, LPARAM param) {
  Bounds bounds;
  get_monitor_bounds(monitor, NULL, &bounds, NULL);

  if (bounds.x < primaryX) primaryX = bounds.x;
  if (bounds.y < primaryY) primaryY = bounds.y;

  return TRUE;
}

static BOOL monitorenumproc_get_info(HMONITOR monitor, HDC hdc, LPRECT rect, LPARAM param) {
  Monitor *mi = (Monitor *)param + monitorEnumIndex;

  get_monitor_bounds(monitor, &mi->raw_bounds, &mi->client_bounds, &mi->virt_bounds);
  get_monitor_name(monitorEnumIndex, mi->name, mi->display_name);

  monitorEnumIndex++;
  return TRUE;
}

Monitor* platform_scan_monitors(int* count) {
	EnumDisplayMonitors(NULL, NULL, &monitorenumproc_find_primary_coords, (LPARAM)NULL);
	monitorEnumIndex = 0;
  
	*count = GetSystemMetrics(SM_CMONITORS);
	Monitor *m = malloc(sizeof(Monitor) * (*count));

	EnumDisplayMonitors(NULL, NULL, &monitorenumproc_get_info, (LPARAM)m);
	
	return m;
}
