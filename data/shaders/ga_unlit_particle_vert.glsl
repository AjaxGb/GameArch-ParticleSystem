#version 400

uniform mat4 u_view;
uniform mat4 u_proj;

layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec2 in_tex_coord;
layout(location = 2) in vec4 in_pos_size;
layout(location = 3) in vec4 in_color;

out vec4 color;
out vec2 tex_coord;

void main(void)
{
	mat4 view = u_view;
	
	view[0][0] = 1.0;
	view[0][1] = 0.0;
	view[0][2] = 0.0;
	
	view[1][0] = 0.0;
	view[1][1] = 1.0;
	view[1][2] = 0.0;
	
	view[2][0] = 0.0;
	view[2][1] = 0.0;
	view[2][2] = 1.0;
	
	vec4 pos = vec4((in_pos_size.xyz + (in_vertex * in_pos_size.w)), 1.0);
	gl_Position = pos * view * u_proj;
	
	color = in_color;
	tex_coord = in_tex_coord;
}