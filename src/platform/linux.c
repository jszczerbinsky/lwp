#include <SDL3/SDL.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <stdio.h>
#include <stdlib.h>

#include "../types/monitor.h"

void platform_init_wnd(SDL_Window** sdl_wnd, SDL_Renderer** sdl_ren) {
	*sdl_wnd = SDL_CreateWindow("JPaper", 1920, 1080, SDL_WINDOW_OPENGL);

	SDL_PropertiesID prop = SDL_GetWindowProperties(*sdl_wnd);

	Window xWnd =
		(Window)SDL_GetNumberProperty(prop, "SDL.window.x11.window", 0);
	Display* display = XOpenDisplay(NULL);
	Atom	 atomType = XInternAtom(display, "_NET_WM_WINDOW_TYPE", 0);
	Atom atomDesktop = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DESKTOP", 0);
	XChangeProperty(display, xWnd, atomType, XA_ATOM, 32, PropModeReplace,
					(const unsigned char*)&atomDesktop, 1);

	Window rootWindow = RootWindow(display, DefaultScreen(display));

	XReparentWindow(display, xWnd, rootWindow, 0, 0);

	XSync(display, 0);

	XCloseDisplay(display);

	*sdl_ren = SDL_CreateRenderer(*sdl_wnd, NULL);
}

Monitor* platform_scan_monitors(int* count) {
	int monitor_count;

	Display*		display = XOpenDisplay(0);
	Window			wnd = DefaultRootWindow(display);
	XRRMonitorInfo* info = XRRGetMonitors(display, wnd, 0, &monitor_count);

	Monitor* m = malloc(sizeof(Monitor) * monitor_count);

	int i = 0;
	while (i < monitor_count) {
		snprintf(m[i].name, MONITOR_NAME_MAX, "%s",
				 XGetAtomName(display, info->name));
		strcpy(m[i].display_name, m[i].name);

		m[i].bounds.x = info->x;
		m[i].bounds.y = info->y;
		m[i].bounds.w = info->width;
		m[i].bounds.h = info->height;

		info++;
		i++;
	}

	XCloseDisplay(display);
	*count = monitor_count;

	return m;
}
