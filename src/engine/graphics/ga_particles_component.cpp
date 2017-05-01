/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/
#include "ga_particles_component.h"
#include "entity/ga_entity.h"

#include "gui/ga_button.h"
#include "gui/ga_checkbox.h"
#include "gui/ga_slider.h"
#include "gui/ga_color_picker.h"
#include "gui/ga_label.h"

#include <iostream>

float random(float max)
{
	return static_cast<float>(rand()) / static_cast<float>(RAND_MAX / max);
}

float random(float min, float max)
{
	return min + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (max - min));
}

ga_vec3f rand_sphere_vector()
{
	float angle = random(360.0f);
	float z = random(-1.0f, 1.0f);
	float inv_z = sqrtf(1 - z * z);
	return { inv_z * cosf(angle), inv_z * sinf(angle), z };
}

ga_vec3f rand_circle_vector()
{
	float angle = random(360.0f);
	float z = random(-1.0f, 1.0f);
	float inv_z = sqrtf(1 - z * z);
	return{ inv_z * cosf(angle), inv_z * sinf(angle), 0 };
}

bool ga_particle::operator<(const ga_particle& that) const
{
	return this->_camera_dist > that._camera_dist;
}

ga_particles_component::ga_particles_component(ga_entity* ent, const char* texture_file, int max_particles) : ga_component(ent)
{
	_material = new ga_particle_material(texture_file);
	_material->init();

	generate_all_buffers(max_particles);
}

ga_particles_component::~ga_particles_component()
{
	delete_all_buffers();
	delete _material;
}

void ga_particles_component::update(ga_frame_params * params)
{
	_draw_gui = ga_checkbox(_draw_gui, "Show GUI", 5, 5, params).get_checked();
	if (_draw_gui)
	{
		draw_gui(params);
	}

	float delta_t = std::chrono::duration_cast<std::chrono::duration<float>>(params->_delta_time).count();

	if (_emitting)
	{
		int new_particle_count = _spawn_per_second.get() * delta_t;
		ga_vec3f my_pos = get_entity()->get_transform().get_translation();

		for (int i = 0; i < new_particle_count; i++)
		{
			ga_particle& p = _particles[find_unused_particle()];

			p._curr_life = p._start_life = _start_life.get();
			p._pos = my_pos;

			p._start_color = _start_color.get();
			p._end_color = _static_color ? p._start_color : _end_color.get();

			p._start_size = _start_size.get();
			p._end_size = _static_size ? p._start_size : _end_size.get();

			p._vel = rand_sphere_vector().scale_result(_start_speed.get());
		}
	}

	int particle_count = 0;
	for (int i = 0; i < _max_particles; i++)
	{
		ga_particle& p = _particles[i];

		if (p._curr_life > 0)
		{
			p._curr_life -= delta_t;
			if (p._curr_life > 0)
			{
				float p_life = 1 - p._curr_life / p._start_life;

				p._vel.y += -9.81f * delta_t * _gravity_scale;
				p._pos += p._vel.scale_result(delta_t);

				p._camera_dist = (p._pos - params->_camera_pos).mag2();

				float curr_size = p._start_size + _size_easing(p_life) * (p._end_size - p._start_size);
				_gpu_pos_size_data[particle_count] = { p._pos.x, p._pos.y, p._pos.z, curr_size };

				ga_vec4f curr_color = p._start_color + (p._end_color - p._start_color).scale_result(_color_easing(p_life));
				_gpu_color_data[particle_count] = curr_color;

				particle_count++;
			}
			else
			{
				p._camera_dist = -1;
			}
		}
		else
		{
			p._camera_dist = -1;
		}
	}
	_last_count = particle_count;

	if (!_material->_additive_blending)
	{
		std::sort(&_particles[0], &_particles[_max_particles]);
	}

	ga_instanced_drawcall draw;
	draw._name = "ga_particles_component";
	draw._vao = _vao;
	draw._index_count = _index_count;
	draw._transform = get_entity()->get_transform();
	draw._draw_mode = GL_TRIANGLES;
	draw._material = _material;
	draw._instance_count = particle_count;

	draw._streams = {
		{ _vbos[POS_SIZE], _max_particles * sizeof(ga_vec4f), particle_count * sizeof(ga_vec4f), _gpu_pos_size_data },
		{ _vbos[COLOR], _max_particles * sizeof(ga_vec4f), particle_count * sizeof(ga_vec4f), _gpu_color_data },
	};

	while (params->_instanced_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
	params->_instanced_drawcalls.push_back(draw);
	params->_instanced_drawcall_lock.clear(std::memory_order_release);
}

int ga_particles_component::max_particles() const
{
	return _max_particles;
}

int ga_particles_component::max_particles(int max_particles)
{
	delete_all_buffers();
	generate_all_buffers(max_particles);
	return _max_particles;
}

float ga_rand_float::get() const
{
	if (_rand)
	{
		return random(_min, _max);
	}
	return _min;
}

ga_vec4f ga_rand_vec4f::get() const
{
	if (_rand)
	{
		return{
			random(_min.x, _max.x),
			random(_min.y, _max.y),
			random(_min.z, _max.z),
			random(_min.w, _max.w),
		};
	}
	return _min;
}

int ga_particles_component::find_unused_particle()
{
	for (int i = _last_used; i < _max_particles; i++)
	{
		if (_particles[i]._curr_life <= 0)
		{
			_last_used = i;
			return i;
		}
	}

	for (int i = 0; i < _last_used; i++)
	{
		if (_particles[i]._curr_life <= 0)
		{
			_last_used = i;
			return i;
		}
	}

	// All are used, overwrite next.
	return ++_last_used;
}

#define DRAW_RAND_FLOAT(value, name, min, max)\
	ga_label("Randomize " name ":", curr_x, curr_y + text_offset, params);\
	(value)._rand = ga_checkbox((value)._rand, curr_x + label_width, curr_y + checkbox_offset, params).get_checked();\
	curr_y += line_height;\
	\
	ga_label((value)._rand ? name " Min:" : name ":", curr_x + indent, curr_y + text_offset, params);\
	(value)._min = ga_slider((min), (max), (value)._min, curr_x + label_width, curr_y, 120, 20, params).get_value();\
	curr_y += line_height;\
	\
	if ((value)._rand)\
	{\
		ga_label(name " Max:", curr_x + indent, curr_y + text_offset, params);\
		(value)._max = ga_slider((min), (max), (value)._max, curr_x + label_width, curr_y, 120, 20, params).get_value();\
		curr_y += line_height;\
	}

#define DRAW_RAND_COLOR(value, name)\
	ga_label("Randomize " name ":", curr_x, curr_y + text_offset, params);\
	(value)._rand = ga_checkbox((value)._rand, curr_x + label_width, curr_y + checkbox_offset, params).get_checked();\
	curr_y += line_height;\
	\
	ga_label((value)._rand ? name " Min:" : name ":", curr_x + indent, curr_y + text_offset, params);\
	(value)._min = ga_color_picker((value)._min, curr_x + label_width, curr_y, params).get_color_alpha();\
	curr_y += line_height;\
	\
	if ((value)._rand)\
	{\
		ga_label(name " Max:", curr_x + indent, curr_y + text_offset, params);\
		(value)._max = ga_color_picker((value)._max, curr_x + label_width, curr_y, params).get_color_alpha();\
		curr_y += line_height;\
	}

void ga_particles_component::draw_gui(struct ga_frame_params* params)
{
	static char integer_text[12];

	float curr_x = 5;
	float label_width = 225;
	float indent = 10;
	float curr_y = 30;
	float line_height = 25;
	float text_offset = 15;
	float checkbox_offset = 6;

	ga_label("Emitting:", curr_x, curr_y + text_offset, params);
	_emitting = ga_checkbox(_emitting, curr_x + label_width, curr_y + checkbox_offset, params).get_checked();
	ga_label(itoa(_last_count, integer_text, 10), curr_x + label_width + 40, curr_y + text_offset + 2, params);
	curr_y += line_height;

	curr_y += 10;

	DRAW_RAND_FLOAT(_spawn_per_second, "Spawn Per Second", 0, 1000);

	DRAW_RAND_FLOAT(_start_life, "Starting Life", 0, 10);

	DRAW_RAND_FLOAT(_start_speed, "Starting Speed", 0, 10);

	curr_y += 10;

	DRAW_RAND_FLOAT(_start_size, "Starting Size", 0, 10);

	ga_label("Static Size:", curr_x, curr_y + text_offset, params);
	_static_size = ga_checkbox(_static_size, curr_x + label_width, curr_y + checkbox_offset, params).get_checked();
	curr_y += line_height;

	if (!_static_size)
	{
		DRAW_RAND_FLOAT(_end_size, "Ending Size", 0, 10);

		ga_label("Size Easing:", curr_x, curr_y + text_offset, params);
		_size_easing = ga_button("Linear", curr_x + label_width, curr_y + 18, params).get_clicked() ? ga_easing_linear :
			ga_button("Quad In", curr_x + label_width + 70, curr_y + 18, params).get_clicked() ? ga_easing_quad_in :
			ga_button("Cubic In", curr_x + label_width + 150, curr_y + 18, params).get_clicked() ? ga_easing_cubic_in :
			_size_easing;
		curr_y += line_height;
	}

	curr_y += 10;

	DRAW_RAND_COLOR(_start_color, "Starting Color");

	ga_label("Static Color:", curr_x, curr_y + text_offset, params);
	_static_color = ga_checkbox(_static_color, curr_x + label_width, curr_y + checkbox_offset, params).get_checked();
	curr_y += line_height;

	if (!_static_color)
	{
		DRAW_RAND_COLOR(_end_color, "Ending Color");

		ga_label("Color Easing:", curr_x, curr_y + text_offset, params);
		_color_easing = ga_button("Linear", curr_x + label_width, curr_y + 16, params).get_clicked() ? ga_easing_linear :
			ga_button("Quad In", curr_x + label_width + 70, curr_y + 16, params).get_clicked() ? ga_easing_quad_in :
			ga_button("Cubic In", curr_x + label_width + 150, curr_y + 16, params).get_clicked() ? ga_easing_cubic_in :
			_color_easing;
		curr_y += line_height;
	}

	curr_y += 10;

	ga_label("Gravity Scale:", curr_x, curr_y + text_offset, params);
	_gravity_scale = ga_slider(-2, 2, _gravity_scale, curr_x + label_width, curr_y, 120, 20, params).get_value();
	curr_y += line_height;

	ga_label("Additive Blending:", curr_x, curr_y + text_offset, params);
	_material->_additive_blending = ga_checkbox(_material->_additive_blending, curr_x + label_width, curr_y + checkbox_offset, params).get_checked();
	curr_y += line_height;
}

#undef DRAW_RAND_FLOAT
#undef DRAW_RAND_COLOR

void ga_particles_component::delete_all_buffers()
{
	glDeleteBuffers(NUM_OF_VBOS, _vbos);
	glDeleteVertexArrays(1, &_vao);
	delete[] _particles;
	delete[] _gpu_pos_size_data;
	delete[] _gpu_color_data;
}

void ga_particles_component::generate_all_buffers(int max_particles)
{
	static const GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f,
		+0.5f, -0.5f, 0.0f,
		-0.5f, +0.5f, 0.0f,
		+0.5f, +0.5f, 0.0f,
	};
	static const GLfloat texture_coords[] = {
		0, 0,
		1, 0,
		0, 1,
		1, 1,
	};
	static GLushort indices[] = {
		0, 1, 3, 3, 2, 0
	};

	_index_count = uint32_t(sizeof(indices) / sizeof(*indices));

	_max_particles = max_particles;
	_particles = new ga_particle[_max_particles]();
	_gpu_pos_size_data = new ga_vec4f[_max_particles];
	_gpu_color_data = new ga_vec4f[_max_particles];

	_last_used = 0;

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glGenBuffers(NUM_OF_VBOS, _vbos);

	glBindBuffer(GL_ARRAY_BUFFER, _vbos[VERT]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribDivisor(0, 0); // Same for every instance

	glBindBuffer(GL_ARRAY_BUFFER, _vbos[TEX_COORD]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texture_coords), texture_coords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribDivisor(1, 0); // Same for every instance

	glBindBuffer(GL_ARRAY_BUFFER, _vbos[POS_SIZE]);
	glBufferData(GL_ARRAY_BUFFER, _max_particles * sizeof(ga_vec4f), NULL, GL_STREAM_DRAW);
	// Empty, will be streamed.
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);
	glVertexAttribDivisor(2, 1); // One pos+size per instance

	glBindBuffer(GL_ARRAY_BUFFER, _vbos[COLOR]);
	glBufferData(GL_ARRAY_BUFFER, _max_particles * sizeof(ga_vec4f), NULL, GL_STREAM_DRAW);
	// Empty, will be streamed.
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(3);
	glVertexAttribDivisor(3, 1); // One color per instance

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbos[INDICES]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
}

ga_particle_material::ga_particle_material(const char* texture_file) : _texture_file(texture_file)
{
}

ga_particle_material::~ga_particle_material()
{
}

bool ga_particle_material::init()
{
	std::string source_vs;
	load_shader("data/shaders/ga_unlit_particle_vert.glsl", source_vs);

	std::string source_fs;
	load_shader("data/shaders/ga_unlit_particle_frag.glsl", source_fs);

	_vs = new ga_shader(source_vs.c_str(), GL_VERTEX_SHADER);
	if (!_vs->compile())
	{
		std::cerr << "Failed to compile vertex shader:" << std::endl << _vs->get_compile_log() << std::endl;
	}

	_fs = new ga_shader(source_fs.c_str(), GL_FRAGMENT_SHADER);
	if (!_fs->compile())
	{
		std::cerr << "Failed to compile fragment shader:\n\t" << std::endl << _fs->get_compile_log() << std::endl;
	}

	_program = new ga_program();
	_program->attach(*_vs);
	_program->attach(*_fs);
	if (!_program->link())
	{
		std::cerr << "Failed to link shader program:\n\t" << std::endl << _program->get_link_log() << std::endl;
	}

	_texture = new ga_texture();
	if (!_texture->load_from_file(_texture_file.c_str()))
	{
		std::cerr << "Failed to load test.bmp" << std::endl;
	}

	return true;
}

void ga_particle_material::bind(const ga_mat4f& view, const ga_mat4f& proj, const ga_mat4f& transform)
{
	ga_uniform view_uniform = _program->get_uniform("u_view");
	ga_uniform proj_uniform = _program->get_uniform("u_proj");
	ga_uniform texture_uniform = _program->get_uniform("u_texture");

	_program->use();

	view_uniform.set(view);
	proj_uniform.set(proj);
	texture_uniform.set(*_texture, 0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, _additive_blending ? GL_ONE : GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
}
