#ifndef ASSETS_H
#define ASSETS_H

#include "geometry.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

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

#endif
