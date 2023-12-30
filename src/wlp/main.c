#include "main.h"

#include "debug.h"
#include "parser.h"
#include "platform_guard.h"
#include "wallpaper.h"
#include "window.h"

#ifdef __WIN32
static void initCmd()
{
  // Create console
  AllocConsole();
  AttachConsole(ATTACH_PARENT_PROCESS);
  freopen("CONOUT$", "w", stdout);
  HANDLE hOut   = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD  dwMode = 0;
  GetConsoleMode(hOut, &dwMode);
  SetConsoleMode(hOut, dwMode | 0x0004);

  // Remove closing button (because closing it closes the entire app)
  HWND  hwnd  = GetConsoleWindow();
  HMENU hMenu = GetSystemMenu(hwnd, FALSE);
  DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);

  // Set console title
  SetConsoleTitle("Layered WallPaper");
}
BOOL monitorenumproc(HMONITOR monitor, HDC hdc, LPRECT rect, LPARAM param)
{
  int px = GetSystemMetrics(SM_XVIRTUALSCREEN);
  int py = GetSystemMetrics(SM_YVIRTUALSCREEN);

  MONITORINFO monitorInfo;
  monitorInfo.cbSize = sizeof(MONITORINFO);

  GetMonitorInfo(monitor, &monitorInfo);
  lwpLog(
      LOG_INFO,
      "	Monitor: position %ldx%ld, size %ldx%ld %s",
      monitorInfo.rcMonitor.left - px,
      monitorInfo.rcMonitor.top - py,
      monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
      monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
      (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) == MONITORINFOF_PRIMARY ? "primary" : ""
  );

  return TRUE;
}
static void scanMonitors()
{
  lwpLog(LOG_INFO, "Scanning monitors...");
  EnumDisplayMonitors(NULL, NULL, &monitorenumproc, NULL);
}
#endif

int main(int argc, char *argv[])
{
  lwpLog(LOG_INFO, "Starting Layered WallPaper");

#ifdef __WIN32
  if (argc == 2 && strcmp(argv[1], "/console") == 0) initCmd();
  initTrayIcon();

  scanMonitors();
#endif

  Config cfg;
  parseConfig(&cfg);

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  initWindow(&cfg);

  cfg.renderer =
      SDL_CreateRenderer(cfg.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (cfg.renderer == NULL)
  {
    lwpLog(LOG_ERROR, "%s", SDL_GetError());
  }

  runWallpaperLoop(&cfg);


#ifdef __WIN32
  removeTrayIcon();
#endif

  freeConfig(&cfg);

  SDL_DestroyRenderer(cfg.renderer);
  SDL_DestroyWindow(cfg.window);
  SDL_Quit();

  return 0;
}
