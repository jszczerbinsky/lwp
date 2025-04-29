#ifndef LAYER_H
#define LAYER_H

#include "assets.h"
#include "color.h"
#include "log.h"

#define ANCHOR_TOPL	   0
#define ANCHOR_TOPR	   1
#define ANCHOR_CENT	   2
#define ANCHOR_BOTL	   3
#define ANCHOR_BOTR	   4
#define ANCHOR_INVALID -1
int parseanchor(const char* str);

// Simple behaviours - no arg required
#define BEHAVIOUR_BGFILL	0
#define BEHAVIOUR_BGFIT		1
#define BEHAVIOUR_BGSTRETCH 2
// Other behaviours - arg required
#define BEHAVIOUR_SHAKE		  3
#define BEHAVIOUR_FOLLOWMOUSE 4
#define BEHAVIOUR_INVALID	  -1
int parsebehaviour(const char* str);

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
	int	  iarg;
} Behaviour;

typedef struct _Layer {
	const LogContext* logctx;

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

void	   layer_freecontent(Layer* layer);
void	   layer_setcontent(Layer* layer, int contenttype);
void	   layer_getrenbounds(Layer* layer, BoundsF* destbounds);
const Tex* layer_getrentex(Layer* layer);
void layer_addbehaviour(Layer* layer, int behaviour, float farg, int iarg);

void layer_text_settext(Layer* layer, const char* str);
void layer_text_setfg(Layer* layer, const RGBA* fg);
void layer_text_setbg(Layer* layer, const RGBA* bg);
void layer_text_setfont(Layer* layer, const Font* font);

void layer_img_settex(Layer* layer, const Tex* tex, int applybounds);

#endif
