#include "main.h"
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

static const int LIGHTUSERDATA_INST_PTR = 0;

static WlpInstance* get_inst(lua_State* lua) {
	lua_pushlightuserdata(lua, (void*)&LIGHTUSERDATA_INST_PTR);
	lua_gettable(lua, LUA_REGISTRYINDEX);

	WlpInstance* inst = (WlpInstance*)lua_touserdata(lua, -1);
	lua_pop(lua, 1);
	return inst;
}

static Layer* check_layer_handle(WlpInstance* inst, lua_Integer layer_handle) {
	Layer* layer = inst->layers;
	while (layer) {
		if ((lua_Integer)layer == layer_handle) {
			return layer;
		}
		layer = layer->next;
	}
	printlog(LOG_WARNING, "Layer handle is broken");
	return NULL;
}

static int wlpapi_setpos(lua_State* lua) {
	WlpInstance* inst = get_inst(lua);

	lua_Integer layer_handle = luaL_checkinteger(lua, 1);
	lua_Number	mov_x = luaL_checknumber(lua, 2);
	lua_Number	mov_y = luaL_checknumber(lua, 3);

	Layer* layer = check_layer_handle(inst, layer_handle);
	if (layer) {
		layer->bounds.x = mov_x;
		layer->bounds.y = mov_y;
	}

	return 0;
}

static int wlpapi_move(lua_State* lua) {
	WlpInstance* inst = get_inst(lua);

	lua_Integer layer_handle = luaL_checkinteger(lua, 1);
	lua_Number	mov_x = luaL_checknumber(lua, 2);
	lua_Number	mov_y = luaL_checknumber(lua, 3);

	Layer* layer = check_layer_handle(inst, layer_handle);
	if (layer) {
		layer->bounds.x += mov_x;
		layer->bounds.y += mov_y;
	}

	return 0;
}

static int wlpapi_setrot(lua_State* lua) {
	WlpInstance* inst = get_inst(lua);

	lua_Integer layer_handle = luaL_checkinteger(lua, 1);
	lua_Number	rot = luaL_checknumber(lua, 2);

	Layer* layer = check_layer_handle(inst, layer_handle);
	if (layer) {
		layer->rot = rot;
	}

	return 0;
}

static int wlpapi_getrot(lua_State* lua) {
	WlpInstance* inst = get_inst(lua);

	lua_Integer layer_handle = luaL_checkinteger(lua, 1);

	Layer* layer = check_layer_handle(inst, layer_handle);
	if (layer) {
		lua_pushnumber(lua, lua_tonumber(lua, layer->rot));
		return 1;
	}

	return 0;
}

static int wlpapi_rotate(lua_State* lua) {
	WlpInstance* inst = get_inst(lua);

	lua_Integer layer_handle = luaL_checkinteger(lua, 1);
	lua_Number	rot = luaL_checknumber(lua, 2);

	Layer* layer = check_layer_handle(inst, layer_handle);
	if (layer) {
		layer->rot += rot;
	}

	return 0;
}

static int wlpapi_spawn(lua_State* lua) {
	WlpInstance* inst = get_inst(lua);

	const char* tex_name = luaL_checkstring(lua, 1);

	Tex* tex = NULL;

	Tex* ptr = inst->texs;
	while (ptr && !tex) {
		if (strcmp(tex_name, ptr->name) == 0) {
			tex = ptr;
		}
		ptr = ptr->next;
	}
	if (!tex) {
		printlog(LOG_WARNING, "Texture '%s' not found", tex_name);
	}

	lua_Integer layer_handle = (lua_Integer)instance_spawn_layer(inst, tex);
	lua_pushinteger(lua, layer_handle);
	return 1;
}

static int wlpapi_getpos(lua_State* lua) {
	WlpInstance* inst = get_inst(lua);

	lua_Integer layer_handle = luaL_checkinteger(lua, 1);

	Layer* layer = check_layer_handle(inst, layer_handle);
	if (layer) {
		lua_pushnumber(lua, layer->bounds.x);
		lua_pushnumber(lua, layer->bounds.y);
		return 2;
	}

	return 0;
}

static int wlpapi_setsize(lua_State* lua) {
	WlpInstance* inst = get_inst(lua);

	lua_Integer layer_handle = luaL_checkinteger(lua, 1);
	lua_Number	w = luaL_checknumber(lua, 2);
	lua_Number	h = luaL_checknumber(lua, 3);

	Layer* layer = check_layer_handle(inst, layer_handle);
	if (layer) {
		layer->bounds.w = w;
		layer->bounds.h = h;
	}

	return 0;
}

static int wlpapi_getsize(lua_State* lua) {
	WlpInstance* inst = get_inst(lua);

	lua_Integer layer_handle = luaL_checkinteger(lua, 1);

	Layer* layer = check_layer_handle(inst, layer_handle);
	if (layer) {
		lua_pushnumber(lua, layer->bounds.w);
		lua_pushnumber(lua, layer->bounds.h);
		return 2;
	}

	return 0;
}

static void def_funcs(WlpInstance* inst) {
	lua_pushcfunction(inst->lua, wlpapi_spawn);
	lua_setglobal(inst->lua, "spawn");
	lua_pushcfunction(inst->lua, wlpapi_move);
	lua_setglobal(inst->lua, "move");
	lua_pushcfunction(inst->lua, wlpapi_setpos);
	lua_setglobal(inst->lua, "setpos");
	lua_pushcfunction(inst->lua, wlpapi_getpos);
	lua_setglobal(inst->lua, "getpos");
	lua_pushcfunction(inst->lua, wlpapi_setrot);
	lua_setglobal(inst->lua, "setrot");
	lua_pushcfunction(inst->lua, wlpapi_getrot);
	lua_setglobal(inst->lua, "getrot");
	lua_pushcfunction(inst->lua, wlpapi_setsize);
	lua_setglobal(inst->lua, "setsize");
	lua_pushcfunction(inst->lua, wlpapi_getsize);
	lua_setglobal(inst->lua, "getsize");
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
