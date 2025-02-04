#include "main.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <libxml/xmlreader.h>

int main(int argc, char** argv) {

	LIBXML_TEST_VERSION

	xmlInitParser();

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		printlog(LOG_ERROR, "Failed to initialize SDL3 - Internal error: %s",
				 SDL_GetError());
		return 1;
	}
	if (!TTF_Init()) {
		printlog(LOG_ERROR,
				 "Failed to initialize SDL3_ttf - Internal error: %s",
				 SDL_GetError());
		SDL_Quit();
		return 1;
	}

	WlpInstance* inst = instance_create();

	instance_load_wlp(inst, "/home/cziken/.config/lwp/testwlp");

	instance_run(inst);

	instance_free(inst);

	TTF_Quit();
	SDL_Quit();

	xmlCleanupParser();

	return 0;
}
