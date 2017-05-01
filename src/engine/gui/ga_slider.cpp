/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_slider.h"

#include "framework/ga_frame_params.h"
#include "ga_font.h"

ga_slider::ga_slider(float min, float max, float current,
	float x, float y, float width, float height,
	struct ga_frame_params* params)
{
	float left = x;
	float right = x + width;
	float top = y;
	float bottom = y + height;

	bool mouseover =
		params->_mouse_x >= left  &&
		params->_mouse_x <= right &&
		params->_mouse_y >= top   &&
		params->_mouse_y <= bottom;
	bool pressed = mouseover && params->_mouse_press_mask;

	// DRAW BOX
	//   x
	// y 0--1
	//   |  |
	//   3--2

	draw_empty_rect(left, right, top, bottom, mouseover ? k_color_active : k_color_inactive, params);

	// DRAW SLIDER
	//   x
	// y 0--1
	//   |  |
	//   3--2

	float slider_width = 5;
	float slider_offset = 2;
	float slider_left;
	if (pressed)
	{
		slider_left = fmaxf(left, fminf(right - slider_width, params->_mouse_x - slider_offset));
		_value = fmaxf(min, fminf(max, min + (params->_mouse_x - slider_offset - left) / (width - slider_width) * (max - min)));
	}
	else
	{
		slider_left = left + ((width - slider_width) * (current - min) / (max - min));
		_value = current;
	}

	draw_filled_rect(slider_left, slider_left + slider_width, top, bottom, pressed ? k_color_active : k_color_inactive, params);
}

float ga_slider::get_value() const
{
	return _value;
}