#include "main.h"

#include "../platform_guard.h"

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
BOOLmonitorenumproc(HMONITOR monitor, HDC hdc, LPRECT rect, LPARAM param)
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
/*
static void scanMonitors()
{
  lwpLog(LOG_INFO, "Scanning monitors...");
  EnumDisplayMonitors(NULL, NULL, &monitorenumproc, NULL);
}*/
#endif

static App app;

static void atExit()
{
  // freeConfig(&cfg);
  /*
    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);
    SDL_Quit();*/
}

void exitSignalHandler(int s)
{
  lwpLog(LOG_INFO, "Terminating...");
  exit(0);
}

void initWallpaper(App *app, Monitor *m, WallpaperInfo *wallpapers, int wallpapersCount)
{
  MonitorInfo *mi = &m->info;

  int foundWlp = 0;

  for (int j = 0; j < wallpapersCount; j++)
  {
    if (strcmp(mi->config.wlpName, wallpapers[j].name) == 0)
    {
      memcpy(&m->wlp.info, wallpapers + j, sizeof(WallpaperInfo));

      if (!loadWallpaperConfig(m->wlp.info.dirPath, &m->wlp.info.config))
      {
        lwpLog(LOG_WARNING, "Could not load the wallpaper config");
      }
      else
      {
        Wallpaper *wallpaper = &m->wlp;

        lwpLog(LOG_INFO, "Initializing wallpaper %s...", wallpaper->info.name);
        lwpLog(LOG_INFO, "Layers count: %d", wallpaper->info.config.layersCount);
        lwpLog(
            LOG_INFO,
            "Repeat X Y: %d %d",
            wallpaper->info.config.repeatX,
            wallpaper->info.config.repeatY
        );

        m->tex = SDL_CreateTexture(
            app->renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_TARGET,
            mi->bounds.w,
            mi->bounds.h
        );
        if (m->tex == NULL)
          lwpLog(LOG_ERROR, "Failed creating a texture for the monitor: %s", SDL_GetError());

        wallpaper->layers = malloc(sizeof(Layer) * wallpaper->info.config.layersCount);
        wallpaper->tex    = SDL_CreateTexture(
            app->renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_TARGET,
            mi->config.wlpBounds.w,
            mi->config.wlpBounds.h
        );
        if (wallpaper->tex == NULL)
          lwpLog(LOG_ERROR, "Failed creating a texture for the monitor: %s", SDL_GetError());

        for (int l = 0; l < wallpaper->info.config.layersCount; l++)
        {
          char path[PATH_MAX];
          snprintf(path, PATH_MAX, "%s/%d.bmp", wallpaper->info.dirPath, l + 1);

          SDL_Surface *surf = SDL_LoadBMP(path);
          if (!surf) lwpLog(LOG_ERROR, "File %s not found", path);

          if (l == 0)
          {
            wallpaper->originalW = surf->w;
            wallpaper->originalH = surf->h;
          }

          wallpaper->layers[l].tex = SDL_CreateTextureFromSurface(app->renderer, surf);
          if (wallpaper->tex == NULL)
            lwpLog(LOG_ERROR, "Failed creating a texture for the layer %d: %s", l, SDL_GetError());

          SDL_FreeSurface(surf);
        }
      }
      foundWlp = 1;

      break;
    }
  }
  if (!foundWlp) lwpLog(LOG_WARNING, "Couldn't find the wallpaper. Ignoring...");
}

int initMonitors(App *app)
{
  MonitorInfo *monitors = scanMonitors(&app->monitorsCount);
  app->monitors         = malloc(sizeof(Monitor) * app->monitorsCount);

  int            wallpapersCount;
  WallpaperInfo *wallpapers = scanWallpapers(&wallpapersCount);

  lwpLog(LOG_INFO, "Scanner found %d monitor(s)", app->monitorsCount);

  for (int i = 0; i < app->monitorsCount; i++)
  {
    memcpy(&app->monitors[i].info, monitors + i, sizeof(MonitorInfo));
    app->monitors[i].tex = NULL;

    MonitorInfo *mi = &app->monitors[i].info;

    if (!loadMonitorConfig(mi->name, &mi->config))
    {
      lwpLog(LOG_WARNING, "Couldn't find config file for monitor %s. Ignoring...", mi->name);
    }
    else
    {
      lwpLog(LOG_INFO, "Initializing monitor %d...", i);
      lwpLog(LOG_INFO, "Wallpaper: %s", mi->config.wlpName);
      lwpLog(
          LOG_INFO, "Bounds: %d %d %dx%d", mi->bounds.x, mi->bounds.y, mi->bounds.w, mi->bounds.h
      );
      lwpLog(
          LOG_INFO,
          "Wallpaper destination bounds: %d %d %dx%d",
          mi->config.wlpBounds.x,
          mi->config.wlpBounds.y,
          mi->config.wlpBounds.w,
          mi->config.wlpBounds.h
      );

      initWallpaper(app, app->monitors + i, wallpapers, wallpapersCount);
    }
  }

  free(wallpapers);
  free(monitors);
}

int main(int argc, char *argv[])
{
  char pidStr[10];
  sprintf(pidStr, "%d\0", getpid());
  fputs(pidStr, stdout);
  printf("\n");
  fflush(stdout);

#ifdef __WIN32
  signal(SIGTERM, exitSignalHandler);
#else
  signal(SIGINT, exitSignalHandler);
#endif

  lwpLog(LOG_INFO, "Starting Layered WallPaper");

#ifdef __WIN32
  if (argc == 2 && strcmp(argv[1], "/console") == 0) initCmd();
  initTrayIcon();

  // scanMonitors();
#endif

  lwpLog(LOG_INFO, "Loading app config");
  loadAppConfig(&app.config);

  lwpLog(LOG_INFO, "Initializing SDL");
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

  lwpLog(LOG_INFO, "Initializing window");
  initWindow(&app);

  lwpLog(LOG_INFO, "Initializing monitors");
  initMonitors(&app);

  atexit(atExit);

  lwpLog(LOG_INFO, "Starting wallpaper loop");
  runWallpaperLoop(&app);

#ifdef __WIN32
  removeTrayIcon();
#endif

  return 0;
}
