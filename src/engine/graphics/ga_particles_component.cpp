/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/
#include "ga_particles_component.h"

#include "gui/ga_button.h"
#include "gui/ga_checkbox.h"
#include "gui/ga_slider.h"
#include "gui/ga_label.h"

ga_particles_component::ga_particles_component(ga_entity* ent, const char* texture_file) : ga_component(ent)
{
	_max_particles = 1000;
}

ga_particles_component::~ga_particles_component()
{
}

void ga_particles_component::update(ga_frame_params * params)
{
	_draw_gui = ga_checkbox(_draw_gui, "Show GUI", 5, 5, params).get_checked();
	if (_draw_gui)
	{
		static char integer_text[12];

		float label_width = 150;
		ga_vec2f gui_pos = { 200, 5 };
		float curr_y = gui_pos.y;
		float line_height = 25;
		float text_offset = 15;

	}
}
