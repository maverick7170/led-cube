#version 410 core
uniform sampler2D tex;
in vec2 vs_tex_coord;
in vec2 vs_color;
out vec4 color;
void main() {
	color = texture(tex, vs_tex_coord);    
}