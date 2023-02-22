#include "main.h"

#include "debug.h"
#include "parser.h"
#include "wallpaper.h"
#include "window.h"

static int lerp(int a, int b, float t)
{
  return (int)((float)a + (float)t * ((float)b - (float)a));
}

static int init(App *app, Config *cfg)
{
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
  {
    lwpLog(LOG_ERROR, "%s", SDL_GetError());
    return 0;
  }

#ifndef __WIN32
  app->display = XOpenDisplay(NULL);
#endif

  initWindow(app, cfg);

  app->renderer =
      SDL_CreateRenderer(app->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (app->renderer == NULL)
  {
    lwpLog(LOG_ERROR, "%s", SDL_GetError());
    return 0;
  }

  return 1;
}

#ifdef __WIN32
static void initCmd()
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

int main(int argc, char *argv[])
{
#ifdef __WIN32
  if (argc == 2 && strcmp(argv[1], "/console") == 0) initCmd();
#endif

  App    app;
  Config cfg;

  if (!parseConfig(&app, &cfg) || !init(&app, &cfg) || !loadMonitors(&app, &cfg)) return 1;

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

    for (int m = 0; m < cfg.monitorsCount; m++)
    {
      int relativeCurrentX = currentX - cfg.monitors[m].x;
      int relativeCurrentY = currentY - cfg.monitors[m].y;

      if (relativeCurrentX < 0) relativeCurrentX = 0;
      if (relativeCurrentY < 0) relativeCurrentY = 0;
      if (relativeCurrentX > cfg.monitors[m].w) relativeCurrentX = cfg.monitors[m].w;
      if (relativeCurrentY > cfg.monitors[m].h) relativeCurrentY = cfg.monitors[m].h;

      SDL_SetRenderTarget(app.renderer, cfg.monitors[m].wallpaper.tex);
      SDL_RenderClear(app.renderer);

      for (int i = 0; i < cfg.monitors[m].wallpaper.layersCount; i++)
      {
        SDL_Rect src = {
            .x = 0,
            .y = 0,
            .w = cfg.monitors[m].wallpaper.originalW,
            .h = cfg.monitors[m].wallpaper.originalH,
        };

        int x = -((relativeCurrentX - cfg.monitors[m].w / 2) *
                  cfg.monitors[m].wallpaper.layers[i].sensitivityX);
        int y = -((relativeCurrentY - cfg.monitors[m].h / 2) *
                  cfg.monitors[m].wallpaper.layers[i].sensitivityY);

        
		for(int k = -cfg.monitors[m].wallpaper.repeatY; k <= cfg.moitors[m].wallpaper.repeatY; k++)
		{
			for (int j = -cfg.monitors[m].wallpaper.repeatX; j <= cfg.monitors[m].wallpaper.repeatX; j++)
			{
			  SDL_Rect dest = {
				  .x = x + j * cfg.monitors[m].wallpaperW,
				  .y = y + k * cfg.monitors[m].wallpaperH,
				  .w = cfg.monitors[m].wallpaperW,
				  .h = cfg.monitors[m].wallpaperH,
			  };

			  SDL_RenderCopy(app.renderer, cfg.monitors[m].wallpaper.layers[i].tex, &src, &dest);
			}
		}
      }

      SDL_SetRenderTarget(app.renderer, cfg.monitors[m].tex);

      SDL_Rect src = {
          .x = 0,
          .y = 0,
          .w = cfg.monitors[m].wallpaperW,
          .h = cfg.monitors[m].wallpaperH,
      };

      SDL_Rect dest = {
          .x = cfg.monitors[m].wallpaperX,
          .y = cfg.monitors[m].wallpaperY,
          .w = cfg.monitors[m].wallpaperW,
          .h = cfg.monitors[m].wallpaperH,
      };

      SDL_RenderCopy(app.renderer, cfg.monitors[m].wallpaper.tex, &src, &dest);

      SDL_SetRenderTarget(app.renderer, NULL);

      SDL_Rect finalSrc = {
          .x = 0,
          .y = 0,
          .w = cfg.monitors[m].w,
          .h = cfg.monitors[m].h,
      };

      SDL_Rect finalDest = {
          .x = cfg.monitors[m].x,
          .y = cfg.monitors[m].y,
          .w = cfg.monitors[m].w,
          .h = cfg.monitors[m].h,
      };

      SDL_RenderCopy(app.renderer, cfg.monitors[m].tex, &finalSrc, &finalDest);
    }
    SDL_RenderPresent(app.renderer);
    SDL_Delay(1000 / 60);
  }

  freeConfig(&cfg);

#ifndef __WIN32
  XCloseDisplay(app.display);
#endif
  SDL_DestroyRenderer(app.renderer);
  SDL_DestroyWindow(app.window);
  SDL_Quit();

  return 0;
}
