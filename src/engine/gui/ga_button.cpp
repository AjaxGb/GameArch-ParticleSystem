/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_button.h"

#include "framework/ga_frame_params.h"
#include "ga_font.h"

ga_button::ga_button(const char* text, float x, float y, ga_frame_params* params)
{
	// Print text first, to get bounds
	ga_vec2f text_min, text_max;
	extern ga_font* g_font;
	g_font->print(params, text, x, y, k_color_text, &text_min, &text_max);

	float left   = text_min.x - 5;
	float right  = text_max.x + 5;
	float top    = text_min.y - 5;
	float bottom = text_max.y + 5;

	bool mouseover =
		params->_mouse_x >= left  &&
		params->_mouse_x <= right &&
		params->_mouse_y >= top   &&
		params->_mouse_y <= bottom;
	bool pressed = mouseover && params->_mouse_press_mask;
	_clicked = mouseover && params->_mouse_click_mask;

	if (pressed)
	{
		// DRAW BACKGROUND
		//   x
		// y 0--1
		//   |  |
		//   3--2

		ga_dynamic_drawcall drawcall;

		drawcall._positions.push_back({ text_min.x, text_min.y, -1.0f });
		drawcall._positions.push_back({ text_max.x, text_min.y, -1.0f });
		drawcall._positions.push_back({ text_max.x, text_max.y, -1.0f });
		drawcall._positions.push_back({ text_min.x, text_max.y, -1.0f });

		drawcall._indices.push_back(0);
		drawcall._indices.push_back(1);
		drawcall._indices.push_back(3);
		drawcall._indices.push_back(2);

		drawcall._color = k_color_inactive;
		drawcall._draw_mode = GL_TRIANGLE_STRIP;
		drawcall._transform.make_identity();
		drawcall._material = nullptr;

		while (params->_gui_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
		// Insert background before text, so it is drawn first
		params->_gui_drawcalls.insert(params->_gui_drawcalls.end() - 1, drawcall);
		params->_gui_drawcall_lock.clear(std::memory_order_release);
	}

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
}

bool ga_button::get_clicked(const ga_frame_params* params) const
{
	return _clicked;
}
