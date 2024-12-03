#include "main.h"
#include <stdlib.h>

Layer* instance_spawn_layer(WlpInstance* inst, Tex* tex) {
	Layer* layer = malloc(sizeof(Layer));

	layer->current_tex = tex;

	layer->bounds.x = 0;
	layer->bounds.y = 0;
	layer->rot = 0;

	if (tex) {
		layer->bounds.w = tex->original_size.w;
		layer->bounds.h = tex->original_size.h;
	} else {
		layer->bounds.w = 0;
		layer->bounds.h = 0;
	}

	layer->next = inst->layers;
	inst->layers = layer;

	return layer;
}
