#version 410 core
//uniform sampler2D tex;
in vec4 ourColor;
//in vec2 vs_tex_coord;
out vec4 color;
void main() {
    color = ourColor;
    //color = texture(tex, vs_tex_coord);
}