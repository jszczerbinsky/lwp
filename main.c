#include <SDL2/SDL.h>
#include <libconfig.h>
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

int lerp(int a, int b, float t)
{
    return (int)((float)a + (float)t * ((float)b - (float)a));
}

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

void loadTextures(SDL_Renderer *renderer, SDL_Texture **tex, const char *dir, int count, int *srcWidth, int *srcHeight)
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
	config_t config;
	config_init(&config);
	
	config_read_file(&config, "/home/cziken/.config/lwp/lwp.cfg");

	const char *dir = NULL;
	int count, instancesCount;

	config_lookup_string(&config, "path", &dir); 
	config_lookup_int(&config, "monitors", &instancesCount); 
	config_lookup_int(&config, "count", &count); 
	printf("%s %d %d\n", dir, count, instancesCount);
	fflush(stdout);

	Instance instances[instancesCount];

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
		char str[15];
		sprintf(str, "monitor%d_x", i+1);
		config_lookup_int(&config, str, &instances[i].dest.x);
		sprintf(str, "monitor%d_y", i+1);
		config_lookup_int(&config, str, &instances[i].dest.y);
		sprintf(str, "monitor%d_w", i+1);
		config_lookup_int(&config, str, &instances[i].dest.w);
		sprintf(str, "monitor%d_h", i+1);
		config_lookup_int(&config, str, &instances[i].dest.h);

		instances[i].buffTex = SDL_CreateTexture(
			renderer, 
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

		currentX = lerp(currentX, mx, dT*8);
		currentY = lerp(currentY, my, dT*8);

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

				int x = -((currentX-instances[u].dest.w/2)/20)*i;
				int y = -((currentY-instances[u].dest.h/2)/20)*i;

				for(int j = -1; j <= 1; j++)
				{
					SDL_Rect dest = {
						.x = x + j*instances[u].dest.w,
						.y = y,
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

	config_destroy(&config);
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
