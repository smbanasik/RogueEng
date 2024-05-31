#version 330 core
in vec2 tex_coords;
in vec3 text_color;
out vec4 color;

uniform sampler2D textu;

void main() {
    //vec4 vert_color = vec4(1.0, 1.0, 1.0, texture(textu, tex_coords).r);
    //color = vec4(text_color, 1.0) * vert_color;
    color = vec4(text_color.r, text_color.g, text_color.b, texture(textu, tex_coords).r);
}