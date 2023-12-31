// 11/20/2023
// Spencer Banasik

// Renderable stores all of the structures for our render objects
#ifndef UT_RENDERABLE_HPP
#define UT_RENDERABLE_HPP

#include <string>
#include <vector>
#include <array>

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

// TODO: CONST REF
class Sprite {
public:
    Sprite(ShaderProgram* shader, Texture* texture) : botleft_pos(0.0f), dimensions(0.0f), palette_colors(1.0f), atlas_coords(0), shader(shader), texture(texture) {};
    Sprite(glm::vec2 botleft, glm::vec2 dimensions, glm::vec3 palette, glm::ivec2 atlas, ShaderProgram* shader, Texture* texture)
         : botleft_pos(botleft), dimensions(dimensions), palette_colors(palette), atlas_coords(atlas), shader(shader), texture(texture) {};
    glm::vec2 botleft_pos;
    glm::vec2 dimensions;
    glm::vec3 palette_colors;
    glm::ivec2 atlas_coords;
    uteng_render::ShaderProgram * shader;
    uteng_render::Texture * texture;
};
// TODO: CONST REF
class Model {
public:

    Model(const std::vector<Sprite>& sprites, Texture* palette);
    ~Model();
    Model(const Model& other);
    Model& operator=(const Model& other);
    Model(Model&& other) noexcept;
    Model& operator=(Model&& other) noexcept;

    void set_color(glm::vec3 color);
    void prepare_sprites();
    void draw(glm::mat4 transform);

    std::vector<Sprite> sprites;
private:
    unsigned int VBO, VAO, EBO;
    unsigned int* ref_count;
    glm::vec3 uni_color;
    Texture* palette;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    std::vector<Texture*> sprite_textures;
    std::vector<ShaderProgram*> sprite_shaders;

    void bind_buffer();
};

// TODO: cpp of this once we figure out how sprites work better
//class Model {
//public:
//
//    Model(std::vector<Sprite> sprites, ShaderProgram* shader);
//
//    void translate_model();
//    void scale_model();
//    void rotate_model();
//
//    glm::mat4 get_model_matrix();
//
//private:
//    std::vector<Sprite> sprites;
//    glm::mat4 model_matrix;
//    ShaderProgram* shader;
//};

}

#endif