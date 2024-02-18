#include "main.h"

#ifdef __WIN32
#include <SDL2/SDL_syswm.h>
#endif // __WIN32

#ifdef __LINUX
#include <SDL2/SDL_syswm.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#endif //__LINUX

#ifdef __WIN32

static HWND iconWorkerw;
static BOOL CALLBACK getIconWorkerw(HWND hWnd, LPARAM lParam) {
  char buff[10];
  GetClassName(hWnd, buff, 10);

  if (strcmp(buff, "WorkerW") == 0) {
    HWND defView = FindWindowEx(hWnd, NULL, "SHELLDLL_DefView", NULL);
    if (defView) {
      iconWorkerw = hWnd;
      return FALSE;
    }
  }
  return TRUE;
}

#elif __DARWIN

// Helper macros for objc runtime interaction
#define OBJC_MSG_INT ((id(*)(id, SEL, int))objc_msgSend)
#define OBJC_MSG_ID ((id(*)(id, SEL, id))objc_msgSend)
#define OBJC_MSG_PTR ((id(*)(id, SEL, void *))objc_msgSend)
#define OBJC_MSG_CLS ((id(*)(Class, SEL))objc_msgSend)
#define OBJC_MSG_CLS_CHR ((id(*)(Class, SEL, char *))objc_msgSend)

void initWindow(Config *cfg) {
  // Get main display size
  const CGDirectDisplayID displayID = CGMainDisplayID();
  const size_t w = CGDisplayPixelsWide(displayID);
  const size_t h = CGDisplayPixelsHigh(displayID);
  const struct CGRect frameRect = {0, 0, w, h};

  // Get shared NScfglication instance
  const id ns_cfg = OBJC_MSG_CLS(objc_getClass("NScfglication"),
                                 sel_getUid("sharedcfglication"));
  OBJC_MSG_INT(ns_cfg, sel_getUid("setActivationPolicy:"),
               0); // NScfglicationActivationPolicyRegular

  // Create NSWindow
  const id window =
      ((id(*)(id, SEL, struct CGRect, int, int, int))objc_msgSend)(
          OBJC_MSG_CLS(objc_getClass("NSWindow"), sel_getUid("alloc")),
          sel_getUid("initWithContentRect:styleMask:backing:defer:"), frameRect,
          0, // NSWindowStyleMaskBorderless
          2, // NSBackingStoreBuffered
          false);

  // Set window attributes
  OBJC_MSG_ID(window, sel_getUid("setTitle:"),
              OBJC_MSG_CLS_CHR(objc_getClass("NSString"),
                               sel_getUid("stringWithUTF8String:"),
                               "Parallax wallpaper"));
  OBJC_MSG_PTR(window, sel_getUid("makeKeyAndOrderFront:"), nil);
  OBJC_MSG_INT(window, sel_getUid("setLevel:"), kCGDesktopWindowLevel - 1);
  OBJC_MSG_INT(ns_cfg, sel_getUid("activateIgnoringOthercfgs:"), true);

  // Create SDL window from NSWindow
  cfg->window = SDL_CreateWindowFrom((void *)window);
  if (cfg->window == NULL)
    lwpLog(LOG_ERROR, "%s", SDL_GetError());
}
#endif

void initWindow(App *app) {
#ifdef __WIN32

  app->window = SDL_CreateWindow("Parallax wallpaper", 0, 0, 0, 0,
                                 SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
  if (app->window == NULL)
    lwpLog(LOG_ERROR, "%s", SDL_GetError());

  SDL_SysWMinfo sysWmInfo;
  SDL_VERSION(&sysWmInfo.version)
  SDL_GetWindowWMInfo(app->window, &sysWmInfo);
  HWND hWindow = sysWmInfo.info.win.window;

  HWND progman = FindWindow("Progman", NULL);
  iconWorkerw = progman;
  SendMessageTimeout(progman, 0x052C, NULL, NULL, SMTO_NORMAL, 1000, NULL);
  if (!FindWindowEx(progman, NULL, "SHELLDLL_DefView", NULL))
    EnumWindows(getIconWorkerw, NULL);

  HWND wallpaperWorkerw = GetWindow(iconWorkerw, GW_HWNDNEXT);
  SetParent(hWindow, wallpaperWorkerw);
  SetWindowLongPtr(hWindow, GWL_EXSTYLE,
                   WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR |
                       WS_EX_NOACTIVATE);
  SetWindowLongPtr(hWindow, GWL_STYLE, WS_CHILDWINDOW | WS_VISIBLE);

  SetWindowPos(hWindow, NULL, 0, 0, GetSystemMetrics(SM_CXVIRTUALSCREEN),
               GetSystemMetrics(SM_CYVIRTUALSCREEN),
               SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);

#elif __DARWIN

#else

  if (!app->config.drawOnRootWindow) {
    Display *display = XOpenDisplay(NULL);
    XCloseDisplay(display);

    app->window =
        SDL_CreateWindow("Layered WallPaper", 0, 0, DisplayWidth(display, 0),
                         DisplayHeight(display, 0), SDL_WINDOW_OPENGL);

    SDL_SysWMinfo wmInfo;
    SDL_GetVersion(&wmInfo.version);
    SDL_GetWindowWMInfo(app->window, &wmInfo);

    Window xWnd = wmInfo.info.x11.window;
    display = wmInfo.info.x11.display;

    Atom atomType = XInternAtom(display, "_NET_WM_WINDOW_TYPE", 0);
    Atom atomDesktop = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DESKTOP", 0);
    XChangeProperty(display, xWnd, atomType, XA_ATOM, 32, PropModeReplace,
                    (const unsigned char *)&atomDesktop, 1);

    Window rootWindow = RootWindow(display, DefaultScreen(display));

    XReparentWindow(display, xWnd, rootWindow, 0, 0);

    XSync(display, 0);
  } else {
    Display *display = XOpenDisplay(NULL);
    Window rootWindow = RootWindow(display, DefaultScreen(display));
    app->window = SDL_CreateWindowFrom((void *)rootWindow);
    XCloseDisplay(display);
  }

#endif

  if (app->window == NULL)
    lwpLog(LOG_ERROR, "Failed to initialize window: %s", SDL_GetError());

  app->renderer = SDL_CreateRenderer(
      app->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (app->renderer == NULL)
    lwpLog(LOG_ERROR, "Failed to initialize renderer: %s", SDL_GetError());
}