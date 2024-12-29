#include "../main.h"
#include <stdlib.h>

void layer_text_setfont(Layer* layer, const Font* font) {
	layer->renopts.text.font = font;
}

void layer_text_setbg(Layer* layer, const RGBA* bg) {
	memcpy(&layer->renopts.text.bg, bg, sizeof(RGBA));
}

void layer_text_setfg(Layer* layer, const RGBA* fg) {
	memcpy(&layer->renopts.text.fg, fg, sizeof(RGBA));
}

void layer_text_settext(Layer* layer, const char* str) {
	RenOpts_Text* renopts = &layer->renopts.text;

	if (layer->renopts.text.str) {
		free(renopts->str);
		tex_free(renopts->tmptex);
	}

	SDL_Color sdl_fg = {renopts->fg.r, renopts->fg.g, renopts->fg.b,
						renopts->fg.a};
	SDL_Color sdl_bg = {renopts->bg.r, renopts->bg.g, renopts->bg.b,
						renopts->bg.a};

	SDL_Surface* sdl_surf =
		TTF_RenderText_Shaded(renopts->font->sdl_font, str, 0, sdl_fg, sdl_bg);

	if (!sdl_surf) {
		printlog(LOG_ERROR, "Counldn't render text - internal error: %s",
				 SDL_GetError());
		return;
	}

	SDL_Texture* sdl_tex =
		SDL_CreateTextureFromSurface(layer->sdl_ren, sdl_surf);
	if (!sdl_tex) {
		printlog(
			LOG_ERROR,
			"Failed creating a texture from rendered text - internal error: %s",
			SDL_GetError());
		SDL_DestroySurface(sdl_surf);
		return;
	}

	Tex* tex = malloc(sizeof(Tex));
	tex->sdl_tex = sdl_tex;
	tex->original_size.w = sdl_surf->w;
	tex->original_size.h = sdl_surf->h;
	tex->next = NULL;
	SDL_DestroySurface(sdl_surf);

	renopts->tmptex = tex;

	layer->bounds.w = tex->original_size.w;
	layer->bounds.h = tex->original_size.h;
}
