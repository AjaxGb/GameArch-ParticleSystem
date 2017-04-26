/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_checkbox.h"

#include "framework/ga_frame_params.h"
#include "ga_font.h"

ga_checkbox::ga_checkbox(bool state, const char* text, float x, float y, ga_frame_params* params)
{
	float left   = x;
	float right  = x + 13;
	float top    = y;
	float bottom = y + 13;

	bool mouseover =
		params->_mouse_x >= left  &&
		params->_mouse_x <= right &&
		params->_mouse_y >= top   &&
		params->_mouse_y <= bottom;
	bool pressed = mouseover && params->_mouse_press_mask;
	bool clicked = mouseover && params->_mouse_click_mask;

	// DRAW BOX
	//   x
	// y 0--1
	//   |  |
	//   3--2

	ga_dynamic_drawcall drawcall;

	drawcall._positions.push_back({ left,  top,    0.0f });
	drawcall._positions.push_back({ right, top,    0.0f });
	drawcall._positions.push_back({ right, bottom, 0.0f });
	drawcall._positions.push_back({ left,  bottom, 0.0f });

	drawcall._indices.push_back(0);
	drawcall._indices.push_back(1);
	drawcall._indices.push_back(2);
	drawcall._indices.push_back(3);

	drawcall._color = mouseover ? k_color_active : k_color_inactive;
	drawcall._draw_mode = GL_LINE_LOOP;
	drawcall._transform.make_identity();
	drawcall._material = nullptr;

	while (params->_gui_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
	params->_gui_drawcalls.push_back(drawcall);
	params->_gui_drawcall_lock.clear(std::memory_order_release);
	
	// DRAW TEXT

	extern ga_font* g_font;
	g_font->print(params, text, right + 3, bottom - 1, k_color_text);

	if (clicked) state = !state;
	_checked = state;

	if (state)
	{
		// DRAW CROSS
		//   x
		// y 0 1
		//    X
		//   3 2

		ga_dynamic_drawcall drawcall;

		drawcall._positions.push_back({ left,  top,    0.0f });
		drawcall._positions.push_back({ right, top,    0.0f });
		drawcall._positions.push_back({ right, bottom, 0.0f });
		drawcall._positions.push_back({ left,  bottom, 0.0f });

		drawcall._indices.push_back(0);
		drawcall._indices.push_back(2);
		drawcall._indices.push_back(1);
		drawcall._indices.push_back(3);

		drawcall._color = pressed ? k_color_active : k_color_inactive;
		drawcall._draw_mode = GL_LINES;
		drawcall._transform.make_identity();
		drawcall._material = nullptr;

		while (params->_gui_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
		params->_gui_drawcalls.push_back(drawcall);
		params->_gui_drawcall_lock.clear(std::memory_order_release);
	}
}

bool ga_checkbox::get_checked() const
{
	return _checked;
}
