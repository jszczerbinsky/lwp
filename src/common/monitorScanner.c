#include <stdio.h>
#include <stdlib.h>

#include "../common.h"

#ifdef __WIN32
#include <windows.h>
#else
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#endif

#ifdef __WIN32
int monitorEnumIndex = 0;

static BOOL monitorenumproc(HMONITOR monitor, HDC hdc, LPRECT rect, LPARAM param)
{
  MONITORINFOEX info;
  info.cbSize = sizeof(MONITORINFOEX);

  GetMonitorInfo(monitor, (LPMONITORINFO)&info);

  MonitorInfo *mi = (MonitorInfo *)param + monitorEnumIndex;

  DEVMODE devmode;
  devmode.dmSize = sizeof(DEVMODE);
  EnumDisplaySettings(info.szDevice, ENUM_CURRENT_SETTINGS, &devmode);
  
  strncpy(mi->name, devmode.dmDeviceName, MONITOR_NAME_MAX);
  mi->bounds.x      = info.rcMonitor.left;
  mi->bounds.y      = info.rcMonitor.top;
  mi->bounds.w      = devmode.dmPelsWidth;
  mi->bounds.h      = devmode.dmPelsHeight;
  mi->config.loaded = 0;

  monitorEnumIndex++;
  return TRUE;
}
#endif

MonitorInfo *scanMonitors(int *count)
{
  MonitorInfo *m = NULL;

#ifdef __WIN32
  monitorEnumIndex = 0;
  *count           = GetSystemMetrics(SM_CMONITORS);
  m                = malloc(sizeof(MonitorInfo) * (*count));

  EnumDisplayMonitors(NULL, NULL, &monitorenumproc, (LPARAM)m);

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
