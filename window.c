#include "main.h"
#include "parser.h"

#ifdef __WIN32
HWND          iconWorkerw;
BOOL CALLBACK getIconWorkerw(HWND hWnd, LPARAM lParam)
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
  if (app->window == NULL) SDL_Log("%s", SDL_GetError());

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
  Window rootWindow;

  if (cfg->reloadRootWnd)
  {
    Window originalRoot = RootWindow(app->display, DefaultScreen(app->display));
    rootWindow          = XCreateSimpleWindow(app->display, originalRoot, 0, 0,
                                              DisplayWidth(app->display, 0), DisplayHeight(app->display, 0),
                                              1, BlackPixel(app->display, 0), WhitePixel(app->display, 0));
    Atom atomType       = XInternAtom(app->display, "_NET_WM_WINDOW_TYPE", 0);
    Atom atomDesktop    = XInternAtom(app->display, "_NET_WM_WINDOW_TYPE_DESKTOP", 0);
    XChangeProperty(app->display, rootWindow, atomType, XA_ATOM, 32, PropModeReplace, &atomDesktop,
                    1);
    XMapWindow(app->display, rootWindow);
    XSync(app->display, 0);
  }
  else
  {
    rootWindow = RootWindow(app->display, DefaultScreen(app->display));
  }
  app->window = SDL_CreateWindowFrom((void *)rootWindow);
  if (app->window == NULL) SDL_Log("%s", SDL_GetError());

#endif
}
