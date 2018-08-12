#version 410 core
layout (location = 0) in vec2 in_tex_coord;
layout (location = 1) in vec3 position;
uniform mat4 mvp;
out vec2 vs_tex_coord;
void main() {
	gl_Position = mvp*vec4(position, 1.0f);
	vs_tex_coord = in_tex_coord;
}