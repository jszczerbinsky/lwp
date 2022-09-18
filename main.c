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

void loadTextures(SDL_Renderer *renderer, SDL_Texture **tex, char *dir, int count)
{
	char path[PATH_MAX];

	for(int i = 0; i < count; i++)
	{
		sprintf(path, "%s/%d.bmp", dir, i+1);
		SDL_Surface *surf = SDL_LoadBMP(path);
		tex[i] = SDL_CreateTextureFromSurface(renderer, surf);
		SDL_FreeSurface(surf);
	}
}

int main(int argc, char *argv[])
{
	if(argc != 5)
	{
		printf("\nLayered Wallpaper Engine\n");
		printf("Usage:\n");
		printf("	lwp [screen width] [screen height] [layers count] [img dir]\n\n");
		return 0;
	}

	int width = atoi(argv[1]);
	int height = atoi(argv[2]);
	int count = atoi(argv[3]);

	char *dir = argv[4];

	Display *display;
	SDL_Window *window;
	SDL_Renderer *renderer;

	SDL_Texture *tex[count];

	init(&display, &window, &renderer);
	loadTextures(renderer, tex, dir, count);

	SDL_SetRelativeMouseMode(SDL_TRUE);

	SDL_Event event;
	int quit = 0;
	while(!quit)
	{
		int mx;

		while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_QUIT)
				quit = 1;
			else if(event.type == SDL_MOUSEMOTION)
				mx = event.motion.x;
		}

		SDL_RenderClear(renderer);

		for(int i = 0; i < count; i++)
		{
			SDL_Rect src = {
				.x = 0,
				.y = 0,
				.w = width,
				.h = height 
			};

			int x = 0-(mx/20)*i;

			for(int j = -1; j <= 1; j++)
			{
				SDL_Rect dest = {
					.x = x + j*width,
					.y = 0,
					.w = width,
					.h = height 
				};

				SDL_RenderCopy(renderer, tex[i], &src, &dest);
			}
		}
		SDL_RenderPresent(renderer);
		SDL_Delay(1000/60);
	}

	for(int i = 0; i < count; i++)
		SDL_DestroyTexture(tex[i]);
	XCloseDisplay(display);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
