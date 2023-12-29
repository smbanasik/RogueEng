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
    glm::vec3 color; // Unused for now
    glm::vec2 texture_coords;
};
typedef unsigned int Texture; // Unsigned int for now, but here in case we need to add more.

// TODO: will work for now but we will need to adjust this when I'm optimizing this
class Sprite {
public:

    Sprite(glm::vec2 position, glm::vec2 dimensions, Texture texture, glm::ivec2 atlas_coords);
    ~Sprite();
    Sprite(const Sprite& other);
    Sprite& operator=(const Sprite& other);
    Sprite(Sprite&& other) noexcept;
    Sprite& operator=(Sprite&& other) noexcept;

    void draw(ShaderProgram& shader);

    void translate(glm::vec3 translation);
    void scale(glm::vec3 transformation);
    void rotate(glm::vec3 transformation, float angle);
    void reset_model_matrix();

    void set_color(glm::vec3 color);

    glm::mat4 get_model_matrix();

private:
    unsigned int VBO, VAO, EBO;
    unsigned int* ref_count_vao, *ref_count_vebo;
    glm::vec3 color;
    glm::mat4 model_matrix;
    Texture texture;

    std::array<Vertex, 8> verts;
    std::array<unsigned int, 12> indices;

    void initialize_sprite();
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