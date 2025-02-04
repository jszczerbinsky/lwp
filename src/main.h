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

#define ANCHOR_TOPL 0
#define ANCHOR_TOPR 1
#define ANCHOR_CENT 2
#define ANCHOR_BOTL 3
#define ANCHOR_BOTR 4

// Simple behaviours - no arg required
#define BEHAVIOUR_BGFILL	0
#define BEHAVIOUR_BGFIT		1
#define BEHAVIOUR_BGSTRETCH 2
// Other behaviours - arg required
#define BEHAVIOUR_SHAKE		  3
#define BEHAVIOUR_FOLLOWMOUSE 4

#define CONTENT_NONE 0
#define CONTENT_IMG	 1
#define CONTENT_TEXT 2
#define CONTENT_ANIM 3
#define CONTENT_COL	 4

typedef struct {
	const Tex* tex;
} LayerContent_Img;

typedef struct {
	char*		str;
	Tex*		tmptex;
	RGBA		bg;
	RGBA		fg;
	const Font* font;
} LayerContent_Text;

typedef struct {
	// todo
} LayerContent_Anim;

typedef struct {
	// todo
} LayerContent_Col;

typedef union {
	LayerContent_Img  img;
	LayerContent_Text text;
	LayerContent_Anim anim;
	LayerContent_Col  col;
} LayerContent;

typedef struct {
	int	  behid;
	float farg;
} Behaviour;

typedef struct _Layer {

	Behaviour* behs;
	int		   behcnt;

	int			 contenttype;
	LayerContent content;

	int		anchor;
	BoundsF bounds;
	SizeF	scale;
	float	rot;

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
void	   layer_freecontent(Layer* layer);
void	   layer_setcontent(Layer* layer, int contenttype);
void	   layer_getrenbounds(Layer* layer, BoundsF* destbounds);
const Tex* layer_getrentex(Layer* layer);
void	   layer_addbehaviour(Layer* layer, int behaviour, float arg);

void layer_updatebehaviour(WlpInstance* inst, Layer* layer, float dt);

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

float lerp(float a, float b, float t);

#endif
