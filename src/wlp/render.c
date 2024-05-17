#include <SDL2/SDL.h>

#include "main.h"

static int lerp(int a, int b, float t)
{
  if (t > 1) t = 1;
  return (int)((float)a + (float)t * ((float)b - (float)a));
}

static float clamp(float a, float min, float max)
{
  if (a < min) return min;
  if (a > max) return max;
  return a;
}

static void lerpTargetPoint(Point* p, Point* target, float dT)
{
  p->x = lerp(p->x, target->x, dT * 4);  // 4: smooth
  p->y = lerp(p->y, target->y, dT * 4);
}

static void getRelativeTargetPoint(
    Point* dest, const Point* globalTargetPoint, const Monitor* m
)
{
  dest->x = globalTargetPoint->x - m->info.clientBounds.x;
  dest->y = globalTargetPoint->y - m->info.clientBounds.y;
}

static void clampTargetPoint(Point* p, Monitor* m)
{
  p->x = clamp(p->x, 0, m->info.clientBounds.w);
  p->y = clamp(p->y, 0, m->info.clientBounds.h);
}

static void comeBackTargetPoint(Point* p, Monitor* m)
{
  if (p->x < 0 || p->x > m->info.clientBounds.w || p->y < 0 ||
      p->y > m->info.clientBounds.h)
  {
    p->x = m->info.clientBounds.w / 2;
    p->y = m->info.clientBounds.h / 2;
  }
}

static float distanceSquared(const Point* p, const Point* q)
{
  return (p->x - q->x) * (p->x - q->x) + (p->y - q->y) * (p->y - q->y);
}

static void renderMonitor(
    App* app, Monitor* monitor, const Point* globalTargetPoint, float dT,
    int firstRender
)
{
  if (!monitor->info.config.loaded || !monitor->wlp.info.config.loaded) return;

  Point targetPoint;
  getRelativeTargetPoint(&targetPoint, globalTargetPoint, monitor);

  if (app->config.unfocusedComeback)
    comeBackTargetPoint(&targetPoint, monitor);
  else
    clampTargetPoint(&targetPoint, monitor);

  lerpTargetPoint(&monitor->currentPoint, &targetPoint, dT);

  if (distanceSquared(&monitor->currentPoint, &targetPoint) < 1 && !firstRender)
    return;

  if (SDL_SetRenderTarget(monitor->renderer, monitor->wlp.tex) != 0)
  {
    printlog(
        LOG_ERROR, "Error setting the renderer target: %s", SDL_GetError()
    );
    monitor->aborted = 1;
    return;
  }
  SDL_RenderClear(monitor->renderer);

  for (int i = 0; i < monitor->wlp.info.config.layersCount; i++)
  {
    SDL_Rect src = {
        .x = 0,
        .y = 0,
        .w = monitor->wlp.originalW,
        .h = monitor->wlp.originalH,
    };

    int x =
        -((monitor->currentPoint.x - monitor->info.clientBounds.w / 2) *
          monitor->wlp.info.config.layerConfigs[i].sensitivityX);
    int y =
        -((monitor->currentPoint.y - monitor->info.clientBounds.h / 2) *
          monitor->wlp.info.config.layerConfigs[i].sensitivityY);

    for (int k = -monitor->wlp.info.config.repeatY;
         k <= monitor->wlp.info.config.repeatY;
         k++)
    {
      for (int j = -monitor->wlp.info.config.repeatX;
           j <= monitor->wlp.info.config.repeatX;
           j++)
      {
        SDL_Rect dest = {
            .x = x + j * monitor->info.config.wlpBounds.w,
            .y = y + k * monitor->info.config.wlpBounds.h,
            .w = monitor->info.config.wlpBounds.w,
            .h = monitor->info.config.wlpBounds.h,
        };

        if (SDL_RenderCopy(
                monitor->renderer, monitor->wlp.layers[i].tex, &src, &dest
            ) != 0)
        {
          printlog(LOG_ERROR, "Error rendering copy: %s", SDL_GetError());
          monitor->aborted = 1;
        }
      }
    }
  }

  if (SDL_SetRenderTarget(monitor->renderer, monitor->tex) != 0)
  {
    printlog(
        LOG_ERROR, "Error setting the renderer target: %s", SDL_GetError()
    );
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

  if (SDL_RenderCopy(monitor->renderer, monitor->wlp.tex, &src, &dest) != 0)
  {
    printlog(LOG_ERROR, "Error rendering copy: %s", SDL_GetError());
    monitor->aborted = 1;
  }

  SDL_SetRenderTarget(monitor->renderer, NULL);

  if (SDL_RenderCopy(monitor->renderer, monitor->tex, NULL, NULL) != 0)
  {
    printlog(LOG_ERROR, "Error rendering copy: %s", SDL_GetError());
    monitor->aborted = 1;
  }

  SDL_RenderPresent(monitor->renderer);
  SDL_Delay(1000 / app->config.targetFps);
}

static void getInput(int* quit)
{
  SDL_Event event;

  while (SDL_PollEvent(&event))
    if (event.type == SDL_QUIT) (*quit) = 1;
}

void runWallpaperLoop(App* app)
{
  Point targetPoint;

  int firstRender = 1;
  int quit        = 0;
  while (!quit)
  {
    static int lastTicks = 0;

    int   ticks = SDL_GetTicks();
    float dT    = (ticks - lastTicks) / 1000.0f;
    lastTicks   = ticks;

    getInput(&quit);

    getTargetPoint(app, &targetPoint);

    for (int m = 0; m < app->monitorsCount; m++)
    {
      if (!app->monitors[m].aborted)
        renderMonitor(app, app->monitors + m, &targetPoint, dT, firstRender);
    }
    firstRender = 0;
  }
}
