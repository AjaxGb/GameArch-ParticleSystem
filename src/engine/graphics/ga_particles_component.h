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
#include "graphics/ga_material.h"
#include "math/ga_easing.h"

#include "math/ga_mat4f.h"
#include "math/ga_vec3f.h"

struct ga_particle
{
	ga_vec3f _pos;
	ga_vec3f _vel;
	float _start_life;
	float _curr_life;
	float _start_size;
	float _end_size;
	ga_vec4f _start_color;
	ga_vec4f _end_color;
	float _camera_dist;

	bool operator<(const ga_particle& that) const;
};

struct ga_rand_float
{
	bool _rand = false;
	float _min = 0;
	float _max = 0;

	float get() const;
};

struct ga_rand_vec4f
{
	bool _rand = false;
	ga_vec4f _min = { 1, 1, 1, 1 };
	ga_vec4f _max = { 1, 1, 1, 1 };

	ga_vec4f get() const;
};

/*
** Class for particle systems.
*/
class ga_particles_component : public ga_component
{
public:
	ga_particles_component(class ga_entity* ent, const char* texture_file, int max_particles = 10000);
	virtual ~ga_particles_component();

	virtual void update(struct ga_frame_params* params) override;

	int max_particles() const;
	int max_particles(int max_particles);

	bool _emitting = true;
	bool _draw_gui;

	ga_rand_float _spawn_per_second;

	float _gravity_scale = 0;

	ga_rand_float _start_life;
	ga_rand_float _start_speed;

	ga_rand_float _start_size;
	bool _static_size = true;
	ga_rand_float _end_size;
	ga_easing_function _size_easing = ga_easing_linear;

	ga_rand_vec4f _start_color;
	bool _static_color = true;
	ga_rand_vec4f _end_color;
	ga_easing_function _color_easing = ga_easing_linear;
private:
	class ga_particle_material* _material;
	GLuint _vao;
	enum vbo_types { VERT, TEX_COORD, POS_SIZE, COLOR, INDICES, NUM_OF_VBOS };
	GLuint _vbos[NUM_OF_VBOS];
	uint32_t _index_count;

	ga_particle* _particles;
	ga_vec4f* _gpu_pos_size_data;
	ga_vec4f* _gpu_color_data;

	int _max_particles;
	int _last_used;

	int find_unused_particle();

	void draw_gui(struct ga_frame_params* params);

	void delete_all_buffers();
	void generate_all_buffers(int max_particles);
};

/*
** Material used to render particles.
*/
class ga_particle_material : public ga_material
{
public:
	ga_particle_material(const char* texture_file);
	~ga_particle_material();

	virtual bool init() override;

	virtual void bind(const struct ga_mat4f& view, const ga_mat4f& proj, const struct ga_mat4f& transform) override;

	virtual void set_color(const struct ga_vec3f& color) override { _color = color; }

	bool _additive_blending = false;
private:
	std::string _texture_file;

	ga_shader* _vs;
	ga_shader* _fs;
	ga_program* _program;
	ga_texture* _texture;
	ga_vec3f _color;
};
