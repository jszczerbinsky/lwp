#include <stdio.h>
#include <stdlib.h>

#include "../common.h"

#ifdef __WIN32
#include <Windows.h>
#include <shellscalingapi.h>
#else
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#endif

#ifdef __WIN32

int monitorEnumIndex = 0;

int primaryX = 0;
int primaryY = 0;

static float getScalingFactor(HMONITOR monitor)
{
  DEVICE_SCALE_FACTOR rawScaleFactor;
  HRESULT hr = GetScaleFactorForMonitor(monitor, &rawScaleFactor);

  return rawScaleFactor / 100;
}

static void getMonitorName(int index, char *name, char *displayName)
{
  DISPLAY_DEVICE dd;
  dd.cb = sizeof(DISPLAY_DEVICE);
  EnumDisplayDevices(NULL, index, &dd, EDD_GET_DEVICE_INTERFACE_NAME);
  DISPLAY_DEVICE dd2;
  dd2.cb = sizeof(DISPLAY_DEVICE);
  EnumDisplayDevices(dd.DeviceName, 0, &dd2, EDD_GET_DEVICE_INTERFACE_NAME );

  char *ptr = dd2.DeviceID;
  while(*ptr != '#')
    ptr++;
  ptr++;

  char *ptrEnd = ptr;
  int hashes = 0;
  while(hashes < 2)
  {
    ptrEnd++;
    if(*ptrEnd == '#')
      hashes++;
  }
  *ptrEnd = '\0';

  strncpy(displayName, dd2.DeviceString, MONITOR_NAME_MAX);
  strncpy(name, ptr, MONITOR_NAME_MAX);
}

static void getMonitorOriginalBounds(HMONITOR monitor, Bounds *bounds)
{
  MONITORINFOEX info;
  info.cbSize = sizeof(MONITORINFOEX);

  GetMonitorInfo(monitor, (LPMONITORINFO)&info);

  DEVMODE devmode;
  devmode.dmSize = sizeof(DEVMODE);
  EnumDisplaySettings(info.szDevice, ENUM_CURRENT_SETTINGS, &devmode);

  bounds->x = devmode.dmPosition.x;
  bounds->y = devmode.dmPosition.y;
  bounds->w = devmode.dmPelsWidth;
  bounds->h = devmode.dmPelsHeight;
}

static BOOL monitorenumproc_FindPrimaryCoords(HMONITOR monitor, HDC hdc, LPRECT rect, LPARAM param)
{
  Bounds bounds;
  getMonitorOriginalBounds(monitor, &bounds);

  if(bounds.x < primaryX) primaryX = bounds.x;
  if(bounds.y < primaryY) primaryY = bounds.y;

  return TRUE;
}

static BOOL monitorenumproc_GetInfo(HMONITOR monitor, HDC hdc, LPRECT rect, LPARAM param)
{
  MonitorInfo *mi = (MonitorInfo *)param + monitorEnumIndex;

  getMonitorOriginalBounds(monitor, &mi->originalBounds);
  getMonitorName(monitorEnumIndex, mi->name, mi->displayName);

 float scaleFactor = getScalingFactor(monitor);
MONITORINFOEX info;
  info.cbSize = sizeof(MONITORINFOEX);

  GetMonitorInfo(monitor, (LPMONITORINFO)&info);
  mi->bounds.x = mi->originalBounds.x * scaleFactor - primaryX;
  mi->bounds.y = mi->originalBounds.y * scaleFactor - primaryY;
  mi->bounds.w = mi->originalBounds.w * scaleFactor;
  mi->bounds.h = mi->originalBounds.h * scaleFactor;

  mi->config.loaded = 0;

  monitorEnumIndex++;
  return TRUE;
}
#endif

MonitorInfo *scanMonitors(int *count)
{
  MonitorInfo *m = NULL;

#ifdef __WIN32
  EnumDisplayMonitors(NULL, NULL, &monitorenumproc_FindPrimaryCoords, (LPARAM)NULL);

  monitorEnumIndex = 0;
  *count           = GetSystemMetrics(SM_CMONITORS);
  m                = malloc(sizeof(MonitorInfo) * (*count));

  EnumDisplayMonitors(NULL, NULL, &monitorenumproc_GetInfo, (LPARAM)m);

#else
  int monitorCount;

  Display        *display = XOpenDisplay((getenv("DISPLAY")));
  Window          wnd     = DefaultRootWindow(display);
  XRRMonitorInfo *info    = XRRGetMonitors(display, wnd, 0, &monitorCount);

  m = malloc(sizeof(MonitorInfo) * monitorCount);

  int i = 0;
  while (i < monitorCount)
  {
    snprintf(m[i].name, MONITOR_NAME_MAX, "%s", XGetAtomName(display, info->name));
    m[i].bounds.x      = info->x;
    m[i].bounds.y      = info->y;
    m[i].bounds.w      = info->width;
    m[i].bounds.h      = info->height;
    m[i].config.loaded = 0;

    info++;
    i++;
  }

  *count = monitorCount;

#endif
  return m;
}
