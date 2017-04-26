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

#include "math/ga_vec2f.h"

/*
** GUI slider widget.
*/
class ga_slider : public ga_widget
{
public:
	ga_slider(float min, float max, float current,
		float x, float y, float width, float height,
		struct ga_frame_params* params);

	float get_value() const;
private:
	float _value;
};
