#include "main.h"

#ifdef __WIN32
#include <SDL2/SDL_syswm.h>
#endif  // __WIN32

#ifdef __LINUX
#include <SDL2/SDL_syswm.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#endif  //__LINUX

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

void initWindow(App *app, Monitor *monitor)
{
#ifdef __WIN32

  monitor->window = SDL_CreateWindow(
      "Parallax wallpaper", 0, 0, 0, 0, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN
  );
  if (monitor->window == NULL) printlog(LOG_ERROR, "%s", SDL_GetError());

  SDL_SysWMinfo sysWmInfo;
  SDL_VERSION(&sysWmInfo.version)
  SDL_GetWindowWMInfo(monitor->window, &sysWmInfo);
  HWND hWindow = sysWmInfo.info.win.window;

  HWND progman = FindWindow("Progman", NULL);
  iconWorkerw  = progman;
  SendMessageTimeout(progman, 0x052C, 0, 0, SMTO_NORMAL, 1000, NULL);
  if (!FindWindowEx(progman, NULL, "SHELLDLL_DefView", NULL))
    EnumWindows(getIconWorkerw, 0);

  HWND wallpaperWorkerw = GetWindow(iconWorkerw, GW_HWNDNEXT);
  SetParent(hWindow, wallpaperWorkerw);
  SetWindowLongPtr(
      hWindow,
      GWL_EXSTYLE,
      WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOACTIVATE
  );
  SetWindowLongPtr(hWindow, GWL_STYLE, WS_CHILDWINDOW | WS_VISIBLE);

  SetWindowPos(
      hWindow,
      NULL,
      monitor->info.virtualBounds.x,
      monitor->info.virtualBounds.y,
      monitor->info.virtualBounds.w,
      monitor->info.virtualBounds.h,
      SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW
  );

#else

  Display *display;

  monitor->window = SDL_CreateWindow(
      "Layered WallPaper",
      monitor->info.virtualBounds.x,
      monitor->info.virtualBounds.y,
      monitor->info.virtualBounds.w,
      monitor->info.virtualBounds.h,
      SDL_WINDOW_OPENGL
  );

  SDL_SysWMinfo wmInfo;
  SDL_GetVersion(&wmInfo.version);
  SDL_GetWindowWMInfo(monitor->window, &wmInfo);

  Window xWnd = wmInfo.info.x11.window;
  display     = wmInfo.info.x11.display;

  Atom atomType    = XInternAtom(display, "_NET_WM_WINDOW_TYPE", 0);
  Atom atomDesktop = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DESKTOP", 0);
  XChangeProperty(
      display,
      xWnd,
      atomType,
      XA_ATOM,
      32,
      PropModeReplace,
      (const unsigned char *)&atomDesktop,
      1
  );

  Window rootWindow = RootWindow(display, DefaultScreen(display));

  XReparentWindow(
      display,
      xWnd,
      rootWindow,
      monitor->info.virtualBounds.x,
      monitor->info.virtualBounds.y
  );

  XSync(display, 0);

#endif

  if (monitor->window == NULL)
    printlog(
        LOG_ERROR,
        "Failed to initialize window for monitor %s: %s",
        monitor->info.name,
        SDL_GetError()
    );

  monitor->renderer = SDL_CreateRenderer(
      monitor->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
  );
  if (monitor->renderer == NULL)
    printlog(
        LOG_ERROR,
        "Failed to initialize renderer for monitor %s: %s",
        monitor->info.name,
        SDL_GetError()
    );
}
