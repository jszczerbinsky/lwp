#include <SDL2/SDL.h>

#include "main.h"

static int lerp(int a, int b, float t) {
  if (t > 1)
    t = 1;
  return (int)((float)a + (float)t * ((float)b - (float)a));
}

static float clamp(float a, float min, float max)
{
  if(a < min)
    return min;
  if(a > max)
    return max;
  return a;
}

static void getRelativeTargetPoint(Point *dest, Point *globalTargetPoint, Monitor *m)
{
  dest->x = globalTargetPoint->x - m->info.bounds.x;
  dest->y = globalTargetPoint->y - m->info.bounds.y;

  dest->x = clamp(dest->x, 0, m->info.bounds.w);
  dest->y = clamp(dest->y, 0, m->info.bounds.h);
}

static void renderMonitor(App *app, Monitor *monitor, Point *globalTargetPoint) {
  if (!monitor->info.config.loaded || !monitor->wlp.info.config.loaded)
    return;

  Point targetPoint;
  getRelativeTargetPoint(&targetPoint, globalTargetPoint, monitor);

  if (SDL_SetRenderTarget(monitor->renderer, monitor->wlp.tex) != 0) {
    lwpLog(LOG_ERROR, "Error setting the renderer target: %s", SDL_GetError());
    monitor->aborted = 1;
    return;
  }
  SDL_RenderClear(monitor->renderer);

  for (int i = 0; i < monitor->wlp.info.config.layersCount; i++) {
    SDL_Rect src = {
        .x = 0,
        .y = 0,
        .w = monitor->wlp.originalW,
        .h = monitor->wlp.originalH,
    };

    int x = -((targetPoint.x - monitor->info.bounds.w / 2) *
              monitor->wlp.info.config.layerConfigs[i].sensitivityX);
    int y = -((targetPoint.y - monitor->info.bounds.h / 2) *
              monitor->wlp.info.config.layerConfigs[i].sensitivityY);

    for (int k = -monitor->wlp.info.config.repeatY;
         k <= monitor->wlp.info.config.repeatY; k++) {
      for (int j = -monitor->wlp.info.config.repeatX;
           j <= monitor->wlp.info.config.repeatX; j++) {
        SDL_Rect dest = {
            .x = x + j * monitor->info.config.wlpBounds.w,
            .y = y + k * monitor->info.config.wlpBounds.h,
            .w = monitor->info.config.wlpBounds.w,
            .h = monitor->info.config.wlpBounds.h,
        };

        if (SDL_RenderCopy(monitor->renderer, monitor->wlp.layers[i].tex, &src,
                           &dest) != 0) {
          lwpLog(LOG_ERROR, "Error rendering copy: %s", SDL_GetError());
          monitor->aborted = 1;
        }
      }
    }
  }

  if (SDL_SetRenderTarget(monitor->renderer, monitor->tex) != 0) {
    lwpLog(LOG_ERROR, "Error setting the renderer target: %s", SDL_GetError());
    monitor->aborted = 1;
  }

  SDL_Rect src = {
      .x = 0,
      .y = 0,
      .w = monitor->info.config.wlpBounds.w,
      .h = monitor->info.config.wlpBounds.h,
  };

  SDL_Rect dest = {
      .x = monitor->info.config.wlpBounds.x,
      .y = monitor->info.config.wlpBounds.y,
      .w = monitor->info.config.wlpBounds.w,
      .h = monitor->info.config.wlpBounds.h,
  };

  if (SDL_RenderCopy(monitor->renderer, monitor->wlp.tex, &src, &dest) != 0) {
    lwpLog(LOG_ERROR, "Error rendering copy: %s", SDL_GetError());
    monitor->aborted = 1;
  }

  SDL_SetRenderTarget(monitor->renderer, NULL);

  SDL_Rect finalSrc = {
      .x = 0,
      .y = 0,
      .w = monitor->info.bounds.w,
      .h = monitor->info.bounds.h,
  };

  SDL_Rect finalDest = {
      .x = 0,
      .y = 0,
      .w = monitor->info.bounds.w,
      .h = monitor->info.bounds.h,
  };

  if (SDL_RenderCopy(monitor->renderer, monitor->tex, &finalSrc, &finalDest) != 0) {
    lwpLog(LOG_ERROR, "Error rendering copy: %s", SDL_GetError());
    monitor->aborted = 1;
  }

  SDL_RenderPresent(monitor->renderer);
  SDL_Delay(1000 / app->config.targetFps);
}

static void getInput(int *quit)
{
  SDL_Event event;

  while (SDL_PollEvent(&event))
    if (event.type == SDL_QUIT)
      (*quit) = 1;
}

static void getTargetPoint(Point *p)
{
  #ifdef __WIN32
      POINT mPos;
      GetCursorPos(&mPos);
      p->x = mPos.x - GetSystemMetrics(SM_XVIRTUALSCREEN);
      p->y = mPos.y - GetSystemMetrics(SM_YVIRTUALSCREEN);
  #else
      SDL_GetGlobalMouseState(&p->x, &p->y);
  #endif
}

static void lerpTargetPoint(Point *p, Point *target, float dT)
{
  p->x = lerp(p->x, target->x, dT * 4); // 4: smooth
  p->y = lerp(p->y, target->y, dT * 4);
}

void runWallpaperLoop(App *app) {
  Point targetPoint;
  Point point;

  int quit = 0;
  while (!quit) {
    static int lastTicks = 0;

    int ticks = SDL_GetTicks();
    float dT = (ticks - lastTicks) / 1000.0f;
    lastTicks = ticks;

    getInput(&quit);

    getTargetPoint(&targetPoint);
    lerpTargetPoint(&point, &targetPoint, dT);

    for (int m = 0; m < app->monitorsCount; m++)
      if(!app->monitors[m].aborted)
        renderMonitor(app, app->monitors + m, &point);
  }
}
