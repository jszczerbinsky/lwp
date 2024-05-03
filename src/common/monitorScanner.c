#include <stdio.h>
#include <stdlib.h>

#include "../common.h"

#ifdef __WIN32
#include <Windows.h>
#include <shellscalingapi.h>
#else
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <string.h>
#endif

#ifdef __WIN32

int monitorEnumIndex = 0;

int primaryX = 0;
int primaryY = 0;

static float getScalingFactor(HMONITOR monitor)
{
  DEVICE_SCALE_FACTOR rawScaleFactor;
  HRESULT             hr = GetScaleFactorForMonitor(monitor, &rawScaleFactor);

  return rawScaleFactor / 100;
}

static void getMonitorName(int index, char *name, char *displayName)
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

  strncpy(displayName, dd2.DeviceString, MONITOR_NAME_MAX);
  strncpy(name, ptr, MONITOR_NAME_MAX);
}

static void getMonitorBounds(
    HMONITOR monitor, Bounds *original, Bounds *client, Bounds *virtual
)
{
  MONITORINFOEX info;
  info.cbSize = sizeof(MONITORINFOEX);
  GetMonitorInfo(monitor, (LPMONITORINFO)&info);

  if (original)
  {
    original->x = info.rcMonitor.left;
    original->y = info.rcMonitor.top;
    original->w = info.rcMonitor.right - info.rcMonitor.left;
    original->h = info.rcMonitor.bottom - info.rcMonitor.top;
  }

  DEVMODE devmode;
  devmode.dmSize = sizeof(DEVMODE);
  EnumDisplaySettings(info.szDevice, ENUM_CURRENT_SETTINGS, &devmode);

  if (client)
  {
    client->x = devmode.dmPosition.x;
    client->y = devmode.dmPosition.y;
    client->w = devmode.dmPelsWidth;
    client->h = devmode.dmPelsHeight;
  }

  float scaleFactor = getScalingFactor(monitor);

  if (virtual)
  {
    virtual->x = devmode.dmPosition.x * scaleFactor - primaryX;
    virtual->y = devmode.dmPosition.y * scaleFactor - primaryY;
    virtual->w = devmode.dmPelsWidth * scaleFactor;
    virtual->h = devmode.dmPelsHeight * scaleFactor;
  }
}

static BOOL monitorenumproc_FindPrimaryCoords(
    HMONITOR monitor, HDC hdc, LPRECT rect, LPARAM param
)
{
  Bounds bounds;
  getMonitorBounds(monitor, NULL, &bounds, NULL);

  if (bounds.x < primaryX) primaryX = bounds.x;
  if (bounds.y < primaryY) primaryY = bounds.y;

  return TRUE;
}

static BOOL monitorenumproc_GetInfo(
    HMONITOR monitor, HDC hdc, LPRECT rect, LPARAM param
)
{
  MonitorInfo *mi = (MonitorInfo *)param + monitorEnumIndex;

  getMonitorBounds(
      monitor, &mi->pixelBounds, &mi->clientBounds, &mi->virtualBounds
  );
  getMonitorName(monitorEnumIndex, mi->name, mi->displayName);

  mi->config.loaded = 0;

  monitorEnumIndex++;
  return TRUE;
}
#endif

MonitorInfo *scanMonitors(int *count)
{
  MonitorInfo *m = NULL;

#ifdef __WIN32
  EnumDisplayMonitors(
      NULL, NULL, &monitorenumproc_FindPrimaryCoords, (LPARAM)NULL
  );

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
    snprintf(
        m[i].name, MONITOR_NAME_MAX, "%s", XGetAtomName(display, info->name)
    );
    strcpy(m[i].displayName, m[i].name);

    m[i].virtualBounds.x = info->x;
    m[i].virtualBounds.y = info->y;
    m[i].virtualBounds.w = info->width;
    m[i].virtualBounds.h = info->height;

    memcpy(&m[i].clientBounds, &m[i].virtualBounds, sizeof(Bounds));
    memcpy(&m[i].pixelBounds, &m[i].virtualBounds, sizeof(Bounds));

    m[i].config.loaded = 0;

    info++;
    i++;
  }

  *count = monitorCount;

#endif
  return m;
}
