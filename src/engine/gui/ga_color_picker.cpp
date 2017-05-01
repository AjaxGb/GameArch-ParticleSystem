/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_color_picker.h"
#include "ga_slider.h"

#include "framework/ga_frame_params.h"
#include "ga_font.h"

ga_color_picker::ga_color_picker(ga_vec3f color, float x, float y, struct ga_frame_params* params)
{
	float slider_width = 60;
	float padded_width = 70;
	float line_height = 20;

	_color.x = ga_slider(0, 1, color.x, x + padded_width * 0, y, slider_width, line_height, params).get_value();
	_color.y = ga_slider(0, 1, color.y, x + padded_width * 1, y, slider_width, line_height, params).get_value();
	_color.z = ga_slider(0, 1, color.z, x + padded_width * 2, y, slider_width, line_height, params).get_value();
	_color.w = 1;

	// Draw preview
	draw_filled_rect(x + padded_width * 3, x + padded_width * 3 + line_height, y, y + line_height, get_color(), params);
}

ga_color_picker::ga_color_picker(ga_vec4f color, float x, float y, struct ga_frame_params* params)
{
	float slider_width = 60;
	float padded_width = 70;
	float line_height = 20;

	_color.x = ga_slider(0, 1, color.x, x + padded_width * 0, y, slider_width, line_height, params).get_value();
	_color.y = ga_slider(0, 1, color.y, x + padded_width * 1, y, slider_width, line_height, params).get_value();
	_color.z = ga_slider(0, 1, color.z, x + padded_width * 2, y, slider_width, line_height, params).get_value();
	_color.w = ga_slider(0, 1, color.w, x + padded_width * 3, y, slider_width, line_height, params).get_value();

	// Draw preview
	draw_filled_rect(x + padded_width * 4, x + padded_width * 4 + line_height, y, y + line_height, get_color(), params);
}

ga_vec3f ga_color_picker::get_color() const
{
	return { _color.x, _color.y, _color.z };
}

ga_vec4f ga_color_picker::get_color_alpha() const
{
	return _color;
}
