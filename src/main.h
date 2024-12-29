#ifndef MAIN_H
#define MAIN_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
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
	int x;
	int y;
} Point;

typedef struct {
	float w;
	float h;
} SizeF;

typedef struct {
	float x;
	float y;
} PointF;

typedef struct {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
} RGBA;

#define FONT_NAME_MAX 256
typedef struct _Font {
	char	  name[FONT_NAME_MAX];
	TTF_Font* sdl_font;

	struct _Font* next;
} Font;

#define TEX_NAME_MAX 256
typedef struct _Tex {
	char		 name[TEX_NAME_MAX];
	SDL_Texture* sdl_tex;

	Size original_size;

	struct _Tex* next;
} Tex;

void tex_free(Tex* tex);

#define BOUNDS_FIXED	  0
#define BOUNDS_BG_FILL	  1
#define BOUNDS_BG_STRETCH 2

#define REN_NONE 0
#define REN_IMG	 1
#define REN_TEXT 2
#define REN_ANIM 3
#define REN_COL	 4

typedef struct {
	const Tex* tex;
} RenOpts_Img;

typedef struct {
	char*		str;
	Tex*		tmptex;
	RGBA		bg;
	RGBA		fg;
	const Font* font;
} RenOpts_Text;

typedef struct {
	// todo
} RenOpts_Anim;

typedef struct {
	// todo
} RenOpts_Col;

typedef union {
	RenOpts_Img	 img;
	RenOpts_Text text;
	RenOpts_Anim anim;
	RenOpts_Col	 col;
} RenOpts;

typedef struct _Layer {
	int boundstype;
	int rentype;

	RenOpts renopts;

	// =========================
	// settings for BOUNDS_FIXED
	// =========================
	BoundsF bounds;
	SizeF	scale;
	float	rot;
	// =========================

	SDL_Renderer* sdl_ren;

	struct _Layer* next;
} Layer;

typedef struct {
	SDL_Window*	  sdl_wnd;
	SDL_Renderer* sdl_ren;

	lua_State* lua;

	Tex*   texs;
	Font*  fonts;
	Layer* layers;
} WlpInstance;

WlpInstance* instance_create();
void		 instance_run(WlpInstance* inst);
void		 instance_free(WlpInstance* inst);
void		 instance_load_wlp(WlpInstance* inst, const char* dir_path);

void  tex_load(WlpInstance* inst, const char* name, const char* path);
Tex*  tex_find(WlpInstance* inst, const char* name);
void  tex_free(Tex* tex);
void  font_load(WlpInstance* inst, const char* name, const char* path);
Font* font_find(WlpInstance* inst, const char* name);
void  font_free(Font* font);

Layer*	   layer_spawnempty(WlpInstance* inst);
void	   layer_freerenopts(Layer* layer);
void	   layer_initas(Layer* layer, int rentype);
const Tex* layer_getcurrtex(Layer* layer);

void layer_text_settext(Layer* layer, const char* str);
void layer_text_setfg(Layer* layer, const RGBA* fg);
void layer_text_setbg(Layer* layer, const RGBA* bg);
void layer_text_setfont(Layer* layer, const Font* font);

void layer_img_settex(Layer* layer, const Tex* tex, int applybounds);

#define LOG_INFO	0
#define LOG_ERROR	1
#define LOG_WARNING 2

void printlog(int type, const char* str, ...);

void wlpapi_init(WlpInstance* inst, const char* path);

#endif
