#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../platform.h"
#include "../types/wlp.h"

#define LOOP_CONTINUE 0
#define LOOP_STOP	  1

WlpInstance* instance_create(const LogContext* logctx) {
	/*SDL_Window* wnd = SDL_CreateWindow("SDL3 Window", 800, 600, 0);

	if (wnd == NULL) {
		printlog(LOG_ERROR, logctx, SDL_GetError(),
				 "Failed to initialize SDL3 window");
		return NULL;
	}

	SDL_Renderer* ren = SDL_CreateRenderer(wnd, NULL);

	if (ren == NULL) {
		printlog(LOG_ERROR, logctx, SDL_GetError(),
				 "Failed to initialize SDL3 renderer");
		SDL_DestroyWindow(wnd);
		return NULL;
	}*/

	WlpInstance* inst = malloc(sizeof(WlpInstance));
	inst->texs = NULL;
	inst->fonts = NULL;
	inst->layers = NULL;

	platform_init_wnd(&inst->sdl_wnd, &inst->sdl_ren);

	memcpy(&inst->logctx, logctx, sizeof(LogContext));

	return inst;
}

void instance_free(WlpInstance* inst) {
	SDL_DestroyWindow(inst->sdl_wnd);
	SDL_DestroyRenderer(inst->sdl_ren);

	while (inst->layers) {
		Layer* next = inst->layers->next;
		layer_freecontent(inst->layers);
		for (int i = 0; i < inst->layers->behcnt; i++)
			free(inst->layers->behs + i);
		free(inst->layers);
		inst->layers = next;
	}

	while (inst->texs) {
		Tex* next = inst->texs->next;
		tex_free(inst->texs);
		inst->texs = next;
	}
}

static int instance_loop(WlpInstance* inst, float dt) {
	SDL_Event e;

	while (SDL_PollEvent(&e) != 0) {
		if (e.type == SDL_EVENT_QUIT) {
			return LOOP_STOP;
		}
	}

	SDL_SetRenderDrawColor(inst->sdl_ren, 255, 255, 255, 255);
	SDL_RenderClear(inst->sdl_ren);
	Layer* layer = inst->layers;

	while (layer) {
		layer_updatebehaviour(inst, layer, dt);
		const Tex* tex = layer_getrentex(layer);

		if (tex) {

			SDL_FRect sdlsrc = {
				.x = 0,
				.y = 0,
				.w = tex->original_size.w,
				.h = tex->original_size.h,
			};

			BoundsF dst;
			layer_getrenbounds(layer, &dst);

			SDL_FRect sdldst = {.x = dst.x, .y = dst.y, .w = dst.w, .h = dst.h};

			SDL_RenderTextureRotated(inst->sdl_ren, tex->sdl_tex, &sdlsrc,
									 &sdldst, layer->rot, NULL, SDL_FLIP_NONE);
		}
		layer = layer->next;
	}
	SDL_RenderPresent(inst->sdl_ren);
	return LOOP_CONTINUE;
}

void instance_run(WlpInstance* inst) {
	long long last_ticks = SDL_GetTicks();
	int		  quit = 0;
	while (!quit) {
		long long ticks = SDL_GetTicks();
		float	  dt = (ticks - last_ticks) / 1000.0f;
		last_ticks = ticks;
		if (instance_loop(inst, dt) == LOOP_STOP) {
			quit = 1;
		}
		SDL_Delay(1000 / 60);
	}
}
