#version 330 core
out vec4 FragColor;

in vec3 our_color;
in vec2 texture_coord;

uniform sampler2D texture_0;
uniform sampler2D texture_1;

void main() {
    FragColor = mix(texture(texture_0, texture_coord), texture(texture_1, texture_coord), 0.2);
};