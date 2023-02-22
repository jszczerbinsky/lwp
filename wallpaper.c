#include "debug.h"
#include "parser.h"

static int loadMonitor(App *app, Monitor *monitor, Wallpaper *wallpaper, const char *dirPath)
{
  char path[PATH_MAX];

  snprintf(path, PATH_MAX, "%s/wallpaper.cfg", dirPath);

  if (!parseWallpaperConfig(wallpaper, path))
  {
    return 0;
  }

  for (int i = 0; i < wallpaper->layersCount; i++)
  {
    snprintf(path, PATH_MAX, "%s/%d.bmp", dirPath, i + 1);

    SDL_Surface *surf = SDL_LoadBMP(path);
    if (!surf)
    {
      lwpLog(LOG_ERROR, "File %s not found", path);
      return 0;
    }

    if (i == 0)
    {
      wallpaper->originalW = surf->w;
      wallpaper->originalH = surf->h;
    }

    wallpaper->layers[i].tex = SDL_CreateTextureFromSurface(app->renderer, surf);

    SDL_FreeSurface(surf);
  }

  wallpaper->tex =
      SDL_CreateTexture(app->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET,
                        monitor->wallpaperW, monitor->wallpaperH);

  monitor->tex = SDL_CreateTexture(app->renderer, SDL_PIXELFORMAT_ARGB8888,
                                   SDL_TEXTUREACCESS_TARGET, monitor->w, monitor->h);

  return 1;
}

int loadMonitors(App *app, Config *cfg)
{
  for (int m = 0; m < cfg->monitorsCount; m++)
    if (!loadMonitor(app, &cfg->monitors[m], &cfg->monitors[m].wallpaper,
                     cfg->monitors[m].wallpaper.dirPath))
      return 0;

  return 1;
}

void freeMonitor(Monitor *monitor)
{
  SDL_DestroyTexture(monitor->tex);
  SDL_DestroyTexture(monitor->wallpaper.tex);

  for (int i = 0; i < monitor->wallpaper.layersCount; i++)
  {
    SDL_DestroyTexture(monitor->wallpaper.layers[i].tex);
  }
  free(monitor->wallpaper.layers);
}
