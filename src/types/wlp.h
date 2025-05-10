#ifndef INSTANCE_H
#define INSTANCE_H

#include <SDL3/SDL.h>
#include <libxml/xmlreader.h>

#include "assets.h"
#include "layer.h"

typedef struct {
	char* dir_path;
	char* name;
	char* author;
} WlpInfo;

typedef struct {
	LogContext logctx;

	WlpInfo wlp_info;

	SDL_Window*	  sdl_wnd;
	SDL_Renderer* sdl_ren;

	Tex*   texs;
	Font*  fonts;
	Layer* layers;
} WlpInstance;

void wlpcfg_parse_wallpaper(WlpInstance* inst, const char* dir_path,
							xmlNodePtr wallpaper_node);
void wlpcfg_parse_info(WlpInfo* info, xmlNodePtr info_node,
					   const char* dir_path);
int	 wlpcfg_validate(const char* config_path);

WlpInstance* instance_create(const LogContext* logctx);
void		 instance_run(WlpInstance* inst);
void		 instance_free(WlpInstance* inst);

int		 wlp_load_to_instance(WlpInstance* inst, const char* dir_path);
int		 wlp_load_info(WlpInfo* info, const char* dir_path);
void	 wlp_free_info_data(WlpInfo* info);
void	 wlp_subscan(int* count, WlpInfo** infos, const char* dir_path);

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
