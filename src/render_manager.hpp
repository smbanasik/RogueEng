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

    void load_texture(const std::string& keyword, TexType type, const std::string& path, GLint param);
    void unload_texture(const std::string& keyword);

    void load_shader(const std::string& keyword, const std::string& vertex_path, const std::string& fragment_path);
    void unload_shader(const std::string& keyword);

    void append_model(const Model& model, bool dynamic = true);
    void remove_model(unsigned int offset_from_end = 0);
    void clear();
    void reserve_mem(unsigned int num_elems, bool dynamic = true);
    void trim_mem();

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