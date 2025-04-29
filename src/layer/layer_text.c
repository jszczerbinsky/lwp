#include <stdlib.h>

#include "../types/assets.h"
#include "../types/instance.h"

void layer_text_setfont(Layer* layer, const Font* font) {
	layer->content.text.font = font;
}

void layer_text_setbg(Layer* layer, const RGBA* bg) {
	memcpy(&layer->content.text.bg, bg, sizeof(RGBA));
}

void layer_text_setfg(Layer* layer, const RGBA* fg) {
	memcpy(&layer->content.text.fg, fg, sizeof(RGBA));
}

void layer_text_settext(Layer* layer, const char* str) {
	LayerContent_Text* content = &layer->content.text;

	if (layer->content.text.str) {
		free(content->str);
		tex_free(content->tmptex);
	}

	SDL_Color sdl_fg = {content->fg.r, content->fg.g, content->fg.b,
						content->fg.a};
	SDL_Color sdl_bg = {content->bg.r, content->bg.g, content->bg.b,
						content->bg.a};

	SDL_Surface* sdl_surf =
		TTF_RenderText_Shaded(content->font->sdl_font, str, 0, sdl_fg, sdl_bg);

	const char* errstr = "Failed setting a text content";

	if (!sdl_surf) {
		printlog(LOG_ERROR, layer->logctx, SDL_GetError(), errstr);
		return;
	}

	SDL_Texture* sdl_tex =
		SDL_CreateTextureFromSurface(layer->sdl_ren, sdl_surf);
	if (!sdl_tex) {
		printlog(LOG_ERROR, layer->logctx, SDL_GetError(), errstr);
		SDL_DestroySurface(sdl_surf);
		return;
	}

	Tex* tex = malloc(sizeof(Tex));
	tex->sdl_tex = sdl_tex;
	tex->original_size.w = sdl_surf->w;
	tex->original_size.h = sdl_surf->h;
	tex->next = NULL;
	SDL_DestroySurface(sdl_surf);

	content->tmptex = tex;

	layer->bounds.w = tex->original_size.w;
	layer->bounds.h = tex->original_size.h;
}
