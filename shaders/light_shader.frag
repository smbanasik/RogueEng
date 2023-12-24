#version 330 core
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
struct DirLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 direction;
};
struct PointLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
}
#define NR_POINT_LIGHTS 4


out vec4 FragColor;

in vec2 texture_coords;
in vec3 normal;
in vec3 frag_pos;

uniform Material material;
uniform DirLight dir_light;
uniform PointLight point_lights[NR_POINT_LIGHTS];

vec3 calc_dir_light(DirLight light, vec3 normal, vec3 view_direction);
vec3 calc_point_light(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_direction);

void main() {

    vec3 norm = normalize(normal);
    vec3 view_direction = normalize(-frag_pos);
    
    vec3 result = calc_dir_light(dir_light, norm, view_direction);
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += calc_point_light(point_lights[i], norm, frag_pos, view_direction
    
    FragColor = vec4(result, 1.0);
    
};

vec3 calc_dir_light(DirLight light, vec3 normal, vec3 view_direction) {
    vec3 light_direction = normalize(-light.direction);
    // diffuse
    float diff = max(dot(normal, light_direction), 0.0);
    // specular
    vec3 reflect_direction = reflect(-light_direction, normal);
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
    
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texture_coords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texture_coords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texture_coords));
    return (ambient + diffuse + specular);
};

vec3 calc_point_light(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_direction) {
    vec3 light_direction = normalize(light.position - frag_pos);
    // diffuse
    float diff = max(dot(normal, light_direction), 0.0);
    // specular
    vec3 reflect_direction = reflect(-light_direction, normal);
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texture_coords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texture_coords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texture_coords));
    return attenuation * (ambient + diffuse + specular);
};