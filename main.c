#include "main.h"

#include "debug.h"
#include "parser.h"
#include "window.h"

App app;

int lerp(int a, int b, float t) { return (int)((float)a + (float)t * ((float)b - (float)a)); }

void init(Config *cfg)
{
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) SDL_Log("%s", SDL_GetError());

#ifndef __WIN32
  app.display = XOpenDisplay(NULL);
#endif

  initWindow(&app, cfg);

  app.renderer =
      SDL_CreateRenderer(app.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (app.renderer == NULL) SDL_Log("%s", SDL_GetError());
}

int loadTextures(Config *cfg, SDL_Texture **tex)
{
  char path[PATH_MAX];

  for (int i = 0; i < cfg->count; i++)
  {
    sprintf(path, "%s/%d.bmp", cfg->path, i + 1);
    SDL_Surface *surf = SDL_LoadBMP(path);
    if (!surf)
    {
      lwpLog(LOG_ERROR, "File %s not found", path);
      return 0;
    }
    if (i == 0)
    {
      app.srcWidth  = surf->w;
      app.srcHeight = surf->h;
    }
    tex[i] = SDL_CreateTextureFromSurface(app.renderer, surf);
    SDL_FreeSurface(surf);
  }
  return 1;
}

int main(int argc, char *argv[])
{
#ifdef __WIN32
  if (argc == 2 && strcmp(argv[1], "/console") == 0)
  {
    AllocConsole();
    AttachConsole(ATTACH_PARENT_PROCESS);
    freopen("CONOUT$", "w", stdout);
    HANDLE hOut   = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD  dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    SetConsoleMode(hOut, dwMode | 0x0004);
  }
#endif
  Config cfg;

  openConfig();
  if (!parseConfig(&cfg)) return 1;

  Instance     instances[cfg.monitors];
  SDL_Texture *tex[cfg.count];

  init(&cfg);

  if (!loadTextures(&cfg, tex)) return 1;

  parseInstancesConfig(instances, cfg.monitors);
  float layerMovX[cfg.count];
  float layerMovY[cfg.count];

  parsePerLayerMovements(layerMovX, layerMovY, cfg.count, cfg.movementX, cfg.movementY);

  closeConfig();

  for (int i = 0; i < cfg.monitors; i++)
  {
    instances[i].buffTex =
        SDL_CreateTexture(app.renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET,
                          instances[i].dest.w, instances[i].dest.h);
    instances[i].finalTex =
        SDL_CreateTexture(app.renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET,
                          instances[i].finalDest.w, instances[i].finalDest.h);
  }

  SDL_Event event;
  int       quit = 0;

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
    mx = mPos.x;
    my = mPos.y;
#else
    SDL_GetGlobalMouseState(&mx, &my);
#endif

    while (SDL_PollEvent(&event))
      if (event.type == SDL_QUIT) quit = 1;

    currentX = lerp(currentX, mx, dT * cfg.smooth);
    currentY = lerp(currentY, my, dT * cfg.smooth);

    for (int u = 0; u < cfg.monitors; u++)
    {
      int relativeCurrentX = currentX - instances[u].finalDest.x;
      int relativeCurrentY = currentY - instances[u].finalDest.y;

      if (relativeCurrentX < 0) relativeCurrentX = 0;
      if (relativeCurrentY < 0) relativeCurrentY = 0;
      if (relativeCurrentX > instances[u].finalDest.w) relativeCurrentX = instances[u].finalDest.w;
      if (relativeCurrentY > instances[u].finalDest.y) relativeCurrentY = instances[u].finalDest.h;

      SDL_SetRenderTarget(app.renderer, instances[u].buffTex);
      SDL_RenderClear(app.renderer);

      for (int i = 0; i < cfg.count; i++)
      {
        SDL_Rect src = {.x = 0, .y = 0, .w = app.srcWidth, .h = app.srcHeight};

        int x = -((relativeCurrentX - instances[u].dest.w / 2) * layerMovX[i]) * i;
        int y = -((relativeCurrentY - instances[u].dest.h / 2) * layerMovY[i]) * i;

        for (int j = -1; j <= 1; j++)
        {
          SDL_Rect dest = {.x = x + j * instances[u].dest.w,
                           .y = y,
                           .w = instances[u].dest.w,
                           .h = instances[u].dest.h};

          SDL_RenderCopy(app.renderer, tex[i], &src, &dest);
        }
      }

      SDL_SetRenderTarget(app.renderer, instances[u].finalTex);
      SDL_Rect src = {
          .x = 0,
          .y = 0,
          .w = instances[u].dest.w,
          .h = instances[u].dest.h,
      };

      SDL_RenderCopy(app.renderer, instances[u].buffTex, &src, &instances[u].dest);

      SDL_SetRenderTarget(app.renderer, NULL);
      SDL_Rect finalSrc = {
          .x = 0,
          .y = 0,
          .w = instances[u].finalDest.w,
          .h = instances[u].finalDest.h,
      };

      SDL_RenderCopy(app.renderer, instances[u].finalTex, &finalSrc, &instances[u].finalDest);
    }
    SDL_RenderPresent(app.renderer);
    SDL_Delay(1000 / 60);
  }

  for (int i = 0; i < cfg.count; i++) SDL_DestroyTexture(tex[i]);
  for (int i = 0; i < cfg.monitors; i++)
  {
    SDL_DestroyTexture(instances[i].buffTex);
    SDL_DestroyTexture(instances[i].finalTex);
  }
#ifndef __WIN32
  XCloseDisplay(app.display);
#endif
  SDL_DestroyRenderer(app.renderer);
  SDL_DestroyWindow(app.window);
  SDL_Quit();

  return 0;
}
