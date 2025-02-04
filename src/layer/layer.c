#include "../main.h"
#include <stdlib.h>

void layer_freecontent(Layer* layer) {
	switch (layer->contenttype) {
	case CONTENT_NONE:
		break;
	case CONTENT_IMG:
		break;
	case CONTENT_TEXT:
		if (layer->content.text.tmptex) {
			tex_free(layer->content.text.tmptex);
		}
		break;
	case CONTENT_ANIM:
		break;
	case CONTENT_COL:
		break;
	}
	layer->contenttype = CONTENT_NONE;
}

const Tex* layer_getrentex(Layer* layer) {
	switch (layer->contenttype) {
	case CONTENT_IMG:
		return layer->content.img.tex;
	case CONTENT_TEXT:
		return layer->content.text.tmptex;
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

void layer_addbehaviour(Layer* layer, int behaviour, float farg) {
	layer->behcnt++;
	layer->behs = realloc(layer->behs, layer->behcnt * sizeof(Behaviour));
	layer->behs[layer->behcnt - 1].behid = behaviour;
	layer->behs[layer->behcnt - 1].farg = farg;
}

void layer_setcontent(Layer* layer, int contenttype) {
	const RGBA black = {0, 0, 0, 255};
	const RGBA transparent = {0, 0, 0, 0};

	layer->contenttype = contenttype;

	switch (contenttype) {
	case CONTENT_NONE:
		break;
	case CONTENT_IMG:
		layer->content.img.tex = NULL;
		break;
	case CONTENT_TEXT:
		layer->content.text.str = NULL;
		layer->content.text.tmptex = NULL;
		layer->content.text.font = NULL;
		layer_text_setfg(layer, &black);
		layer_text_setbg(layer, &transparent);
		break;
	case CONTENT_ANIM:
		break;
	case CONTENT_COL:
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

	layer->contenttype = CONTENT_NONE;
	layer->anchor = ANCHOR_CENT;

	layer->behcnt = 0;
	layer->behs = NULL;

	layer->sdl_ren = inst->sdl_ren;

	layer->next = inst->layers;
	inst->layers = layer;

	return layer;
}
