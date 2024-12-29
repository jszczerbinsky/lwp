#include "../main.h"
#include <stdlib.h>

void layer_freerenopts(Layer* layer) {
	switch (layer->rentype) {
	case REN_NONE:
		break;
	case REN_IMG:
		break;
	case REN_TEXT:
		if (layer->renopts.text.tmptex) {
			tex_free(layer->renopts.text.tmptex);
		}
		break;
	case REN_ANIM:
		break;
	case REN_COL:
		break;
	}
	layer->rentype = REN_NONE;
}

const Tex* layer_getcurrtex(Layer* layer) {
	switch (layer->rentype) {
	case REN_IMG:
		return layer->renopts.img.tex;
	case REN_TEXT:
		return layer->renopts.text.tmptex;
	default:
		return NULL;
	}
}

void layer_initas(Layer* layer, int rentype) {
	const RGBA black = {0, 0, 0, 255};
	const RGBA transparent = {0, 0, 0, 0};

	layer->rentype = rentype;

	switch (rentype) {
	case REN_NONE:
		break;
	case REN_IMG:
		layer->renopts.img.tex = NULL;
		break;
	case REN_TEXT:
		layer->renopts.text.str = NULL;
		layer->renopts.text.tmptex = NULL;
		layer->renopts.text.font = NULL;
		layer_text_setfg(layer, &black);
		layer_text_setbg(layer, &transparent);
		break;
	case REN_ANIM:
		break;
	case REN_COL:
		break;
	}
}

Layer* layer_spawnempty(WlpInstance* inst) {
	Layer* layer = malloc(sizeof(Layer));

	layer->bounds.x = 0;
	layer->bounds.y = 0;
	layer->scale.w = 1;
	layer->scale.h = 1;
	layer->rot = 0;

	layer->rentype = REN_NONE;
	layer->boundstype = BOUNDS_FIXED;

	layer->sdl_ren = inst->sdl_ren;

	layer->next = inst->layers;
	inst->layers = layer;

	return layer;
}
