#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "math/ga_vec3f.h"
#include "framework/ga_frame_params.h"

/*
** Base class for GUI widgets.
*/
class ga_widget
{
public:
	const ga_vec3f k_color_inactive = { 0.25f, 0.25f, 1.0f };
	const ga_vec3f k_color_active = { 0.75f, 0.75f, 1.0f };
	const ga_vec3f k_color_text = { 1.00f, 1.00f, 1.0f };
	static void draw_empty_rect(float left, float right, float top, float bottom, ga_vec3f color, ga_frame_params* params);
	static void draw_filled_rect(float left, float right, float top, float bottom, ga_vec3f color, ga_frame_params* params);
};
