#version 330 core
out vec4 FragColor;

in vec3 color;
in vec2 texture_coords;

uniform vec3 uni_color;
uniform sampler2D sprite_texture;
uniform sampler1D palette_texture;

void main() {

    vec4 color_a = texture(palette_texture, color.r);
    vec4 color_b = texture(palette_texture, color.g);
    vec4 color_c = texture(palette_texture, color.b);
    
    vec4 col_filter= texture(sprite_texture, texture_coords);
    
    vec4 final_color = color_a * col_filter.r + color_b * col_filter.g + color_c * col_filter.b;

    FragColor = vec4(uni_color, 1.0) * final_color;
};