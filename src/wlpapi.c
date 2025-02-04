#include "main.h"
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_pixels.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <time.h>

static const int LIGHTUSERDATA_INST_PTR = 0;

static WlpInstance* get_inst(lua_State* lua) {
	lua_pushlightuserdata(lua, (void*)&LIGHTUSERDATA_INST_PTR);
	lua_gettable(lua, LUA_REGISTRYINDEX);

	WlpInstance* inst = (WlpInstance*)lua_touserdata(lua, -1);
	lua_pop(lua, 1);
	return inst;
}

static int api_layer_setpos(lua_State* lua) {
	Layer* layer = (Layer*)luaL_checkudata(lua, 1, "LayerMetaTab");
	layer->bounds.x = luaL_checknumber(lua, 2);
	layer->bounds.y = luaL_checknumber(lua, 3);
	return 0;
}

static int api_layer_getpos(lua_State* lua) {
	Layer* layer = (Layer*)luaL_checkudata(lua, 1, "LayerMetaTab");
	lua_pushnumber(lua, layer->bounds.x);
	lua_pushnumber(lua, layer->bounds.y);
	return 2;
}

static int api_layer_setsize(lua_State* lua) {
	Layer* layer = (Layer*)luaL_checkudata(lua, 1, "LayerMetaTab");
	layer->bounds.w = luaL_checknumber(lua, 2);
	layer->bounds.h = luaL_checknumber(lua, 3);
	return 0;
}

static int api_layer_getsize(lua_State* lua) {
	Layer* layer = (Layer*)luaL_checkudata(lua, 1, "LayerMetaTab");
	lua_pushnumber(lua, layer->bounds.w);
	lua_pushnumber(lua, layer->bounds.h);
	return 2;
}

static int api_layer_setscale(lua_State* lua) {
	Layer* layer = (Layer*)luaL_checkudata(lua, 1, "LayerMetaTab");
	layer->scale.w = luaL_checknumber(lua, 2);
	layer->scale.h = luaL_checknumber(lua, 3);
	return 0;
}

static int api_layer_getscale(lua_State* lua) {
	Layer* layer = (Layer*)luaL_checkudata(lua, 1, "LayerMetaTab");
	lua_pushnumber(lua, layer->scale.w);
	lua_pushnumber(lua, layer->scale.h);
	return 2;
}

static int api_layer_setrot(lua_State* lua) {
	Layer* layer = (Layer*)luaL_checkudata(lua, 1, "LayerMetaTab");
	layer->rot = luaL_checknumber(lua, 2);
	return 0;
}

static int api_layer_getrot(lua_State* lua) {
	Layer* layer = (Layer*)luaL_checkudata(lua, 1, "LayerMetaTab");
	lua_pushnumber(lua, layer->rot);
	return 1;
}

static int api_layer_addbehaviour(lua_State* lua) {
	Layer*		layer = (Layer*)luaL_checkudata(lua, 1, "LayerMetaTab");
	const char* beh_name = luaL_checkstring(lua, 2);

	int behaviour = parsebehaviour(beh_name);

	if (behaviour == BEHAVIOUR_INVALID) {
		luaL_error(lua, "Behaviour '%s' not found", beh_name);
		return 0;
	}

	float farg = luaL_optnumber(lua, 3, 0);
	float iarg = luaL_optnumber(lua, 4, 0);

	layer_addbehaviour(layer, behaviour, farg, iarg);

	return 0;
}

static int api_layer_setanchor(lua_State* lua) {
	Layer*		layer = (Layer*)luaL_checkudata(lua, 1, "LayerMetaTab");
	const char* anch_name = luaL_checkstring(lua, 2);

	if (strcmp(anch_name, "TopLeft") == 0) {
		layer->anchor = ANCHOR_TOPL;
	} else if (strcmp(anch_name, "TopLeft") == 0) {
		layer->anchor = ANCHOR_TOPL;
	} else if (strcmp(anch_name, "TopRight") == 0) {
		layer->anchor = ANCHOR_TOPR;
	} else if (strcmp(anch_name, "BottomLeft") == 0) {
		layer->anchor = ANCHOR_BOTL;
	} else if (strcmp(anch_name, "BottomRight") == 0) {
		layer->anchor = ANCHOR_BOTR;
	} else if (strcmp(anch_name, "Center") == 0) {
		layer->anchor = ANCHOR_CENT;
	} else {

		luaL_error(lua, "Anchor '%s' not found", anch_name);
		return 0;
	}

	return 0;
}

static int api_layer_settext(lua_State* lua) {
	WlpInstance* inst = get_inst(lua);

	Layer*		layer = (Layer*)luaL_checkudata(lua, 1, "LayerMetaTab");
	const char* str = luaL_checkstring(lua, 2);

	layer_text_settext(layer, str);

	return 0;
}

static int api_layer_settex(lua_State* lua) {
	WlpInstance* inst = get_inst(lua);

	Layer*		layer = (Layer*)luaL_checkudata(lua, 1, "LayerMetaTab");
	const char* tex_name = luaL_checkstring(lua, 2);
	int			applybounds = lua_toboolean(lua, 3);

	Tex* tex = tex_find(inst, tex_name);
	if (!tex) {
		luaL_error(lua, "Texture '%s' not found", tex_name);
		return 0;
	}

	if (layer->contenttype != CONTENT_IMG) {
		luaL_error(lua, "This function can be applied only to image layers");
		return 0;
	}

	layer_img_settex(layer, tex, applybounds);

	return 0;
}

static int api_spawnimg(lua_State* lua) {
	WlpInstance* inst = get_inst(lua);
	const char*	 tex_name = luaL_optstring(lua, 1, NULL);

	Tex* tex = tex_find(inst, tex_name);
	if (!tex) {
		luaL_error(lua, "Texture '%s' not found", tex_name);
		return 0;
	}

	Layer* layer = layer_spawnempty(inst);
	layer_setcontent(layer, CONTENT_IMG);

	if (tex) {
		layer_img_settex(layer, tex, 1);
	}

	lua_pushlightuserdata(lua, layer);

	luaL_getmetatable(lua, "LayerMetaTab");
	lua_setmetatable(lua, -2);

	return 1;
}

static int api_spawntext(lua_State* lua) {
	WlpInstance* inst = get_inst(lua);
	const char*	 font_name = luaL_checkstring(lua, 1);
	const char*	 str = luaL_optstring(lua, 2, NULL);

	Font* font = font_find(inst, font_name);
	if (!font) {
		luaL_error(lua, "Font '%s' not found", font_name);
		return 0;
	}

	Layer* layer = layer_spawnempty(inst);
	layer_setcontent(layer, CONTENT_TEXT);
	layer_text_setfont(layer, font);

	if (str) {
		layer_text_settext(layer, str);
	}

	lua_pushlightuserdata(lua, layer);

	luaL_getmetatable(lua, "LayerMetaTab");
	lua_setmetatable(lua, -2);

	return 1;
}

static int api_getmousepos(lua_State* lua) {
	float x, y;
	SDL_GetGlobalMouseState(&x, &y);
	lua_pushnumber(lua, x);
	lua_pushnumber(lua, y);
	return 2;
}

static int api_gettime(lua_State* lua) {
	time_t	   t = time(NULL);
	struct tm* ptr = localtime(&t);
	lua_newtable(lua);
	lua_pushstring(lua, "hour");
	lua_pushinteger(lua, ptr->tm_hour);
	lua_settable(lua, -3);
	lua_pushstring(lua, "min");
	lua_pushinteger(lua, ptr->tm_min);
	lua_settable(lua, -3);
	lua_pushstring(lua, "sec");
	lua_pushinteger(lua, ptr->tm_sec);
	lua_settable(lua, -3);
	lua_pushstring(lua, "day");
	lua_pushinteger(lua, ptr->tm_mday);
	lua_settable(lua, -3);
	lua_pushstring(lua, "month");
	lua_pushinteger(lua, ptr->tm_mon);
	lua_settable(lua, -3);
	lua_pushstring(lua, "year");
	lua_pushinteger(lua, ptr->tm_year);
	lua_settable(lua, -3);

	return 1;
}

static int api_color(lua_State* lua) {
	int r = luaL_checkinteger(lua, 1);
	if (r < 0 || r > 255) {
		return luaL_argerror(lua, 1, "Expected value from 0 to 255");
	}

	int g = luaL_checkinteger(lua, 2);
	if (r < 0 || r > 255) {
		return luaL_argerror(lua, 2, "Expected value from 0 to 255");
	}

	int b = luaL_checkinteger(lua, 3);
	if (r < 0 || r > 255) {
		return luaL_argerror(lua, 3, "Expected value from 0 to 255");
	}

	int a = luaL_optinteger(lua, 4, 0xff);
	if (r < 0 || r > 255) {
		return luaL_argerror(lua, 4, "Expected value from 0 to 255");
	}

	RGBA* color = (RGBA*)lua_newuserdata(lua, sizeof(RGBA));
	color->r = r;
	color->g = g;
	color->b = b;
	color->a = a;

	luaL_getmetatable(lua, "ColorMetaTab");
	lua_setmetatable(lua, -2);

	return 1;
}

static int api_getwndsize(lua_State* lua) {
	WlpInstance* inst = get_inst(lua);

	int w, h;
	SDL_GetWindowSizeInPixels(inst->sdl_wnd, &w, &h);

	lua_pushinteger(lua, w);
	lua_pushinteger(lua, h);
	return 2;
}

static void def_funcs(WlpInstance* inst) {
	lua_State* lua = inst->lua;

	// Color class
	luaL_newmetatable(lua, "ColorMetaTab");
	lua_pushvalue(lua, -1);
	lua_setfield(lua, -2, "__index");

	lua_pushcfunction(lua, api_color);
	lua_setglobal(lua, "Color");

	// Layer class
	luaL_newmetatable(lua, "LayerMetaTab");
	lua_pushvalue(lua, -1);
	lua_setfield(lua, -2, "__index");
	lua_pushcfunction(lua, api_layer_setpos);
	lua_setfield(lua, -2, "SetPosition");
	lua_pushcfunction(lua, api_layer_getpos);
	lua_setfield(lua, -2, "GetPosition");
	lua_pushcfunction(lua, api_layer_setsize);
	lua_setfield(lua, -2, "SetSize");
	lua_pushcfunction(lua, api_layer_getsize);
	lua_setfield(lua, -2, "GetSize");
	lua_pushcfunction(lua, api_layer_setscale);
	lua_setfield(lua, -2, "SetScale");
	lua_pushcfunction(lua, api_layer_getscale);
	lua_setfield(lua, -2, "GetScale");
	lua_pushcfunction(lua, api_layer_setrot);
	lua_setfield(lua, -2, "SetRotation");
	lua_pushcfunction(lua, api_layer_getrot);
	lua_setfield(lua, -2, "GetRotation");
	lua_pushcfunction(lua, api_layer_settext);
	lua_setfield(lua, -2, "SetText");
	lua_pushcfunction(lua, api_layer_setanchor);
	lua_setfield(lua, -2, "SetAnchor");
	lua_pushcfunction(lua, api_layer_addbehaviour);
	lua_setfield(lua, -2, "AddBehaviour");

	lua_pushcfunction(lua, api_spawnimg);
	lua_setglobal(lua, "SpawnImg");
	lua_pushcfunction(lua, api_spawntext);
	lua_setglobal(lua, "SpawnText");

	lua_pushcfunction(lua, api_getmousepos);
	lua_setglobal(lua, "GetMousePos");
	lua_pushcfunction(lua, api_gettime);
	lua_setglobal(lua, "GetTime");
	lua_pushcfunction(lua, api_getwndsize);
	lua_setglobal(lua, "GetWindowSize");
}

void wlpapi_init(WlpInstance* inst, const char* path) {
	lua_State* lua = luaL_newstate();
	luaopen_base(lua);
	luaopen_math(lua);
	luaopen_table(lua);
	luaopen_string(lua);

	// TODO: distingush between missing file/errors in file
	if (luaL_dofile(lua, path) == 0) {
		inst->lua = lua;
		printlog(LOG_INFO, "Loaded wallpaper script form file %s", path);

		lua_pushlightuserdata(lua, (void*)&LIGHTUSERDATA_INST_PTR);
		lua_pushlightuserdata(lua, inst);
		lua_settable(lua, LUA_REGISTRYINDEX);

		def_funcs(inst);
	} else {
		printlog(LOG_INFO, "Wallpaper script %s failed, internal error - %s",
				 path, lua_tostring(lua, -1));
		lua_close(lua);
	}
}
