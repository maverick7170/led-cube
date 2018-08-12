#version 410 core
uniform sampler2D tex;
in vec2 vs_tex_coord;
in vec2 vs_color;
out vec4 color;
void main() {
	if (vs_color[0] == 0.f) {
		color = texture(tex, vs_tex_coord); 
		if (color[0] >= 0.5f && color[1] <= 0.1f && color[2] <= 0.1f) {
			color[0] = 0.0f;
			color[3] = 0.0f;
		}
	} else {
		color = vec4(0.f,0.f,0.f,1.f);
	}   
}