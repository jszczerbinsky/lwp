#include "debug.h"
#include "main.h"

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

void initWindow(App *app, Config *cfg) {
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
}

#elif __DARWIN

// Helper macros for objc runtime interaction
#define msg_int ((id (*)(id, SEL, int))objc_msgSend)
#define msg_id  ((id (*)(id, SEL, id))objc_msgSend)
#define msg_ptr ((id (*)(id, SEL, void*))objc_msgSend)
#define msg_cls ((id (*)(Class, SEL))objc_msgSend)
#define msg_cls_chr ((id (*)(Class, SEL, char*))objc_msgSend)

void initWindow(App *app, Config *cfg) {
  // Get main display size
  const CGDirectDisplayID displayID = CGMainDisplayID();
  const size_t w = CGDisplayPixelsWide(displayID);
  const size_t h = CGDisplayPixelsHigh(displayID);
  const struct CGRect frameRect = { 0, 0, w, h };

  // Get shared NSApplication instance
  const id ns_app = msg_cls(objc_getClass("NSApplication"), sel_getUid("sharedApplication"));
  msg_int(ns_app, sel_getUid("setActivationPolicy:"), 0); // NSApplicationActivationPolicyRegular

  // Create NSWindow
  const id window = ((id (*)(id, SEL, struct CGRect, int, int, int))objc_msgSend)(
    msg_cls(objc_getClass("NSWindow"), sel_getUid("alloc")),
    sel_getUid("initWithContentRect:styleMask:backing:defer:"),
    frameRect,
    0, // NSWindowStyleMaskBorderless
    2, // NSBackingStoreBuffered
    false
  );

  // Set window attributes
  msg_id(
    window,
    sel_getUid("setTitle:"),
    msg_cls_chr(
      objc_getClass("NSString"),
      sel_getUid("stringWithUTF8String:"),
      "Parallax wallpaper"
    )
  );
  msg_ptr(window, sel_getUid("makeKeyAndOrderFront:"), nil);
  msg_int(window, sel_getUid("setLevel:"), kCGDesktopWindowLevel - 1);
  msg_int(ns_app, sel_getUid("activateIgnoringOtherApps:"), true);

  // Create SDL window from NSWindow
  app->window = SDL_CreateWindowFrom((void*) window);
  if (app->window == NULL) lwpLog(LOG_ERROR, "%s", SDL_GetError());
}

#elif __LINUX

void initWindow(App *app, Config *cfg) {
  Window rootWindow;

  if (cfg->reloadRootWnd)
  {
    Window originalRoot = RootWindow(app->display, DefaultScreen(app->display));
    rootWindow          = XCreateSimpleWindow(app->display, originalRoot, 0, 0,
                                              DisplayWidth(app->display, 0), DisplayHeight(app->display, 0),
                                              1, BlackPixel(app->display, 0), WhitePixel(app->display, 0));
    Atom atomType       = XInternAtom(app->display, "_NET_WM_WINDOW_TYPE", 0);
    Atom atomDesktop    = XInternAtom(app->display, "_NET_WM_WINDOW_TYPE_DESKTOP", 0);
    XChangeProperty(app->display, rootWindow, atomType, XA_ATOM, 32, PropModeReplace,
                    (const unsigned char *)&atomDesktop, 1);
    XMapWindow(app->display, rootWindow);
    XSync(app->display, 0);
  }
  else
  {
    rootWindow = RootWindow(app->display, DefaultScreen(app->display));
  }
  app->window = SDL_CreateWindowFrom((void *)rootWindow);
  if (app->window == NULL) lwpLog(LOG_ERROR, "%s", SDL_GetError());
}

#endif
