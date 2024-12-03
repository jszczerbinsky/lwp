#include "main.h"
#include <SDL3/SDL.h>

int main(int argc, char** argv) {

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		printlog(LOG_ERROR, "Failed to initialize SDL3 - Internal error: %s",
				 SDL_GetError());
		return 1;
	}

	WlpInstance* inst = instance_create();

	instance_load_wlp(inst, "/home/cziken/testwlp");

	instance_run(inst);

	while (1) {
		SDL_Delay(1000);
	}

	instance_free(inst);

	SDL_Quit();

	return 0;

	/*lua_State *L = luaL_newstate();
	  luaL_openlibs(L);

	  if (luaL_dofile(L, "main.lua"))
	  {
	  printf("cant find main.lua\n");
	  lua_close(L);
	  }

	  lua_getglobal(L, "start");
	  if (lua_pcall(L, 0, 1, 0) != LUA_OK)
	  {
	  printf("Error running main: %s\n", lua_tostring(L, -1));
	  }

	  double returned = lua_tonumber(L, -1);
	  printf("function returned: %f\n", returned);

	  lua_pop(L, 1);

	  lua_close(L);*/
	return 0;
}
