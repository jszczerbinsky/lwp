#ifdef _MSC_VER
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include "debug.h"
#include "parser.h"

static int appplyWallpaper(Config *cfg, WallpaperDest *monitor, Wallpaper *wallpaper, const char *dirPath)
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

    wallpaper->layers[i].tex = SDL_CreateTextureFromSurface(cfg->renderer, surf);

    SDL_FreeSurface(surf);
  }

  wallpaper->tex =
      SDL_CreateTexture(cfg->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET,
                        monitor->wallpaperW, monitor->wallpaperH);

  monitor->tex = SDL_CreateTexture(cfg->renderer, SDL_PIXELFORMAT_ARGB8888,
                                   SDL_TEXTUREACCESS_TARGET, monitor->w, monitor->h);

  return 1;
}

int applyWallpapers(Config *cfg)
{
  for (int m = 0; m < cfg->monitorsCount; m++)
    if (!appplyWallpaper(cfg, &cfg->monitors[m], &cfg->monitors[m].wallpaper,
                     cfg->monitors[m].wallpaper.dirPath))
      return 0;

  return 1;
}

void freeMonitor(WallpaperDest *monitor)
{
  SDL_DestroyTexture(monitor->tex);
  SDL_DestroyTexture(monitor->wallpaper.tex);

  for (int i = 0; i < monitor->wallpaper.layersCount; i++)
  {
    SDL_DestroyTexture(monitor->wallpaper.layers[i].tex);
  }
  free(monitor->wallpaper.layers);
}

static int lerp(int a, int b, float t)
{
  if (t > 1) t = 1;
  return (int)((float)a + (float)t * ((float)b - (float)a));
}

void runWallpaperLoop(Config *cfg)
{
  int       quit = 0;

  if(!applyWallpapers(cfg))
  {
    lwpLog(LOG_ERROR, "Failed applying wallpapers");
    quit = 1;
  }

  SDL_Event event;

  int mx = 0;
  int my = 0;

  while (!quit)
  {
      static int currentX = 0;
      static int currentY = 0;

      static int lastTicks = 0;

      int   ticks = SDL_GetTicks();
      float dT    = (ticks - lastTicks) / 1000.0f;
      lastTicks   = ticks;

#ifdef __WIN32
      POINT mPos;
      GetCursorPos(&mPos);
      mx = mPos.x - GetSystemMetrics(SM_XVIRTUALSCREEN);
      my = mPos.y - GetSystemMetrics(SM_YVIRTUALSCREEN);
#else
      SDL_GetGlobalMouseState(&mx, &my);
#endif

      while (SDL_PollEvent(&event))
        if (event.type == SDL_QUIT) quit = 1;

      currentX = lerp(currentX, mx, dT * cfg->smooth);
      currentY = lerp(currentY, my, dT * cfg->smooth);

      for (int m = 0; m < cfg->monitorsCount; m++)
      {
        int relativeCurrentX = currentX - cfg->monitors[m].x;
        int relativeCurrentY = currentY - cfg->monitors[m].y;

        if (relativeCurrentX < 0) relativeCurrentX = 0;
        if (relativeCurrentY < 0) relativeCurrentY = 0;
        if (relativeCurrentX > cfg->monitors[m].w) relativeCurrentX = cfg->monitors[m].w;
        if (relativeCurrentY > cfg->monitors[m].h) relativeCurrentY = cfg->monitors[m].h;

        SDL_SetRenderTarget(cfg->renderer, cfg->monitors[m].wallpaper.tex);
        SDL_RenderClear(cfg->renderer);

        for (int i = 0; i < cfg->monitors[m].wallpaper.layersCount; i++)
        {
          SDL_Rect src = {
              .x = 0,
              .y = 0,
              .w = cfg->monitors[m].wallpaper.originalW,
              .h = cfg->monitors[m].wallpaper.originalH,
          };

          int x =
              -((relativeCurrentX - cfg->monitors[m].w / 2) *
                cfg->monitors[m].wallpaper.layers[i].sensitivityX);
          int y =
              -((relativeCurrentY - cfg->monitors[m].h / 2) *
                cfg->monitors[m].wallpaper.layers[i].sensitivityY);

          for (int k = -cfg->monitors[m].wallpaper.repeatY; k <= cfg->monitors[m].wallpaper.repeatY;
               k++)
          {
            for (int j = -cfg->monitors[m].wallpaper.repeatX; j <= cfg->monitors[m].wallpaper.repeatX;
                 j++)
            {
              SDL_Rect dest = {
                  .x = x + j * cfg->monitors[m].wallpaperW,
                  .y = y + k * cfg->monitors[m].wallpaperH,
                  .w = cfg->monitors[m].wallpaperW,
                  .h = cfg->monitors[m].wallpaperH,
              };

              SDL_RenderCopy(cfg->renderer, cfg->monitors[m].wallpaper.layers[i].tex, &src, &dest);
            }
          }
        }

        SDL_SetRenderTarget(cfg->renderer, cfg->monitors[m].tex);

        SDL_Rect src = {
            .x = 0,
            .y = 0,
            .w = cfg->monitors[m].wallpaperW,
            .h = cfg->monitors[m].wallpaperH,
        };

        SDL_Rect dest = {
            .x = cfg->monitors[m].wallpaperX,
            .y = cfg->monitors[m].wallpaperY,
            .w = cfg->monitors[m].wallpaperW,
            .h = cfg->monitors[m].wallpaperH,
        };

        SDL_RenderCopy(cfg->renderer, cfg->monitors[m].wallpaper.tex, &src, &dest);

        SDL_SetRenderTarget(cfg->renderer, NULL);

        SDL_Rect finalSrc = {
            .x = 0,
            .y = 0,
            .w = cfg->monitors[m].w,
            .h = cfg->monitors[m].h,
        };

        SDL_Rect finalDest = {
            .x = cfg->monitors[m].x,
            .y = cfg->monitors[m].y,
            .w = cfg->monitors[m].w,
            .h = cfg->monitors[m].h,
        };

        SDL_RenderCopy(cfg->renderer, cfg->monitors[m].tex, &finalSrc, &finalDest);
      }
      SDL_RenderPresent(cfg->renderer);
    SDL_Delay(1000 / cfg->targetFPS);
#ifdef __WIN32
    if (!updateTrayIcon()) quit = 1;
#endif
  }
}