// 11/29/2023
// Spencer Banasik
#include <renderable.hpp>

#include <string>
#include <vector>
#include <stdint.h>
#include <cstddef>

#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <shader.hpp>

using namespace uteng_render;

static constexpr unsigned int LAYOUT_POSITION = 0;
static constexpr unsigned int LAYOUT_COLOR = 1;
static constexpr unsigned int LAYOUT_TEXTURE = 2;

static constexpr unsigned int POSITION_OFFSET = 0;
static constexpr unsigned int COLOR_OFFSET = 3;
static constexpr unsigned int TEXTURE_OFFSET = 3 + 3;

const float TEXTURE_SIZE = 64;
const float TEXTURE_GRID_SIZE = 16;
const float TEXTURE_SUBDIVISION = TEXTURE_GRID_SIZE / TEXTURE_SIZE;

glm::vec2 calculate_atlas_location(glm::vec2 atlas_coords) {

    glm::vec2 normalized_coords(0.0f);

    normalized_coords.x = atlas_coords.x * TEXTURE_SUBDIVISION;
    normalized_coords.y = 1 - (atlas_coords.y * TEXTURE_SUBDIVISION);

    return normalized_coords;
}

Sprite::Sprite(glm::vec2 position, glm::vec2 dimensions, Texture texture, glm::ivec2 atlas_coords = { 0,0 }) :
    indices{ 0, 2, 1, 2, 3, 1,   4, 6, 5, 6, 7, 5 }, model_matrix(1.0f), texture(texture), color(1.0f) {

    glm::vec2 tex_coord = calculate_atlas_location(atlas_coords);

    // 1 3
    // 0 2
    verts[0] = Vertex({ position.x,                position.y,                0.0f }, { 1.0f, 1.0f, 1.0f }, { tex_coord.x, tex_coord.y});
    verts[1] = Vertex({ position.x,                position.y + dimensions.y, 0.0f }, { 1.0f, 1.0f, 1.0f }, { tex_coord.x, tex_coord.y - TEXTURE_SUBDIVISION});
    verts[2] = Vertex({ position.x + dimensions.x, position.y,                0.0f }, { 1.0f, 1.0f, 1.0f }, { tex_coord.x + TEXTURE_SUBDIVISION, tex_coord.y});
    verts[3] = Vertex({ position.x + dimensions.x, position.y + dimensions.y, 0.0f }, { 1.0f, 1.0f, 1.0f }, { tex_coord.x + TEXTURE_SUBDIVISION, tex_coord.y - TEXTURE_SUBDIVISION });

    verts[4] = Vertex({ position.x + dimensions.x,     position.y + dimensions.y,     0.0f }, { 1.0f, 1.0f, 1.0f }, { tex_coord.x, tex_coord.y });
    verts[5] = Vertex({ position.x + dimensions.x,     position.y + dimensions.y * 2, 0.0f }, { 1.0f, 1.0f, 1.0f }, { tex_coord.x, tex_coord.y - TEXTURE_SUBDIVISION });
    verts[6] = Vertex({ position.x + dimensions.x * 2, position.y + dimensions.y,     0.0f }, { 1.0f, 1.0f, 1.0f }, { tex_coord.x + TEXTURE_SUBDIVISION, tex_coord.y });
    verts[7] = Vertex({ position.x + dimensions.x * 2, position.y + dimensions.y * 2, 0.0f }, { 1.0f, 1.0f, 1.0f }, { tex_coord.x + TEXTURE_SUBDIVISION, tex_coord.y - TEXTURE_SUBDIVISION });

    ref_count_vao = new unsigned int;
    ref_count_vebo = new unsigned int;
    *ref_count_vao = 1;
    *ref_count_vebo = 1;

    initialize_sprite();
}

Sprite::~Sprite() {
    if (ref_count_vao == nullptr || ref_count_vebo == nullptr)
        return;

    (*ref_count_vao)--;
    (*ref_count_vebo)--;

    if (*ref_count_vao == 0) {
        glDeleteVertexArrays(1, &VAO);
        delete ref_count_vao;
    }
    if (*ref_count_vebo == 0) {
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        delete ref_count_vebo;
    }
}
Sprite::Sprite(const Sprite& other) {
    this->ref_count_vao = other.ref_count_vao;
    this->ref_count_vebo = other.ref_count_vebo;

    (*this->ref_count_vao)++;
    (*this->ref_count_vebo)++;

    this->VAO = other.VAO;
    this->VBO = other.VBO;
    this->EBO = other.EBO;
    this->color = other.color;
    this->model_matrix = other.model_matrix;
    this->texture = other.texture;
    this->verts = other.verts;
    this->indices = other.indices;
}
Sprite& Sprite::operator=(const Sprite& other) {
    (*this->ref_count_vao)--;
    (*this->ref_count_vebo)--;

    if (*this->ref_count_vao == 0) {
        glDeleteVertexArrays(1, &VAO);
        delete this->ref_count_vao;
    }
    if (*this->ref_count_vebo == 0) {
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        delete this->ref_count_vebo;
    }

    this->ref_count_vao = other.ref_count_vao;
    this->ref_count_vebo = other.ref_count_vebo;

    (*this->ref_count_vao)++;
    (*this->ref_count_vebo)++;

    this->VAO = other.VAO;
    this->VBO = other.VBO;
    this->EBO = other.EBO;
    this->color = other.color;
    this->model_matrix = other.model_matrix;
    this->texture = other.texture;
    this->verts = other.verts;
    this->indices = other.indices;

    return *this;
}
Sprite::Sprite(Sprite&& other) noexcept {
    this->ref_count_vao = other.ref_count_vao;
    this->ref_count_vebo = other.ref_count_vebo;

    this->VAO = other.VAO;
    this->VBO = other.VBO;
    this->EBO = other.EBO;
    this->color = other.color;
    this->model_matrix = other.model_matrix;
    this->texture = other.texture;
    this->verts = other.verts;
    this->indices = other.indices;

    other.ref_count_vao = nullptr;
    other.ref_count_vebo = nullptr;
    other.VAO = 0;
    other.VBO = 0;
    other.EBO = 0;
}
Sprite& Sprite::operator=(Sprite&& other) noexcept {
    (*this->ref_count_vao)--;
    (*this->ref_count_vebo)--;

    if (*this->ref_count_vao == 0) {
        glDeleteVertexArrays(1, &VAO);
        delete this->ref_count_vao;
    }
    if (*this->ref_count_vebo == 0) {
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        delete this->ref_count_vebo;
    }

    this->ref_count_vao = other.ref_count_vao;
    this->ref_count_vebo = other.ref_count_vebo;

    this->VAO = other.VAO;
    this->VBO = other.VBO;
    this->EBO = other.EBO;
    this->color = other.color;
    this->model_matrix = other.model_matrix;
    this->texture = other.texture;
    this->verts = other.verts;
    this->indices = other.indices;

    other.ref_count_vao = nullptr;
    other.ref_count_vebo = nullptr;
    other.VAO = 0;
    other.VBO = 0;
    other.EBO = 0;

    return *this;
}

void Sprite::draw(ShaderProgram& shader) {

    shader.use();

    shader.set_int("our_tex", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    //shader.set_vec3("color", color);

    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Sprite::translate(glm::vec3 transformation) {
    model_matrix = glm::translate(model_matrix, transformation);
}
void Sprite::scale(glm::vec3 transformation) {
    model_matrix = glm::scale(model_matrix, transformation);
}
void Sprite::rotate(glm::vec3 transformation, float angle) {
    model_matrix = glm::rotate(model_matrix, angle, transformation);
}
void Sprite::reset_model_matrix() {
    model_matrix = glm::mat4(1.0f);
}

void Sprite::set_color(glm::vec3 color) {
    this->color = color;
}

glm::mat4 Sprite::get_model_matrix() {
    return model_matrix;
}

void Sprite::initialize_sprite() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * verts.size(), &verts[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
        &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(LAYOUT_POSITION);
    glVertexAttribPointer(LAYOUT_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)POSITION_OFFSET);

    glEnableVertexAttribArray(LAYOUT_COLOR);
    glVertexAttribPointer(LAYOUT_COLOR, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(COLOR_OFFSET * sizeof(float)));

    glEnableVertexAttribArray(LAYOUT_TEXTURE);
    glVertexAttribPointer(LAYOUT_TEXTURE, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(TEXTURE_OFFSET * sizeof(float)));

    glBindVertexArray(0);
}