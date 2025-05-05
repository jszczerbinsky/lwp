#include <SDL3/SDL_video.h>
#include <stdlib.h>

#include "../types/layer.h"
#include "../types/wlp.h"
#include "../utils.h"

static void update_bgfill(WlpInstance* inst, Layer* layer, Behaviour* beh,
						  float dt) {
	layer->anchor = ANCHOR_TOPL;
	layer->bounds.y = 0;

	int w, h;
	SDL_GetWindowSizeInPixels(inst->sdl_wnd, &w, &h);

	float hratio = h / layer->bounds.h;

	layer->scale.w = layer->scale.h = hratio;

	float diffw = layer->scale.w * layer->bounds.w - w;

	layer->bounds.x = -diffw / 2;
}

static void update_bgfit(WlpInstance* inst, Layer* layer, Behaviour* beh,
						 float dt) {
	layer->anchor = ANCHOR_TOPL;
	layer->bounds.x = 0;

	int w, h;
	SDL_GetWindowSizeInPixels(inst->sdl_wnd, &w, &h);

	float wratio = w / layer->bounds.w;

	layer->scale.w = layer->scale.h = wratio;

	float diffh = layer->scale.h * layer->bounds.h - h;

	layer->bounds.y = -diffh / 2;
}

static void update_bgstretch(WlpInstance* inst, Layer* layer, Behaviour* beh,
							 float dt) {
	layer->anchor = ANCHOR_TOPL;
	layer->bounds.x = 0;
	layer->bounds.y = 0;

	int w, h;
	SDL_GetWindowSizeInPixels(inst->sdl_wnd, &w, &h);
	layer->bounds.w = w;
	layer->bounds.h = h;
}

static void update_shake(WlpInstance* inst, Layer* layer, Behaviour* beh,
						 float dt) {
	layer->bounds.x += (rand() % beh->iarg) - beh->iarg / 2;
	layer->bounds.y += (rand() % beh->iarg) - beh->iarg / 2;
}

static void update_followmouse(WlpInstance* inst, Layer* layer, Behaviour* beh,
							   float dt) {
	float mx, my;
	SDL_GetGlobalMouseState(&mx, &my);

	layer->bounds.x = lerp(layer->bounds.x, mx, beh->farg * dt);
	layer->bounds.y = lerp(layer->bounds.y, my, beh->farg * dt);
}

typedef struct {
	const char* name;
	int			behid;
	void (*update)(WlpInstance* inst, Layer* layer, Behaviour* beh, float dt);
} BehaviourDef;

static const BehaviourDef behdefs[] = {
	{
		.name = "BgFill",
		.behid = BEHAVIOUR_BGFILL,
		.update = update_bgfill,
	},
	{
		.name = "BgFit",
		.behid = BEHAVIOUR_BGFIT,
		.update = update_bgfit,
	},
	{
		.name = "BgStretch",
		.behid = BEHAVIOUR_BGSTRETCH,
		.update = update_bgstretch,
	},
	{
		.name = "Shake",
		.behid = BEHAVIOUR_SHAKE,
		.update = update_shake,
	},

	{
		.name = "FollowMouse",
		.behid = BEHAVIOUR_FOLLOWMOUSE,
		.update = update_followmouse,
	},
};

static const int behdefscnt = sizeof(behdefs) / sizeof(BehaviourDef);

int parsebehaviour(const char* str) {
	for (int i = 0; i < behdefscnt; i++) {
		if (strcmp(behdefs[i].name, str) == 0) {
			return behdefs[i].behid;
		}
	}
	return ANCHOR_INVALID;
}

void layer_updatebehaviour(WlpInstance* inst, Layer* layer, float dt) {
	for (int i = 0; i < layer->behcnt; i++) {
		Behaviour* currbeh = layer->behs + i;

		(*behdefs[currbeh->behid].update)(inst, layer, currbeh, dt);
	}
}
