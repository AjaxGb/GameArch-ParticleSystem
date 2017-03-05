# ga2017-homework7
Seventh homework for RPI Game Architecture 2017.

In this homework you are tasked with integrating the Lua scripting language
into our game engine. You will do this by creating an entity component whose
update() method executes a user specified Lua script, in our case the Lua
file 'data/scripts/move.lua'.

We have created with an empty ga_lua_component to start. This component has
been placed on an entity and added to the simulation (see main.cpp).

* In the constructor you need to create a Lua environment, load the script,
and register methods to implement the following functions:
  * frame_params_get_input_left
  * frame_params_get_input_right
  * component_get_entity
  * entity_translate
* In the enginde-side update method you need to call the Lua-side update method.

When you're done with this homework, the cube should move left and right in
response to whatever input you designate as 'left' and 'right'. NOTE: ga_camera
has reserved WASD and the arrow keys for camera movement.

The source code for Lua is already being compiled into the engine. And there
are lots of resources online that describe how to integate Lua into a C/C++
application. Your job is to figure out how to do it for our engine.

NOTE: Although the Lua VM is register based internally, it was originally stack
based. As a result the Lua API is still stack based. So, for example, if you
want to call a Lua method that takes 2 parameters, you need to *push* each
parameter, and the method itself, before invoking the method.

##One Tip

This code will initialize Lua and load a script:

	lua_State* lua = luaL_newstate();
	luaL_openlibs(lua);

	extern char g_root_path[256];
	std::string fullpath = g_root_path;
	fullpath += path;

	int status = luaL_loadfile(lua, fullpath.c_str());
	if (status)
	{
		std::cerr << "Failed to load script " << path << ": " << lua_tostring(lua, -1);
		lua_close(lua);
	}
