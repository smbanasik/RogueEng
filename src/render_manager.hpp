// 5/23/2024
// Spencer Banasik

// Stores what will go onto the screen and provides an interface for modifying it
// Does not determine what will go onto screen and what won't.
#ifndef UT_RENDER_MANAGER_HPP
#define UT_RENDER_MANAGER_HPP

#include <unordered_map>

#include <shader.hpp>
#include <renderable.hpp>

namespace uteng_render {

enum TexType {
    TWO_DIM,
    ONE_DIM,
};
class RenderManager {
public:

    // Load a texture, the keyword is used to acces the texture.
    void load_texture(const std::string& keyword, TexType type, const std::string& path, GLint param);
    // Unload a texture according to the keyword
    void unload_texture(const std::string& keyword);

    // Load a shader, the keyword is used to access the shader
    void load_shader(const std::string& keyword, const std::string& vertex_path, const std::string& fragment_path);
    // Unload a shader
    void unload_shader(const std::string& keyword);

    // Get a pointer to a texture according to the keyword
    Texture* get_texture(const std::string& keyword);

    // Get a pointer to a shader according to the keyword
    Shader* get_shader(const std::string& keyword);

    // Add a model to be rendered which is tied to the world
    void append_dyn_model(const Model* model);
    // Remove a model from the end of the container
    void remove_dyn_model(unsigned int offset_from_end = 0);

    // Add a model to be rendered which is tied to the camera
    void append_static_model(const Model* model);
    // Remove a model from the end of the container
    void remove_static_model(unsigned int offset_from_end = 0);

    // Clear the dynamic container
    void clear_dyn();
    // Clear the static container
    void clear_static();

    // Reserve memory for a number of elements
    void reserve_dyn(unsigned int num_elems);
    // Reserve memory for a number of elements
    void reserve_static(unsigned int num_elems);
    // Trim memory
    void trim_dyn();
    // Trim memory
    void trim_static();

    // Draw the dynamic and static models
    void draw();

private:

    void load_texture_sprite(const std::string& name, const std::string& path, GLint param);
    void load_texture_palette(const std::string& name, const std::string& path, GLint param);

    std::vector<uteng_render::Model*> dynamic_renderables; // Can fall off screen, attached to world
    std::vector<uteng_render::Model*> static_renderables; // Always on screen, attached to camera

    std::unordered_map<std::string, uteng_render::ShaderProgram> shader_pool;
    std::unordered_map<std::string, uteng_render::Texture>       texture_pool;

};
}
#endif