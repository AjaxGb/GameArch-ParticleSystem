#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "entity/ga_component.h"

#include "math/ga_mat4f.h"
#include "math/ga_vec3f.h"

struct ga_particle
{
	ga_vec3f pos;
	ga_vec3f vel;
	ga_vec3f acc;
};

/*
** Class for particle systems.
*/
class ga_particles_component : public ga_component
{
public:
	ga_particles_component(class ga_entity* ent, const char* texture_file);
	virtual ~ga_particles_component();

	virtual void update(struct ga_frame_params* params) override;

	bool _emitting = true;
	bool _draw_gui;

private:
	ga_particle* _particles;
	int _max_particles;
	int _first_particle;
	int _next_empty;
};
