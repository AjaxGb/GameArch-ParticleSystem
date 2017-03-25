#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_component.h"
#include <lua.hpp>

/*
** Holds the parts of frame_params we wish to pass to LUA code.
** May safely persist beyond the end of the update.
*/
struct ga_ud_frame_params
{
	std::chrono::high_resolution_clock::time_point _current_time;
	std::chrono::high_resolution_clock::duration _delta_time;

	uint64_t _button_mask;

	uint64_t _mouse_click_mask;
	uint64_t _mouse_press_mask;
	float _mouse_x;
	float _mouse_y;

	ga_mat4f _view;
};

/*
** A component whose logic is implemented in LUA.
*/
class ga_lua_component : public ga_component
{
public:
	ga_lua_component(class ga_entity* ent, const char* path);
	virtual ~ga_lua_component();

	virtual void update(struct ga_frame_params* params) override;
private:
	lua_State* _lua;
	static int lua_frame_params_get_input_left(lua_State *lua);
	static int lua_frame_params_get_input_right(lua_State *lua);
	static int lua_component_get_entity(lua_State *lua);
	static int lua_entity_translate(lua_State *lua);
};
