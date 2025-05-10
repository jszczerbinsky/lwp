#include "../../types/layer.h"

typedef struct {
	const char* name;
	int			anchor;
} AnchorDef;

static const AnchorDef anchordefs[] = {
	{.name = "TopLeft", .anchor = ANCHOR_TOPL},
	{.name = "TopRight", .anchor = ANCHOR_TOPR},
	{.name = "Center", .anchor = ANCHOR_CENT},
	{.name = "BottomLeft", .anchor = ANCHOR_BOTL},
	{.name = "BottomRight", .anchor = ANCHOR_BOTR},
};

static const int anchordefscnt = sizeof(anchordefs) / sizeof(AnchorDef);

int parseanchor(const char* str) {
	for (int i = 0; i < anchordefscnt; i++) {
		if (strcmp(anchordefs[i].name, str) == 0) {
			return anchordefs[i].anchor;
		}
	}
	return ANCHOR_INVALID;
}
