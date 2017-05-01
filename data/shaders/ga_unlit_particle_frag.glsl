#version 400

uniform sampler2D u_texture;

in vec4 color;
in vec2 tex_coord;

void main(void)
{
	gl_FragColor = texture(u_texture, tex_coord) * color;
}