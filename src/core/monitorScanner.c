#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

Monitor* scanMonitors(int *count)
{
    int monitorCount;
    Display *display = XOpenDisplay((getenv("DISPLAY")));
    Window   wnd = DefaultRootWindow(display);
    XRRMonitorInfo *info = XRRGetMonitors(display, wnd, 0, &monitorCount);

    Monitor *m = malloc(sizeof(Monitor) * monitorCount);

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
    
    return m;
}