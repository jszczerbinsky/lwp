#include <X11/Xlib.h>
#include <SDL2/SDL.h>
#include <linux/limits.h>

void init(Display **display, SDL_Window **window, SDL_Renderer **renderer)
{
	*display = XOpenDisplay(NULL);	
	Window rootWindow = RootWindow(*display, DefaultScreen(*display));

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
		SDL_Log("%s", SDL_GetError());

	*window = SDL_CreateWindowFrom((void*)rootWindow);	
	if(*window == NULL)
		SDL_Log("%s", SDL_GetError());

	*renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(*renderer == NULL)
		SDL_Log("%s", SDL_GetError());

}

void loadTextures(SDL_Renderer *renderer, SDL_Texture **tex)
{
	char path[PATH_MAX];

	for(int i = 0; i < 3; i++)
	{
		sprintf(path, "%d.bmp", i);
		SDL_Surface *surf = SDL_LoadBMP(path);
		tex[i] = SDL_CreateTextureFromSurface(renderer, surf);
		SDL_FreeSurface(surf);
	}
}

int main(int argc, char *argv[])
{
	Display *display;
	SDL_Window *window;
	SDL_Renderer *renderer;

	SDL_Texture *tex[3];

	init(&display, &window, &renderer);
	loadTextures(renderer, tex);

	SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
	SDL_Event event;
	int quit = 0;
	while(!quit && SDL_WaitEvent(&event))
	{
		if(event.type == SDL_QUIT)
			quit = 1;
		else if(event.type == SDL_MOUSEMOTION || event.type == SDL_SYSWMEVENT)
		{
			SDL_RenderClear(renderer);
			for(int i = 0; i < 3; i++)
			{
				SDL_Rect src = {
					.x = 0,
					.y = 0,
					.w = 1920,
					.h = 1080
				};
				SDL_Rect dest = {
					.x = 0,
					.y = 0,
					.w = 1920,
					.h = 1080
				};

				SDL_RenderCopy(renderer, tex[i], &src, &dest);
			}
			SDL_RenderPresent(renderer);
		}
	}

	for(int i = 0; i < 3; i++)
		SDL_DestroyTexture(tex[i]);
	XCloseDisplay(display);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
