#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_widget.h"

#include "math/ga_vec3f.h"

/*
** GUI pressable checkbox widget.
*/
class ga_color_picker : public ga_widget
{
public:
	ga_color_picker(ga_vec3f color, float x, float y, struct ga_frame_params* params);
	ga_color_picker(ga_vec4f color, float x, float y, struct ga_frame_params* params);

	ga_vec4f get_color_alpha() const;
	ga_vec3f get_color() const;
private:
	ga_vec4f _color;
};
