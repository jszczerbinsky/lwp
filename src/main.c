#include "platform.h"


#include <SDL3_ttf/SDL_ttf.h>
#include <libxml/xmlreader.h>

#include "types/gui.h"
#include "types/log.h"

int main(int argc, char** argv) {
	printlog(LOG_INFO, NULL, NULL, "JPaper " PROGRAM_VERSION " for " OS_NAME);

	LIBXML_TEST_VERSION

	printlog(LOG_INFO, NULL, NULL, "Initializing GUI...");

	App app = {0};
	gui_init(&app.gui, argc, argv);

	xmlInitParser();

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		printlog(LOG_ERROR, NULL, SDL_GetError(), "Failed to initialize SDL3");
		return 1;
	}
	if (!TTF_Init()) {
		printlog(LOG_ERROR, NULL, SDL_GetError(),
				 "Failed to initialize SDL3_ttf");
		SDL_Quit();
		return 1;
	}

	LogContext logctx = {
		.name = "test instance",
		.lvl = LOG_DEBUG,
	};
	/*WlpInstance* inst = instance_create(&logctx);

	instance_load(inst, "/home/cziken/.config/lwp/testwlp");

	instance_run(inst);

	instance_free(inst);*/

	TTF_Quit();
	SDL_Quit();

	xmlCleanupParser();

	return 0;
}
