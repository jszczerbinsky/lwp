#include "../main.h"
#include <SDL3/SDL_video.h>
#include <stdlib.h>

static void update_bgfill(WlpInstance* inst, Layer* layer) {
	layer->anchor = ANCHOR_TOPL;
	layer->bounds.y = 0;

	int w, h;
	SDL_GetWindowSizeInPixels(inst->sdl_wnd, &w, &h);

	float hratio = h / layer->bounds.h;

	layer->scale.w = layer->scale.h = hratio;

	float diffw = layer->scale.w * layer->bounds.w - w;

	layer->bounds.x = -diffw / 2;
}

static void update_bgfit(WlpInstance* inst, Layer* layer) {
	layer->anchor = ANCHOR_TOPL;
	layer->bounds.x = 0;

	int w, h;
	SDL_GetWindowSizeInPixels(inst->sdl_wnd, &w, &h);

	float wratio = w / layer->bounds.w;

	layer->scale.w = layer->scale.h = wratio;

	float diffh = layer->scale.h * layer->bounds.h - h;

	layer->bounds.y = -diffh / 2;
}

static void update_bgstretch(WlpInstance* inst, Layer* layer) {
	layer->anchor = ANCHOR_TOPL;
	layer->bounds.x = 0;
	layer->bounds.y = 0;

	int w, h;
	SDL_GetWindowSizeInPixels(inst->sdl_wnd, &w, &h);
	layer->bounds.w = w;
	layer->bounds.h = h;
}

static void update_shake(WlpInstance* inst, Layer* layer, float arg) {
	int argi = (int)arg;

	layer->bounds.x += (rand() % argi) - argi / 2;
	layer->bounds.y += (rand() % argi) - argi / 2;
}

static void update_followmouse(WlpInstance* inst, Layer* layer, float arg,
							   float dt) {
	float mx, my;
	SDL_GetGlobalMouseState(&mx, &my);

	layer->bounds.x = lerp(layer->bounds.x, mx, arg * dt);
	layer->bounds.y = lerp(layer->bounds.y, my, arg * dt);
}

void layer_updatebehaviour(WlpInstance* inst, Layer* layer, float dt) {
	for (int i = 0; i < layer->behcnt; i++) {
		switch (layer->behs[i].behid) {
		case BEHAVIOUR_BGFIT:
			update_bgfit(inst, layer);
			break;
		case BEHAVIOUR_BGFILL:
			update_bgfill(inst, layer);
			break;
		case BEHAVIOUR_BGSTRETCH:
			update_bgstretch(inst, layer);
			break;
		case BEHAVIOUR_SHAKE:
			update_shake(inst, layer, layer->behs[i].farg);
			break;
		case BEHAVIOUR_FOLLOWMOUSE:
			update_followmouse(inst, layer, layer->behs[i].farg, dt);
			break;
		}
	}
}
