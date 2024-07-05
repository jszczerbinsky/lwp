#include "main.h"

static App app = {0};

static void atExit()
{
#ifdef __LINUX
  printlog(LOG_INFO, "Closing X11 display...");
  if (app.display) XCloseDisplay(app.display);
#endif

  for (int i = 0; i < app.monitorsCount; i++)
  {
    printlog(LOG_INFO, "Freeing monitor %d...", i);
    Monitor *m = app.monitors + i;
    if (m->tex) SDL_DestroyTexture(m->tex);
    if (m->wlp.tex) SDL_DestroyTexture(m->wlp.tex);

    for (int l = 0; l < m->wlp.info.config.layersCount; l++)
      SDL_DestroyTexture(m->wlp.layers[l].tex);
    free(m->wlp.layers);

    if (m->renderer) SDL_DestroyRenderer(m->renderer);
    if (m->window) SDL_DestroyWindow(m->window);

    m++;
  }

  free(app.monitors);

  printlog(LOG_INFO, "Quitting SDL2...");
  SDL_Quit();

  printlog(LOG_INFO, "Done");
}

void exitSignalHandler(int s)
{
  printlog(LOG_INFO, "Terminating...");
  exit(0);
}

void initWallpaper(
    App *app, Monitor *m, WallpaperInfo *wallpapers, int wallpapersCount
)
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
        printlog(LOG_WARNING, "Could not load the wallpaper config");
      }
      else
      {
        Wallpaper *wallpaper = &m->wlp;

        printlog(
            LOG_INFO, "Initializing wallpaper %s...", wallpaper->info.name
        );
        printlog(
            LOG_INFO, "Layers count: %d", wallpaper->info.config.layersCount
        );
        printlog(
            LOG_INFO,
            "Repeat X Y: %d %d",
            wallpaper->info.config.repeatX,
            wallpaper->info.config.repeatY
        );

        m->tex = SDL_CreateTexture(
            m->renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_TARGET,
            mi->clientBounds.w,
            mi->clientBounds.h
        );
        if (m->tex == NULL)
          printlog(
              LOG_ERROR,
              "Failed creating a texture for the monitor: %s",
              SDL_GetError()
          );

        wallpaper->layers =
            malloc(sizeof(Layer) * wallpaper->info.config.layersCount);
        wallpaper->tex = SDL_CreateTexture(
            m->renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_TARGET,
            mi->config.wlpBounds.w,
            mi->config.wlpBounds.h
        );
        if (wallpaper->tex == NULL)
          printlog(
              LOG_ERROR,
              "Failed creating a texture for the monitor: %s",
              SDL_GetError()
          );

        for (int l = 0; l < wallpaper->info.config.layersCount; l++)
        {
          char path[PATH_MAX];
          snprintf(path, PATH_MAX, "%s/%d.bmp", wallpaper->info.dirPath, l + 1);

          SDL_Surface *surf = SDL_LoadBMP(path);
          if (!surf) printlog(LOG_ERROR, "File %s not found", path);

          if (l == 0)
          {
            wallpaper->originalW = surf->w;
            wallpaper->originalH = surf->h;
          }

          wallpaper->layers[l].tex =
              SDL_CreateTextureFromSurface(m->renderer, surf);
          if (wallpaper->tex == NULL)
            printlog(
                LOG_ERROR,
                "Failed creating a texture for the layer %d: %s",
                l,
                SDL_GetError()
            );

          SDL_FreeSurface(surf);
        }
      }
      foundWlp = 1;

      break;
    }
  }
  if (!foundWlp)
    printlog(LOG_WARNING, "Couldn't find the wallpaper. Ignoring...");
}

void initMonitors(App *app)
{
  MonitorInfo *monitors = scanMonitors(&app->monitorsCount);
  app->monitors         = calloc(app->monitorsCount, sizeof(Monitor));

  int            wallpapersCount;
  WallpaperInfo *wallpapers = scanWallpapers(&wallpapersCount);

  printlog(LOG_INFO, "Scanner found %d monitor(s)", app->monitorsCount);

  for (int i = 0; i < app->monitorsCount; i++)
  {
    memcpy(&app->monitors[i].info, monitors + i, sizeof(MonitorInfo));
    app->monitors[i].tex = NULL;

    MonitorInfo *mi = &app->monitors[i].info;

    app->monitors[i].currentPoint.x = 0;
    app->monitors[i].currentPoint.y = 0;

    if (!loadMonitorConfig(mi->name, &mi->config))
    {
      printlog(
          LOG_WARNING,
          "Couldn't find config file for monitor %s. Ignoring...",
          mi->name
      );
    }
    else
    {
      printlog(LOG_INFO, "Initializing monitor %d...", i);
      printlog(LOG_INFO, "Wallpaper: %s", mi->config.wlpName);
      printlog(
          LOG_INFO,
          "Bounds: %d %d %dx%d",
          mi->clientBounds.x,
          mi->clientBounds.y,
          mi->clientBounds.w,
          mi->clientBounds.h
      );
      printlog(
          LOG_INFO,
          "Wallpaper destination bounds: %d %d %dx%d",
          mi->config.wlpBounds.x,
          mi->config.wlpBounds.y,
          mi->config.wlpBounds.w,
          mi->config.wlpBounds.h
      );

      if (app->monitors[i].info.config.active)
      {
        initWindow(app, app->monitors + i);

        initWallpaper(app, app->monitors + i, wallpapers, wallpapersCount);
      }
      else
        app->monitors[i].aborted = 1;
    }
  }

  free(wallpapers);
  free(monitors);
}

int main(int argc, char *argv[])
{
  clearlog();

#ifdef __WIN32
  SetProcessDPIAware();
#endif

  memset(&app, 0, sizeof(App));

  char pidStr[10];
  sprintf(pidStr, "%d\0", getpid());
  fputs(pidStr, stdout);
  printf("\n");
  fflush(stdout);

#ifdef __WIN32
  signal(SIGTERM, exitSignalHandler);
#else
  signal(SIGKILL, exitSignalHandler);
#endif

  printlog(LOG_INFO, "Starting Layered WallPaper");

  printlog(LOG_INFO, "Loading app config");
  loadAppConfig(&app.config);

  printlog(LOG_INFO, "Initializing SDL");
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, app.config.renderQuality);

  printlog(LOG_INFO, "Initializing monitors");
  initMonitors(&app);

  atexit(atExit);

  printlog(LOG_INFO, "Starting wallpaper loop");

#ifdef __LINUX
  app.display = XOpenDisplay(NULL);
#endif
  runWallpaperLoop(&app);

  return 0;
}
