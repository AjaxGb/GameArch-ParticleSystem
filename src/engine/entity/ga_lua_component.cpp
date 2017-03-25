/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_lua_component.h"

#include "entity/ga_entity.h"
#include "framework/ga_frame_params.h"

#include <iostream>
#include <lua.hpp>

ga_lua_component::ga_lua_component(ga_entity* ent, const char* path) : ga_component(ent)
{
	_lua = luaL_newstate();
	luaL_openlibs(_lua);

	luaL_newmetatable(_lua, "ga_lua_component");
	luaL_newmetatable(_lua, "ga_frame_params");
	luaL_newmetatable(_lua, "ga_entity");

	lua_pushcfunction(_lua, lua_frame_params_get_input_left);
	lua_setglobal(_lua, "frame_params_get_input_left");
	lua_pushcfunction(_lua, lua_frame_params_get_input_right);
	lua_setglobal(_lua, "frame_params_get_input_right");
	lua_pushcfunction(_lua, lua_component_get_entity);
	lua_setglobal(_lua, "component_get_entity");
	lua_pushcfunction(_lua, lua_entity_translate);
	lua_setglobal(_lua, "entity_translate");

	extern char g_root_path[256];
	std::string fullpath = g_root_path;
	fullpath += path;

	int status = luaL_dofile(_lua, fullpath.c_str());
	if (status)
	{
		std::cerr << "Failed to load script " << path << ": " << lua_tostring(_lua, -1) << std::endl;
		lua_close(_lua);
	}
}

ga_lua_component::~ga_lua_component()
{
	lua_close(_lua);
}

void ga_lua_component::update(ga_frame_params* params)
{
	lua_getglobal(_lua, "update");

	// Push function arguments onto stack
	ga_lua_component** arg_component = (ga_lua_component**)lua_newuserdata(_lua, sizeof(ga_lua_component*));
	(*arg_component) = this;
	luaL_getmetatable(_lua, "ga_lua_component");
	lua_setmetatable(_lua, -2);

	ga_frame_params* arg_params = (ga_frame_params*)lua_newuserdata(_lua, sizeof(ga_frame_params));
	arg_params->_current_time = params->_current_time;
	arg_params->_delta_time = params->_delta_time;
	arg_params->_button_mask = params->_button_mask;
	arg_params->_mouse_click_mask = params->_mouse_click_mask;
	arg_params->_mouse_press_mask = params->_mouse_press_mask;
	arg_params->_mouse_x = params->_mouse_x;
	arg_params->_mouse_y = params->_mouse_y;
	arg_params->_view = params->_view;
	luaL_getmetatable(_lua, "ga_frame_params");
	lua_setmetatable(_lua, -2);

	if (lua_pcall(_lua, 2, 1, 0))
	{
		std::cerr << "Error while running script: " << lua_tostring(_lua, -1) << std::endl;
	}
}

int ga_lua_component::lua_frame_params_get_input_left(lua_State* lua)
{
	ga_ud_frame_params* params = (ga_ud_frame_params*)luaL_checkudata(lua, 1, "ga_frame_params");
	luaL_argcheck(lua, params != nullptr, 1, "`ga_frame_params' expected");
	lua_pushboolean(lua, params->_button_mask & k_button_j);
	return 1;
}

int ga_lua_component::lua_frame_params_get_input_right(lua_State* lua)
{
	ga_ud_frame_params* params = (ga_ud_frame_params*)luaL_checkudata(lua, 1, "ga_frame_params");
	luaL_argcheck(lua, params != nullptr, 1, "`ga_frame_params' expected");
	lua_pushboolean(lua, params->_button_mask & k_button_k);
	return 1;
}

int ga_lua_component::lua_component_get_entity(lua_State* lua)
{
	ga_lua_component* component = *(ga_lua_component**)luaL_checkudata(lua, 1, "ga_lua_component");
	luaL_argcheck(lua, component != nullptr, 1, "`ga_lua_component' expected");

	ga_entity** entity = (ga_entity**)lua_newuserdata(lua, sizeof(ga_entity*));
	(*entity) = component->get_entity();
	luaL_getmetatable(lua, "ga_entity");
	lua_setmetatable(lua, -2);
	return 1;
}

int ga_lua_component::lua_entity_translate(lua_State* lua)
{
	ga_entity* entity = *(ga_entity**)luaL_checkudata(lua, 1, "ga_entity");
	luaL_argcheck(lua, entity != nullptr, 1, "`ga_entity' expected");
	
	entity->translate({ (float)luaL_checknumber(lua, 2), (float)luaL_checknumber(lua, 3), (float)luaL_checknumber(lua, 4) });
	return 0;
}
