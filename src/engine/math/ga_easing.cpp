/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_easing.h"

float ga_easing_linear(float t)
{
	return t;
}

float ga_easing_quad_in(float t)
{
	return t * t;
}

float ga_easing_quad_out(float t)
{
	return 1 - ga_easing_quad_in(1 - t);
}

float ga_easing_quad_inout(float t)
{
	if (t < 0.5f)
	{
		return ga_easing_quad_in(t * 2.0f) / 2.0f;
	}
	else
	{
		return ga_easing_quad_out(t * 2.0f) / 2.0f;
	}
}

float ga_easing_cubic_in(float t)
{
	return t * t * t;
}

float ga_easing_cubic_out(float t)
{
	return 1 - ga_easing_cubic_in(1 - t);
}

float ga_easing_cubic_inout(float t)
{
	if (t < 0.5f)
	{
		return ga_easing_cubic_in(t * 2.0f) / 2.0f;
	}
	else
	{
		return ga_easing_cubic_out(t * 2.0f) / 2.0f;
	}
}
