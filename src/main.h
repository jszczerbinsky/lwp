#ifndef MAIN_H
#define MAIN_H

#include <SDL3/SDL.h>
#include <linux/limits.h>
#include <lua.h>

#define DIR_SEP "/"

#define DICT_KEY_MAX 256
#define DICT_VAL_MAX 256

typedef struct _Dict {
	char key[DICT_KEY_MAX];
	char val[DICT_VAL_MAX];

	struct _Dict* next;
} Dict;

char* dict_get(Dict* dict, const char* key);
Dict* dict_add(Dict* dict, const char* key, const char* val);
void  dict_free(Dict* dict);
Dict* dict_read(const char* path);

typedef struct {
	int x;
	int y;
	int w;
	int h;
} Bounds;

typedef struct {
	float x;
	float y;
	float w;
	float h;
} BoundsF;

typedef struct {
	int w;
	int h;
} Size;

typedef struct {
	float w;
	float h;
} SizeF;

#define TEX_NAME_MAX 256

typedef struct _Tex {
	char		 name[TEX_NAME_MAX];
	SDL_Texture* sdl_tex;

	Size original_size;

	struct _Tex* next;
} Tex;

typedef struct _Layer {
	BoundsF bounds;
	float	rot;
	Tex*	current_tex;

	struct _Layer* next;
} Layer;

typedef struct {
	SDL_Window*	  sdl_wnd;
	SDL_Renderer* sdl_ren;

	lua_State* lua;

	Tex*   texs;
	Layer* layers;
} WlpInstance;

WlpInstance* instance_create();
void		 instance_run(WlpInstance* inst);
void		 instance_free(WlpInstance* inst);
void		 instance_load_wlp(WlpInstance* inst, const char* dir_path);

Layer* instance_spawn_layer(WlpInstance* inst, Tex* tex);

#define LOG_INFO	0
#define LOG_ERROR	1
#define LOG_WARNING 2

void printlog(int type, const char* str, ...);

void wlpapi_init(WlpInstance* inst, const char* path);

#endif
