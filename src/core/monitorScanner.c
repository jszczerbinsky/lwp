#include "main.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef __WIN32
#include <windows.h>
#else
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#endif

#ifdef __WIN32
int monitorEnumIndex = 0;

BOOL monitorenumproc(HMONITOR monitor, HDC hdc, LPRECT rect, LPARAM param)
{
    MONITORINFO info;
    info.cbSize = sizeof(MONITORINFO);

    GetMonitorInfo(monitor, &info);

    Monitor *m = (Monitor*)param;

    snprintf(m[monitorEnumIndex].name, MONITOR_NAME_MAX, "Monitor %d%s", monitorEnumIndex + 1, info.dwFlags == MONITORINFOF_PRIMARY ? " (main)" : "");
    m[monitorEnumIndex].bounds.x = info.rcWork.left;
    m[monitorEnumIndex].bounds.y = info.rcWork.top;
    m[monitorEnumIndex].bounds.w = info.rcWork.right - info.rcWork.left;
    m[monitorEnumIndex].bounds.h = info.rcWork.bottom - info.rcWork.top;
    m[monitorEnumIndex].config.set = 0;

    monitorEnumIndex++;
    return TRUE;
}
#endif

Monitor* scanMonitors(int *count)
{
    int monitorCount;
    Monitor *m = NULL;

#ifdef __WIN32
    monitorCount = GetSystemMetrics(SM_CMONITORS);
    m = malloc(sizeof(Monitor) * monitorCount);

    monitorEnumIndex = 0;
    EnumDisplayMonitors(NULL, NULL, &monitorenumproc, (LPARAM)m);

#else
    Display *display = XOpenDisplay((getenv("DISPLAY")));
    Window   wnd = DefaultRootWindow(display);
    XRRMonitorInfo *info = XRRGetMonitors(display, wnd, 0, &monitorCount);

    m = malloc(sizeof(Monitor) * monitorCount);

    int i = 0;
    while (i < monitorCount)
    {
        snprintf(m[i].name, MONITOR_NAME_MAX, "%s", XGetAtomName(display, info->name));
        m[i].bounds.x = info->x;
        m[i].bounds.y = info->y;
        m[i].bounds.w = info->width;
        m[i].bounds.h = info->height;
        m[i].config.set = 0;

        info++;
        i++;
    }

    *count = monitorCount;
    
#endif
    return m;
}