#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 normal;
out vec3 frag_pos;
out vec2 texture_coords;

uniform mat4 transform;
uniform mat4 model_view;
uniform mat3 normal_matrix;

void main() {
    gl_Position = transform * vec4(aPos, 1.0);
    frag_pos = vec3(model_view * vec4(aPos, 1.0));
    normal = normal_matrix * aNormal;
    texture_coords = aTexCoords;
};