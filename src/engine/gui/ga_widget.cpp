/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_widget.h"

void ga_widget::draw_empty_rect(float left, float right, float top, float bottom, ga_vec3f color, ga_frame_params* params)
{
	ga_dynamic_drawcall drawcall;

	drawcall._positions.push_back({ left,  top,    0.0f });
	drawcall._positions.push_back({ right, top,    0.0f });
	drawcall._positions.push_back({ right, bottom, 0.0f });
	drawcall._positions.push_back({ left,  bottom, 0.0f });

	drawcall._indices.push_back(0);
	drawcall._indices.push_back(1);
	drawcall._indices.push_back(2);
	drawcall._indices.push_back(3);

	drawcall._color = color;
	drawcall._draw_mode = GL_LINE_LOOP;
	drawcall._transform.make_identity();
	drawcall._material = nullptr;

	while (params->_gui_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
	params->_gui_drawcalls.push_back(drawcall);
	params->_gui_drawcall_lock.clear(std::memory_order_release);
}

void ga_widget::draw_filled_rect(float left, float right, float top, float bottom, ga_vec3f color, ga_frame_params* params)
{
	ga_dynamic_drawcall drawcall;

	drawcall._positions.push_back({ left,  top,    0.0f });
	drawcall._positions.push_back({ right, top,    0.0f });
	drawcall._positions.push_back({ right, bottom, 0.0f });
	drawcall._positions.push_back({ left,  bottom, 0.0f });

	drawcall._indices.push_back(0);
	drawcall._indices.push_back(1);
	drawcall._indices.push_back(3);
	drawcall._indices.push_back(2);

	drawcall._color = color;
	drawcall._draw_mode = GL_TRIANGLE_STRIP;
	drawcall._transform.make_identity();
	drawcall._material = nullptr;

	while (params->_gui_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
	params->_gui_drawcalls.push_back(drawcall);
	params->_gui_drawcall_lock.clear(std::memory_order_release);
}
