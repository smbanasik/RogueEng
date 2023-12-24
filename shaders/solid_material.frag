#version 330 core
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
};

out vec4 FragColor;

in vec3 normal;
in vec3 frag_pos;

uniform Material material;
uniform Light light;

void main() {
    // ambient lighting
    vec3 ambient_light = light.ambient * material.ambient;
    
    // diffuse lighting
    vec3 norm = normalize(normal);
    vec3 light_direction = normalize(light.position - frag_pos);
    float diff = max(dot(norm, light_direction), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
    // specular lighting
    vec3 view_dir = normalize(-frag_pos);
    vec3 reflect_dir = reflect(-light_direction, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);
    
    vec3 result = ambient_light + diffuse + specular;
    FragColor = vec4(result, 1.0);
    
};