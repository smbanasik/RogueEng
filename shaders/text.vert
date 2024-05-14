#version 330 core
layout (location = 0) in vec2 vertex;
layout (location = 1) in vec2 atex_coords;

out vec2 tex_coords;

//uniform mat4 transform;

void main() {
    //gl_Position = transform * vec4(vertex, 0.0, 1.0);
    gl_Position = vec4(vertex, 0.0, 1.0);
    tex_coords = atex_coords;
};