#ifndef INSTANCE_H
#define INSTANCE_H

#include <SDL3/SDL.h>

#include "assets.h"
#include "layer.h"

typedef struct {
	LogContext logctx;

	SDL_Window*	  sdl_wnd;
	SDL_Renderer* sdl_ren;

	Tex*   texs;
	Font*  fonts;
	Layer* layers;
} WlpInstance;

WlpInstance* instance_create(const LogContext* logctx);
void		 instance_run(WlpInstance* inst);
void		 instance_free(WlpInstance* inst);
void		 instance_load(WlpInstance* inst, const char* dir_path);

void  tex_load(WlpInstance* inst, const char* name, const char* path);
Tex*  tex_find(WlpInstance* inst, const char* name);
void  tex_free(Tex* tex);
void  font_load(WlpInstance* inst, const char* name, float ptsize,
				const char* path);
Font* font_find(WlpInstance* inst, const char* name);
void  font_free(Font* font);

// TODO change inst to bounds and move this func to layer.h
void layer_updatebehaviour(WlpInstance* inst, Layer* layer, float dt);

Layer* layer_spawnempty(WlpInstance* inst);
#endif
