#include "main.h"
#include <stdlib.h>

void tex_free(Tex* tex) {
	SDL_DestroyTexture(tex->sdl_tex);
	free(tex);
}

void tex_load(WlpInstance* inst, const char* name, const char* path) {

	const char* errstr = "Failed loading a texture from file %s";

	SDL_Surface* sdl_surf = SDL_LoadBMP(path);
	if (!sdl_surf) {
		printlog(LOG_ERROR, &inst->logctx, SDL_GetError(), errstr, path);
		return;
	}

	SDL_Texture* sdl_tex =
		SDL_CreateTextureFromSurface(inst->sdl_ren, sdl_surf);
	if (!sdl_tex) {
		printlog(LOG_ERROR, &inst->logctx, SDL_GetError(), errstr, path);
		SDL_DestroySurface(sdl_surf);
		return;
	}

	Tex* tex = malloc(sizeof(Tex));
	tex->sdl_tex = sdl_tex;
	tex->original_size.w = sdl_surf->w;
	tex->original_size.h = sdl_surf->h;
	strcpy(tex->name, name);

	tex->next = inst->texs;
	inst->texs = tex;

	SDL_DestroySurface(sdl_surf);
}

Tex* tex_find(WlpInstance* inst, const char* name) {
	if (!name) {
		return NULL;
	}

	Tex* tex = NULL;

	Tex* ptr = inst->texs;
	while (ptr && !tex) {
		if (strcmp(name, ptr->name) == 0) {
			tex = ptr;
		}
		ptr = ptr->next;
	}

	return tex;
}

void font_free(Font* font) {
	TTF_CloseFont(font->sdl_font);
	free(font);
}

void font_load(WlpInstance* inst, const char* name, float ptsize,
			   const char* path) {
	const char* errstr = "Failed loading a font from file %s";
	TTF_Font*	sdl_font = TTF_OpenFont(path, ptsize);
	if (!sdl_font) {
		printlog(LOG_ERROR, &inst->logctx, SDL_GetError(), errstr, path);
		return;
	}

	Font* font = malloc(sizeof(Font));
	font->sdl_font = sdl_font;
	strcpy(font->name, name);
	font->next = inst->fonts;
	inst->fonts = font;
}

Font* font_find(WlpInstance* inst, const char* name) {
	if (!name) {
		return NULL;
	}

	Font* font = NULL;

	Font* ptr = inst->fonts;
	while (ptr && !font) {
		if (strcmp(name, ptr->name) == 0) {
			font = ptr;
		}
		ptr = ptr->next;
	}

	return font;
}
