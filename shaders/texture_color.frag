#version 330 core
out vec4 FragColor;

in vec3 color;
in vec2 texture_coords;

uniform sampler2D sprite_texture;

void main() {
    FragColor = vec4(color, 1.0) * texture(sprite_texture, texture_coords);
};