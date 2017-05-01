/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "framework/ga_camera.h"
#include "framework/ga_compiler_defines.h"
#include "framework/ga_input.h"
#include "framework/ga_sim.h"
#include "framework/ga_output.h"
#include "jobs/ga_job.h"
#include "gui/ga_font.h"

#include "entity/ga_entity.h"
#include "entity/ga_lua_component.h"
#include "graphics/ga_particles_component.h"
#include "graphics/ga_cube_component.h"
#include "graphics/ga_program.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#if defined(GA_MINGW)
#include <unistd.h>
#endif

ga_font* g_font = nullptr;

static void set_root_path(const char* exepath);

int main(int argc, const char** argv)
{
	set_root_path(argv[0]);

	ga_job::startup(0xffff, 256, 256);

	// Create objects for three phases of the frame: input, sim and output.
	ga_input* input = new ga_input();
	ga_sim* sim = new ga_sim();
	ga_output* output = new ga_output(input->get_window());

	// Create camera.
	ga_camera* camera = new ga_camera({ 0.0f, 7.0f, 20.0f });
	ga_quatf rotation;
	rotation.make_axis_angle(ga_vec3f::y_vector(), ga_degrees_to_radians(180.0f));
	camera->rotate(rotation);
	rotation.make_axis_angle(ga_vec3f::x_vector(), ga_degrees_to_radians(15.0f));
	camera->rotate(rotation);

	// Create the default font:
	g_font = new ga_font("VeraMono.ttf", 16.0f, 512, 512);

	// Create a particle system whose movement is driven by Lua script.
	ga_entity particle_lua;
	particle_lua.translate({ 0.0f, 2.0f, 1.0f });
	ga_lua_component lua_move(&particle_lua, "data/scripts/move.lua");
	ga_particles_component particle_system(&particle_lua, "data/textures/default_particle.png");
	//ga_cube_component cube(&particle_lua, "data/textures/rpi.png");
	particle_system._draw_gui = true;
	sim->add_entity(&particle_lua);

	// Main loop:
	while (true)
	{
		// We pass frame state through the 3 phases using a params object.
		ga_frame_params params;

		// Gather user input and current time.
		if (!input->update(&params))
		{
			break;
		}

		// Update the camera.
		camera->update(&params);

		// Run gameplay.
		sim->update(&params);

		// Perform the late update.
		sim->late_update(&params);

		ga_dynamic_drawcall axes;
		axes._name = "axes";
		axes._transform.make_identity();
		axes._draw_mode = GL_LINES;

		axes._positions = { {0, 0, 0}, {1, 0, 0} };
		axes._indices = { 0, 1 };
		axes._color = { 1, 0, 0 };
		params._dynamic_drawcalls.push_back(axes);

		axes._positions = { { 0, 0, 0 },{ 0, 1, 0 } };
		axes._indices = { 0, 1 };
		axes._color = { 0, 1, 0 };
		params._dynamic_drawcalls.push_back(axes);

		axes._positions = { { 0, 0, 0 },{ 0, 0, 1 } };
		axes._indices = { 0, 1 };
		axes._color = { 0, 0, 1 };
		params._dynamic_drawcalls.push_back(axes);

		// Draw to screen.
		output->update(&params);
	}

	delete output;
	delete sim;
	delete input;
	delete camera;

	ga_job::shutdown();

	return 0;
}

char g_root_path[256];
static void set_root_path(const char* exepath)
{
#if defined(GA_MSVC)
	strcpy_s(g_root_path, sizeof(g_root_path), exepath);

	// Strip the executable file name off the end of the path:
	char* slash = strrchr(g_root_path, '\\');
	if (!slash)
	{
		slash = strrchr(g_root_path, '/');
	}
	if (slash)
	{
		slash[1] = '\0';
	}
#elif defined(GA_MINGW)
	char* cwd;
	char buf[PATH_MAX + 1];
	cwd = getcwd(buf, PATH_MAX + 1);
	strcpy_s(g_root_path, sizeof(g_root_path), cwd);

	g_root_path[strlen(cwd)] = '/';
	g_root_path[strlen(cwd) + 1] = '\0';
#endif
}
