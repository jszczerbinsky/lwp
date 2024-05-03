#include "main.h"

#include "../platform_guard.h"

static App app;

static void atExit()
{
  for (int i = 0; i < app.monitorsCount; i++)
  {
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

  SDL_Quit();
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
            m->renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_TARGET,
            mi->clientBounds.w,
            mi->clientBounds.h
        );
        if (m->tex == NULL)
          lwpLog(LOG_ERROR, "Failed creating a texture for the monitor: %s", SDL_GetError());

        wallpaper->layers = malloc(sizeof(Layer) * wallpaper->info.config.layersCount);
        wallpaper->tex    = SDL_CreateTexture(
            m->renderer,
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

          wallpaper->layers[l].tex = SDL_CreateTextureFromSurface(m->renderer, surf);
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
  app->monitors         = calloc(app->monitorsCount, sizeof(Monitor));

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
          LOG_INFO, "Bounds: %d %d %dx%d", mi->clientBounds.x, mi->clientBounds.y, mi->clientBounds.w, mi->clientBounds.h
      );
      lwpLog(
          LOG_INFO,
          "Wallpaper destination bounds: %d %d %dx%d",
          mi->config.wlpBounds.x,
          mi->config.wlpBounds.y,
          mi->config.wlpBounds.w,
          mi->config.wlpBounds.h
      );

      initWindow(app, app->monitors + i);

      initWallpaper(app, app->monitors + i, wallpapers, wallpapersCount);
    }
  }

  free(wallpapers);
  free(monitors);
}

int main(int argc, char *argv[])
{
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
  signal(SIGINT, exitSignalHandler);
#endif

  lwpLog(LOG_INFO, "Starting Layered WallPaper");

  lwpLog(LOG_INFO, "Loading app config");
  loadAppConfig(&app.config);

  lwpLog(LOG_INFO, "Initializing SDL");
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, app.config.renderQuality);

  lwpLog(LOG_INFO, "Initializing monitors");
  initMonitors(&app);

  atexit(atExit);

  lwpLog(LOG_INFO, "Starting wallpaper loop");
  runWallpaperLoop(&app);

  return 0;
}
