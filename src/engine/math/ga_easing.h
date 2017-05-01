#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

/*
** Easing function.
** Input: t - Time, between 0 and 1 inclusive.
** Output: Position, where 0 and 1 are the start and end positions, respectively.
*/
typedef float(*ga_easing_function)(float);

float ga_easing_linear(float);
float ga_easing_quad_in(float);
float ga_easing_quad_out(float);
float ga_easing_quad_inout(float);
float ga_easing_cubic_in(float);
float ga_easing_cubic_out(float);
float ga_easing_cubic_inout(float);
