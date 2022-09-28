#include "main.h"
#include "window.h"
#include "parser.h"
#include "debug.h"

App app;

int lerp(int a, int b, float t)
{
    return (int)((float)a + (float)t * ((float)b - (float)a));
}

void init()
{
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
		SDL_Log("%s", SDL_GetError());

	#ifndef __WIN32
		app.display = XOpenDisplay(NULL);	
	#endif

		initWindow();
	
		app.renderer = SDL_CreateRenderer(app.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(app.renderer == NULL)
		SDL_Log("%s", SDL_GetError());
}

int loadTextures(Config *cfg, SDL_Texture **tex)
{
	char path[PATH_MAX];

	for(int i = 0; i < cfg->count; i++)
	{
		sprintf(path, "%s/%d.bmp", cfg->path, i+1);
		SDL_Surface *surf = SDL_LoadBMP(path);
		if(!surf)
		{
			lwpLog(LOG_ERROR, "File %s not found", path);
			return 0;
		}
		if(i == 0)
		{
			app.srcWidth = surf->w;
			app.srcHeight = surf->h;
		}
		tex[i] = SDL_CreateTextureFromSurface(app.renderer, surf);
		SDL_FreeSurface(surf);
	}
	return 1;
}

int main(int argc, char *argv[])
{
	Config cfg;

	openConfig();
	if(!parseConfig(&cfg))
		return 1;
	
	Instance instances[cfg.monitors];
	SDL_Texture *tex[cfg.count];

	init();
	
	if(!loadTextures(&cfg, tex))
		return 1;

	parseInstancesConfig(instances, cfg.monitors);

	closeConfig();

	for(int i = 0; i < cfg.monitors; i++)
	{
		instances[i].buffTex = SDL_CreateTexture(
			app.renderer, 
			SDL_PIXELFORMAT_ARGB8888, 
			SDL_TEXTUREACCESS_TARGET, 
			instances[i].dest.w, 
			instances[i].dest.h
		);
	}

	#ifndef __WIN32
		SDL_SetRelativeMouseMode(SDL_TRUE);
	#endif

	SDL_Event event;
	int quit = 0;

	int mx = 0;
	int my = 0;

	while(!quit)
	{
		static int currentX = 0;
		static int currentY = 0;

		static int lastTicks = 0;

		int ticks = SDL_GetTicks();
		float dT = (ticks-lastTicks)/1000.0f;
		lastTicks = ticks;

		#ifdef __WIN32
			POINT mPos;
			GetCursorPos(&mPos);
			mx = mPos.x;
			my = mPos.y;
		#endif

		while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_QUIT)
				quit = 1;
			#ifndef __WIN32
				else if(event.type == SDL_MOUSEMOTION)
				{
					mx = event.motion.x;
					my = event.motion.y;
				}
			#endif
		}

		currentX = lerp(currentX, mx, dT*cfg.smooth);
		currentY = lerp(currentY, my, dT*cfg.smooth);

		for(int u = 0; u < cfg.monitors; u++)
		{
			SDL_SetRenderTarget(app.renderer, instances[u].buffTex);
			SDL_RenderClear(app.renderer);

			for(int i = 0; i < cfg.count; i++)
			{
				SDL_Rect src = {
					.x = 0,
					.y = 0,
					.w = app.srcWidth,
					.h = app.srcHeight 
				};

				int x = -((currentX-instances[u].dest.w/2)*cfg.movementX)*i;
				int y = -((currentY-instances[u].dest.h/2)*cfg.movementY)*i;

				for(int j = -1; j <= 1; j++)
				{
					SDL_Rect dest = {
						.x = x + j*instances[u].dest.w,
						.y = y,
						.w = instances[u].dest.w,
						.h = instances[u].dest.h 
					};

					SDL_RenderCopy(app.renderer, tex[i], &src, &dest);
				}
			}
		
			SDL_SetRenderTarget(app.renderer, NULL);
			SDL_Rect src = {
				.x = 0,
				.y = 0,
				.w = instances[u].dest.w,
				.h = instances[u].dest.h 
			};
		
			SDL_RenderCopy(app.renderer, instances[u].buffTex, &src, &instances[u].dest);
		}
		SDL_RenderPresent(app.renderer);
		SDL_Delay(1000/60);
	}

	for(int i = 0; i < cfg.count; i++)
		SDL_DestroyTexture(tex[i]);
	for(int i = 0; i < cfg.monitors; i++)
		SDL_DestroyTexture(instances[i].buffTex);
	#ifndef __WIN32
		XCloseDisplay(app.display);
	#endif
	SDL_DestroyRenderer(app.renderer);
	SDL_DestroyWindow(app.window);
	SDL_Quit();

	return 0;
}
