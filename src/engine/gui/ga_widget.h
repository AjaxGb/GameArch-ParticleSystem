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

/*
** Base class for GUI widgets.
*/
class ga_widget
{
public:
	const ga_vec3f k_color_inactive = { 0.25f, 0.25f, 1.0f };
	const ga_vec3f k_color_active = { 0.75f, 0.75f, 1.0f };
	const ga_vec3f k_color_text = { 1.00f, 1.00f, 1.0f };
};
