#include <SDL2/SDL.h>
#ifdef __WIN32
	#include <windows.h>
	#include <stdio.h>
	#include <SDL2/SDL_syswm.h>
#else
	#include <X11/Xlib.h>
	#include <linux/limits.h>
#endif

typedef struct
{
	SDL_Rect dest;
	SDL_Texture *buffTex;
} Instance;

#ifdef __WIN32
	HWND iconWorkerw;
	BOOL CALLBACK getIconWorkerw(HWND hWnd, LPARAM lParam)
	{
		char buff[10];
		GetClassName(hWnd, buff, 10);
		
		if(strcmp(buff, "WorkerW") == 0)
		{
			HWND defView = FindWindowEx(hWnd, NULL, "SHELLDLL_DefView", NULL);
			if(defView)
			{
				iconWorkerw = hWnd;
				return FALSE;
			}
		}
		return TRUE;
	}

	void init(SDL_Window **window, SDL_Renderer **renderer)
#else
	void init(Display **display, SDL_Window **window, SDL_Renderer **renderer)
#endif
{
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
		SDL_Log("%s", SDL_GetError());
	
	#ifdef __WIN32
		*window = SDL_CreateWindow("Parallax wallpaper", 0, 0, 0, 0, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
		if(*window == NULL)
			SDL_Log("%s", SDL_GetError());
		
		SDL_SysWMinfo sysWmInfo;
		SDL_VERSION(&sysWmInfo.version)
		SDL_GetWindowWMInfo(*window, &sysWmInfo);
		HWND hWindow = sysWmInfo.info.win.window;
		
		HWND progman = FindWindow("Progman", NULL);
		iconWorkerw = progman;
		SendMessageTimeout(progman, 0x052C, NULL, NULL, SMTO_NORMAL, 1000, NULL);
		if(!FindWindowEx(progman, NULL, "SHELLDLL_DefView", NULL))
			EnumWindows(getIconWorkerw, NULL);
		
		HWND wallpaperWorkerw = GetWindow(iconWorkerw, GW_HWNDNEXT);
		SetParent(hWindow, wallpaperWorkerw);
		SetWindowLongPtr(hWindow, GWL_EXSTYLE, WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOACTIVATE);
		SetWindowLongPtr(hWindow, GWL_STYLE, WS_CHILDWINDOW | WS_VISIBLE);
		
		SetWindowPos(hWindow, NULL,
			GetSystemMetrics(SM_XVIRTUALSCREEN),
			GetSystemMetrics(SM_YVIRTUALSCREEN),
			GetSystemMetrics(SM_CXVIRTUALSCREEN),
			GetSystemMetrics(SM_CYVIRTUALSCREEN),
			SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW
		);
	#else
		*display = XOpenDisplay(NULL);	
		Window rootWindow = RootWindow(*display, DefaultScreen(*display));
	
		*window = SDL_CreateWindowFrom((void*)rootWindow);	
		if(*window == NULL)
			SDL_Log("%s", SDL_GetError());
	#endif
	
	*renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(*renderer == NULL)
		SDL_Log("%s", SDL_GetError());
}

void loadTextures(SDL_Renderer *renderer, SDL_Texture **tex, char *dir, int count, int *srcWidth, int *srcHeight)
{
	char path[PATH_MAX];

	for(int i = 0; i < count; i++)
	{
		sprintf(path, "%s/%d.bmp", dir, i+1);
		SDL_Surface *surf = SDL_LoadBMP(path);
		if(i == 0)
		{
			*srcWidth = surf->w;
			*srcHeight = surf->h;
		}
		tex[i] = SDL_CreateTextureFromSurface(renderer, surf);
		SDL_FreeSurface(surf);
	}
}

int main(int argc, char *argv[])
{
	if((argc-3)%4 != 0 || argc < 7)
	{
		printf("\nLayered Wallpaper Engine\n");
		printf("Usage:\n");
		printf("	lwp [layers count] [img dir] [monitor1 options] [monitor2 options] ...\n\n");
		printf("Monitor options:\n");
		printf("	[x] [y] [width] [height]\n\n");
		return 0;
	}
	int instancesCount = (argc-2)/4;

	Instance instances[instancesCount];

	int count = atoi(argv[1]);
	char *dir = argv[2];

	int srcWidth, srcHeight;
	
	SDL_Window *window;
	SDL_Renderer *renderer;

	SDL_Texture *tex[count];

	#ifdef __WIN32
		init(&window, &renderer);
	#else
		Display *display;
		init(&display, &window, &renderer);
	#endif
	
	loadTextures(renderer, tex, dir, count, &srcWidth, &srcHeight);

	for(int i = 0; i < instancesCount; i++)
	{
		instances[i].dest.x = atoi(argv[3+i*4]);
		instances[i].dest.y = atoi(argv[4+i*4]);
		instances[i].dest.w = atoi(argv[5+i*4]);
		instances[i].dest.h = atoi(argv[6+i*4]);
		instances[i].buffTex = SDL_CreateTexture(
			renderer, 
			SDL_PIXELFORMAT_ARGB8888, 
			SDL_TEXTUREACCESS_TARGET, 
			instances[i].dest.w, 
			instances[i].dest.h
		);
	}

	SDL_SetRelativeMouseMode(SDL_TRUE);

	SDL_Event event;
	int quit = 0;
	while(!quit)
	{
		int mx;
		
		#ifdef __WIN32
			POINT mPos;
			GetCursorPos(&mPos);
			mx = mPos.x;
		#endif

		while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_QUIT)
				quit = 1;
			#ifndef __WIN32
				else if(event.type == SDL_MOUSEMOTION)
					mx = event.motion.x;
			#endif
		}

		for(int u = 0; u < instancesCount; u++)
		{
			SDL_SetRenderTarget(renderer, instances[u].buffTex);
			SDL_RenderClear(renderer);

			for(int i = 0; i < count; i++)
			{
				SDL_Rect src = {
					.x = 0,
					.y = 0,
					.w = srcWidth,
					.h = srcHeight 
				};

				int x = 0-(mx/20)*i;

				for(int j = -1; j <= 1; j++)
				{
					SDL_Rect dest = {
						.x = x + j*instances[u].dest.w,
						.y = 0,
						.w = instances[u].dest.w,
						.h = instances[u].dest.h 
					};

					SDL_RenderCopy(renderer, tex[i], &src, &dest);
				}
			}
		
			SDL_SetRenderTarget(renderer, NULL);
			SDL_Rect src = {
				.x = 0,
				.y = 0,
				.w = instances[u].dest.w,
				.h = instances[u].dest.h 
			};
		
			SDL_RenderCopy(renderer, instances[u].buffTex, &src, &instances[u].dest);
		}
		SDL_RenderPresent(renderer);
		SDL_Delay(1000/60);
	}

	for(int i = 0; i < count; i++)
		SDL_DestroyTexture(tex[i]);
	for(int i = 0; i < instancesCount; i++)
		SDL_DestroyTexture(instances[i].buffTex);
	#ifndef __WIN32
		XCloseDisplay(display);
	#endif
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
