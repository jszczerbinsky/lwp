#include "debug.h"
#include "main.h"

#ifdef __WIN32
#include <shellapi.h>
#include <tchar.h>
#ifdef _MSC_VER
#include <SDL_syswm.h>
#else
#include <SDL2/SDL_syswm.h>
#endif  // _MSC_VER
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

#define WM_TRAY_ICON (WM_USER + 1)

static NOTIFYICONDATA nid;
static int            quit = 0;

void removeTrayIcon() { Shell_NotifyIcon(NIM_DELETE, &nid); }

int updateTrayIcon()
{
  MSG msg;
  while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return !quit;
}

static LRESULT CALLBACK wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    case WM_TRAY_ICON:
      if (lParam == WM_RBUTTONDOWN || lParam == WM_LBUTTONDOWN)
      {
        int res = MessageBox(NULL, "Do You want to run Layered WallPaper with debug console?",
                             "Restart Layered WallPaper", MB_YESNOCANCEL | MB_ICONQUESTION);

        TCHAR processParam = NULL;

        if (res != IDCANCEL)
        {
          TCHAR fileName[MAX_PATH];
          GetModuleFileName(NULL, fileName, MAX_PATH);

          TCHAR cmd[MAX_PATH + 10];
          _tcscpy(cmd, fileName);

          if (res == IDYES) _tcscat(cmd, " /console");

          STARTUPINFO si;
          memset(&si, 0, sizeof(STARTUPINFO));

          PROCESS_INFORMATION pi;

          CreateProcess(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

          quit = 1;
        }
      }
      break;
  }
}

void initTrayIcon()
{
  // Create an invisible window to process tray icon events

  HINSTANCE     hInstance    = GetModuleHandle(NULL);
  const wchar_t CLASS_NAME[] = L"Hidden Window";
  WNDCLASS      wc;
  memset(&wc, 0, sizeof(WNDCLASS));
  wc.lpfnWndProc   = wndProc;
  wc.hInstance     = hInstance;
  wc.lpszClassName = CLASS_NAME;
  RegisterClass(&wc);
  HWND hWnd = CreateWindowEx(0, CLASS_NAME, L"", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                             CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

  // Create tray icon

  nid.cbSize           = sizeof(NOTIFYICONDATA);
  nid.hWnd             = hWnd;
  nid.uCallbackMessage = WM_TRAY_ICON;
  nid.hIcon            = LoadIcon(hInstance, "ID");
  nid.uFlags           = NIF_ICON | NIF_MESSAGE;

  Shell_NotifyIcon(NIM_ADD, &nid);
}

void initWindow(App *app, Config *cfg)
{
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

  SetWindowPos(hWindow, NULL, 0, 0, 
							GetSystemMetrics(SM_CXVIRTUALSCREEN),
              GetSystemMetrics(SM_CYVIRTUALSCREEN),
              SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

#elif __DARWIN

// Helper macros for objc runtime interaction
#define OBJC_MSG_INT     ((id(*)(id, SEL, int))objc_msgSend)
#define OBJC_MSG_ID      ((id(*)(id, SEL, id))objc_msgSend)
#define OBJC_MSG_PTR     ((id(*)(id, SEL, void *))objc_msgSend)
#define OBJC_MSG_CLS     ((id(*)(Class, SEL))objc_msgSend)
#define OBJC_MSG_CLS_CHR ((id(*)(Class, SEL, char *))objc_msgSend)

void initWindow(App *app, Config *cfg)
{
  // Get main display size
  const CGDirectDisplayID displayID = CGMainDisplayID();
  const size_t            w         = CGDisplayPixelsWide(displayID);
  const size_t            h         = CGDisplayPixelsHigh(displayID);
  const struct CGRect     frameRect = {0, 0, w, h};

  // Get shared NSApplication instance
  const id ns_app = OBJC_MSG_CLS(objc_getClass("NSApplication"), sel_getUid("sharedApplication"));
  OBJC_MSG_INT(ns_app, sel_getUid("setActivationPolicy:"),
               0);  // NSApplicationActivationPolicyRegular

  // Create NSWindow
  const id window = ((id(*)(id, SEL, struct CGRect, int, int, int))objc_msgSend)(
      OBJC_MSG_CLS(objc_getClass("NSWindow"), sel_getUid("alloc")),
      sel_getUid("initWithContentRect:styleMask:backing:defer:"), frameRect,
      0,  // NSWindowStyleMaskBorderless
      2,  // NSBackingStoreBuffered
      false);

  // Set window attributes
  OBJC_MSG_ID(window, sel_getUid("setTitle:"),
              OBJC_MSG_CLS_CHR(objc_getClass("NSString"), sel_getUid("stringWithUTF8String:"),
                               "Parallax wallpaper"));
  OBJC_MSG_PTR(window, sel_getUid("makeKeyAndOrderFront:"), nil);
  OBJC_MSG_INT(window, sel_getUid("setLevel:"), kCGDesktopWindowLevel - 1);
  OBJC_MSG_INT(ns_app, sel_getUid("activateIgnoringOtherApps:"), true);

  // Create SDL window from NSWindow
  app->window = SDL_CreateWindowFrom((void *)window);
  if (app->window == NULL) lwpLog(LOG_ERROR, "%s", SDL_GetError());
}

#elif __LINUX

void initWindow(App *app, Config *cfg)
{
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
}
#endif
