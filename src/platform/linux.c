#include <SDL3/SDL.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <stdio.h>

void linux_init_wnd(SDL_Window** sdl_wnd, SDL_Renderer** sdl_ren) {
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
