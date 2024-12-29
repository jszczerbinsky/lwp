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

const Tex* layer_getrentex(Layer* layer) {
	switch (layer->rentype) {
	case REN_IMG:
		return layer->renopts.img.tex;
	case REN_TEXT:
		return layer->renopts.text.tmptex;
	default:
		return NULL;
	}
}

void layer_getrenbounds(Layer* layer, BoundsF* destbounds) {
	destbounds->w = layer->bounds.w * layer->scale.w;
	destbounds->h = layer->bounds.h * layer->scale.h;

	switch (layer->anchor) {
	case ANCHOR_TOPL:
		destbounds->x = layer->bounds.x;
		destbounds->y = layer->bounds.y;
		break;
	case ANCHOR_TOPR:
		destbounds->x = layer->bounds.x - destbounds->w;
		destbounds->y = layer->bounds.y;
		break;
	case ANCHOR_CENT:
		destbounds->x = layer->bounds.x - destbounds->w / 2;
		destbounds->y = layer->bounds.y - destbounds->h / 2;
		break;
	case ANCHOR_BOTL:
		destbounds->x = layer->bounds.x;
		destbounds->y = layer->bounds.y - destbounds->h;
		break;
	case ANCHOR_BOTR:
		destbounds->x = layer->bounds.x - destbounds->w;
		destbounds->y = layer->bounds.y - destbounds->h;
		break;
	}
}

void layer_addbehaviour(Layer* layer, int behaviour, float arg) {
	layer->behcnt++;
	layer->behs = realloc(layer->behs, layer->behcnt * sizeof(Behaviour));
	layer->behs[layer->behcnt - 1].behid = behaviour;
	layer->behs[layer->behcnt - 1].arg = arg;
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
	layer->anchor = ANCHOR_CENT;

	layer->behcnt = 0;
	layer->behs = NULL;

	layer->sdl_ren = inst->sdl_ren;

	layer->next = inst->layers;
	inst->layers = layer;

	return layer;
}
