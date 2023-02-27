#include "debug.h"
#include "main.h"

#ifndef __WIN32
#include <SDL2/SDL_syswm.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#endif

#ifdef __WIN32
static HWND          iconWorkerw;
static BOOL CALLBACK getIconWorkerw(HWND hWnd, LPARAM lParam)
{
  char buff[10];
  GetClassName(hWnd, buff, 10);

  if (strcmp(buff, "WorkerW") == 0)
  {
    HWND defView = FindWindowEx(hWnd, NULL, "SHELLDLL_DefView", NULL);
    if (defView)
    {
      iconWorkerw = hWnd;
      return FALSE;
    }
  }
  return TRUE;
}
#endif

void initWindow(App *app, Config *cfg)
{
#ifdef __WIN32
  app->window =
      SDL_CreateWindow("Parallax wallpaper", 0, 0, 0, 0, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
  if (app->window == NULL) lwpLog(LOG_ERROR, "%s", SDL_GetError());

  SDL_SysWMinfo sysWmInfo;
  SDL_VERSION(&sysWmInfo.version)
  SDL_GetWindowWMInfo(app->window, &sysWmInfo);
  HWND hWindow = sysWmInfo.info.win.window;

  HWND progman = FindWindow("Progman", NULL);
  iconWorkerw  = progman;
  SendMessageTimeout(progman, 0x052C, NULL, NULL, SMTO_NORMAL, 1000, NULL);
  if (!FindWindowEx(progman, NULL, "SHELLDLL_DefView", NULL)) EnumWindows(getIconWorkerw, NULL);

  HWND wallpaperWorkerw = GetWindow(iconWorkerw, GW_HWNDNEXT);
  SetParent(hWindow, wallpaperWorkerw);
  SetWindowLongPtr(hWindow, GWL_EXSTYLE,
                   WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOACTIVATE);
  SetWindowLongPtr(hWindow, GWL_STYLE, WS_CHILDWINDOW | WS_VISIBLE);

  SetWindowPos(hWindow, NULL, GetSystemMetrics(SM_XVIRTUALSCREEN),
               GetSystemMetrics(SM_YVIRTUALSCREEN), GetSystemMetrics(SM_CXVIRTUALSCREEN),
               GetSystemMetrics(SM_CYVIRTUALSCREEN),
               SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
#else

  if (cfg->reloadRootWnd)
  {
    Display *display = XOpenDisplay(NULL);
    XCloseDisplay(display);

    app->window = SDL_CreateWindow("Parallax wallpaper", 0, 0, DisplayWidth(display, 0),
                                   DisplayHeight(display, 0), SDL_WINDOW_OPENGL);

    SDL_SysWMinfo wmInfo;
    SDL_GetVersion(&wmInfo.version);
    SDL_GetWindowWMInfo(app->window, &wmInfo);

    Window xWnd = wmInfo.info.x11.window;
    display     = wmInfo.info.x11.display;

    Atom atomType    = XInternAtom(display, "_NET_WM_WINDOW_TYPE", 0);
    Atom atomDesktop = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DESKTOP", 0);
    XChangeProperty(display, xWnd, atomType, XA_ATOM, 32, PropModeReplace,
                    (const unsigned char *)&atomDesktop, 1);

    Window rootWindow = RootWindow(display, DefaultScreen(display));

    XReparentWindow(display, xWnd, rootWindow, 0, 0);

    XSync(display, 0);
  }
  else
  {
    Display *display    = XOpenDisplay(NULL);
    Window   rootWindow = RootWindow(display, DefaultScreen(display));
    app->window         = SDL_CreateWindowFrom((void *)rootWindow);
    XCloseDisplay(display);
  }

  if (app->window == NULL) lwpLog(LOG_ERROR, "%s", SDL_GetError());
#endif
}
