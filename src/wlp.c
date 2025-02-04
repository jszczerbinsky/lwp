#include "main.h"
#include <SDL2/SDL_timer.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_rect.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <lua.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LOOP_CONTINUE 0
#define LOOP_STOP	  1

WlpInstance* instance_create() {
	SDL_Window* wnd = SDL_CreateWindow("SDL3 Window", 800, 600, 0);

	if (wnd == NULL) {
		printlog(LOG_ERROR,
				 "Failed to initialize SDL3 wnd - Internal error: %s",
				 SDL_GetError());
		return NULL;
	}

	SDL_Renderer* ren = SDL_CreateRenderer(wnd, NULL);

	if (ren == NULL) {
		printlog(LOG_ERROR,
				 "Failed to initialize SDL3 ren - Internal error: %s",
				 SDL_GetError());
		SDL_DestroyWindow(wnd);
		return NULL;
	}

	WlpInstance* inst = malloc(sizeof(WlpInstance));
	inst->sdl_wnd = wnd;
	inst->sdl_ren = ren;
	inst->texs = NULL;
	inst->fonts = NULL;
	inst->layers = NULL;
	inst->lua = NULL;

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

	if (inst->lua) {
		lua_close(inst->lua);
	}
}

static int instance_loop(WlpInstance* inst, float dt) {
	SDL_Event e;

	while (SDL_PollEvent(&e) != 0) {
		if (e.type == SDL_EVENT_QUIT) {
			return LOOP_STOP;
		}
	}

	if (inst->lua) {
		lua_getglobal(inst->lua, "onUpdate");
		lua_pushnumber(inst->lua, dt);
		if (lua_pcall(inst->lua, 1, 0, 0) != LUA_OK) {
			printlog(LOG_ERROR, "Error running onUpdate: %s\n",
					 lua_tostring(inst->lua, -1));
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
	if (inst->lua) {
		lua_getglobal(inst->lua, "onStart");
		if (lua_pcall(inst->lua, 0, 0, 0) != LUA_OK) {
			printlog(LOG_ERROR, "Error running onStart: %s\n",
					 lua_tostring(inst->lua, -1));
		}
	}

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

void instance_load(WlpInstance* inst, const char*);
void instance_load_wlp(WlpInstance* inst, const char* dir_path) {
	instance_load(inst, dir_path);
	/*
		char  path[PATH_MAX];
		Dict* ptr;

		sprintf(path, "%s%s%s", dir_path, DIR_SEP, "textures.cfg");
		Dict* tex_set = dict_read(path);
		ptr = tex_set;
		while (ptr) {
			printlog(LOG_INFO, "Loading texture '%s' from file: %s", ptr->key,
					 ptr->val);

			sprintf(path, "%s%s%s%s%s", dir_path, DIR_SEP, "assets", DIR_SEP,
					ptr->val);

			tex_load(inst, ptr->key, path);
			ptr = ptr->next;
		}

		sprintf(path, "%s%s%s", dir_path, DIR_SEP, "fonts.cfg");
		Dict* font_set = dict_read(path);
		ptr = font_set;
		while (ptr) {
			printlog(LOG_INFO, "Loading font '%s' from file: %s", ptr->key,
					 ptr->val);

			sprintf(path, "%s%s%s%s%s", dir_path, DIR_SEP, "assets", DIR_SEP,
					ptr->val);

			font_load(inst, ptr->key, path);
			ptr = ptr->next;
		}

		sprintf(path, "%s%s%s", dir_path, DIR_SEP, "main.lua");
		wlpapi_init(inst, path);

		dict_free(font_set);
		dict_free(tex_set);
		*/
}
