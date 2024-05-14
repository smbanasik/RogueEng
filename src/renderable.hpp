// 11/20/2023
// Spencer Banasik

// Renderable stores all of the structures for our render objects
#ifndef UT_RENDERABLE_HPP
#define UT_RENDERABLE_HPP

#include <string>
#include <vector>
#include <array>
#include <stdint.h>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace uteng_render {
class ShaderProgram;
struct Vertex {
    glm::vec3 position;
    glm::vec3 color_keys;
    glm::vec2 texture_coords;
};
enum TextureType {
    PALETTE,
    NORMAL,
};
struct Texture {
    unsigned int id;
    TextureType type;
    float texture_size;
    float grid_size;
};
class Sprite {
public:
    static uint32_t generate_id() {
        static uint32_t global_id = 1;
        return global_id++;
    }
    Sprite(uint32_t id, ShaderProgram* shader, Texture* texture) : id(id), botleft_pos(0.0f), dimensions(0.0f), palette_colors(1.0f), atlas_coords(0), shader(shader), texture(texture) {};
    Sprite(const glm::vec2& botleft, const glm::vec2& dimensions, const glm::vec3& palette, const glm::ivec2& atlas, uint32_t id, ShaderProgram* shader, Texture* texture)
         : botleft_pos(botleft), dimensions(dimensions), palette_colors(palette), atlas_coords(atlas), id(id), shader(shader), texture(texture) {};
    glm::vec2 botleft_pos;
    glm::vec2 dimensions;
    glm::vec3 palette_colors;
    glm::ivec2 atlas_coords;
    uteng_render::ShaderProgram * shader;
    uteng_render::Texture * texture;
    uint32_t id;
};
// Model class. If you have modified the sprites, call prepare_sprites() before calling draw()
class Model {
public:

    Model(const std::vector<Sprite>& sprites, Texture* palette);
    ~Model();
    Model(const Model& other);
    Model& operator=(const Model& other);
    Model(Model&& other) noexcept;
    Model& operator=(Model&& other) noexcept;

    void set_model_color(const glm::vec3& color);
    void sprite_set_texture(const size_t id, const glm::ivec2& atlas_coords);
    void sprite_set_color(const size_t id, const glm::vec3& palette_keys);
    void sprite_set_position(const size_t id, const glm::vec2 coordinates);
    void sprite_set_scale(const size_t id, const glm::vec2 dimensions);
    void sprite_set_layer(const size_t id, const float layer);
    // TODO: do these later since they are more complicated
    //void sprite_set_rotate(const size_t id, const glm::vec2 rotation);
    //void sprite_set_flip(const size_t id, const bool horizontal = true);
    void draw(const glm::mat4& transform);

private:
    bool sprite_changed;
    size_t verts_changed_min, verts_changed_max;

    unsigned int VBO, VAO, EBO;
    unsigned int* ref_count;
    glm::vec3 uni_color;
    Texture* palette;

    std::vector<Sprite> sprites;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    std::vector<Texture*> sprite_textures;
    std::vector<ShaderProgram*> sprite_shaders;

    void prepare_sprites();
    void bind_buffer();
    // Converts a sprite ID to an index for the sprites vector
    // Returns vector size on failed lookup
    size_t find_sprite(size_t id);
    void copy_vars(const Model& other);
    void set_sprite_changed(size_t vert_idx);
    void handle_sprite_changed();
};

}

#endif