#include "../types/layer.h"

void layer_img_settex(Layer* layer, const Tex* tex, int applybounds) {
	layer->content.img.tex = tex;

	if (tex) {
		layer->bounds.w = tex->original_size.w;
		layer->bounds.h = tex->original_size.h;
	} else {
		layer->bounds.w = 0;
		layer->bounds.h = 0;
	}
}
