#include "main.h"
#include <SDL3/SDL_mouse.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <math.h>

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

static int api_spawn(lua_State* lua) {
	WlpInstance* inst = get_inst(lua);
	const char*	 tex_name = luaL_checkstring(lua, 1);

	Tex* tex = NULL;

	Tex* ptr = inst->texs;
	while (ptr && !tex) {
		if (strcmp(tex_name, ptr->name) == 0) {
			tex = ptr;
		}
		ptr = ptr->next;
	}
	if (!tex) {
		luaL_error(lua, "Texture '%s' not found", tex_name);
		return 0;
	}

	Layer* layer = instance_spawn_layer(inst, tex);

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

static void def_funcs(WlpInstance* inst) {
	lua_State* lua = inst->lua;

	// Layer class
	luaL_newmetatable(lua, "LayerMetaTab");
	lua_pushvalue(lua, -1);
	lua_setfield(lua, -2, "__index");
	lua_pushcfunction(lua, api_layer_setpos);
	lua_setfield(lua, -2, "setpos");
	lua_pushcfunction(lua, api_layer_getpos);
	lua_setfield(lua, -2, "getpos");
	lua_pushcfunction(lua, api_layer_setsize);
	lua_setfield(lua, -2, "setsize");
	lua_pushcfunction(lua, api_layer_getsize);
	lua_setfield(lua, -2, "getsize");
	lua_pushcfunction(lua, api_layer_setrot);
	lua_setfield(lua, -2, "setrot");
	lua_pushcfunction(lua, api_layer_getrot);
	lua_setfield(lua, -2, "getrot");
	lua_pushcfunction(lua, api_spawn);
	lua_setglobal(lua, "Spawn");

	lua_pushcfunction(lua, api_getmousepos);
	lua_setglobal(lua, "GetMousePos");
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
		printlog(LOG_INFO, "Found lua file in %s", path);

		lua_pushlightuserdata(lua, (void*)&LIGHTUSERDATA_INST_PTR);
		lua_pushlightuserdata(lua, inst);
		lua_settable(lua, LUA_REGISTRYINDEX);

		def_funcs(inst);
	} else {
		lua_close(lua);
		printlog(LOG_INFO, "Lua file %s not found", path);
	}
}
