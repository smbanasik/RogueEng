// 11/29/2023
// Spencer Banasik
#include <renderable.hpp>

#include <string>
#include <vector>
#include <stdint.h>
#include <cstddef>
#include <array>
#include <algorithm>

#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <shader.hpp>

#include<iostream>

using namespace uteng_render;

static constexpr unsigned int LAYOUT_POSITION = 0;
static constexpr unsigned int LAYOUT_COLOR = 1;
static constexpr unsigned int LAYOUT_TEXTURE = 2;

static constexpr unsigned int POSITION_OFFSET = 0;
static constexpr unsigned int COLOR_OFFSET = 3;
static constexpr unsigned int TEXTURE_OFFSET = 3 + 3;

namespace {
struct SpriteSortCompare {
    bool operator()(const Sprite& first, const Sprite& second) {
        return (first.shader->get_id() > second.shader->get_id()) || (first.shader->get_id() == second.shader->get_id() && first.texture > second.texture);
    }
};
}

// Returns normalized coordinates. Z value contains the normalized grid size
glm::vec3 calculate_atlas_location(const Texture& texture, const glm::vec2& atlas_coords) {
    glm::vec3 normalized_coords(0.0f);
    normalized_coords.z = texture.grid_size / texture.texture_size;
    normalized_coords.x = atlas_coords.x * normalized_coords.z;
    normalized_coords.y = 1 - (atlas_coords.y * normalized_coords.z);
    return normalized_coords;
}

glm::vec3 calculate_palette_location(const Texture& palette, const glm::vec3& palette_colors) {
    return (palette_colors / palette.grid_size);
}

std::array<Vertex, 4> sprite_to_verts(const Sprite& sprite, const Texture* palette) {
    std::array<Vertex, 4> vertices;
    glm::vec3 normalized_coords = calculate_atlas_location(*sprite.texture, sprite.atlas_coords);

    //glm::vec3 normalized_colors = calculate_palette_location(*palette, sprite.palette_colors);
    glm::vec3 normalized_colors{ 1.0f };

    // 1 3
    // 0 2

    vertices[0] = { {sprite.botleft_pos.x, sprite.botleft_pos.y, 0.0f},
                    {normalized_colors},
                    {normalized_coords.x, normalized_coords.y} };
    vertices[1] = { {sprite.botleft_pos.x, sprite.botleft_pos.y + sprite.dimensions.y, 0.0f},
                    {normalized_colors},
                    {normalized_coords.x, normalized_coords.y - normalized_coords.z} };
    vertices[2] = { {sprite.botleft_pos.x + sprite.dimensions.x, sprite.botleft_pos.y, 0.0f},
                    {normalized_colors},
                    {normalized_coords.x + normalized_coords.z, normalized_coords.y} };
    vertices[3] = { {sprite.botleft_pos.x + sprite.dimensions.x, sprite.botleft_pos.y + sprite.dimensions.y, 0.0f},
                    {normalized_colors},
                    {normalized_coords.x + normalized_coords.z, normalized_coords.y - normalized_coords.z} };

    return vertices;
}

Model::Model(const std::vector<Sprite>& sprites, Texture* palette) :
    sprites(sprites), palette(palette), uni_color(1.0f) {

    VAO = VBO = EBO = 0;

    ref_count = new unsigned int;
    *ref_count = 1;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    vertices.reserve(sprites.size() * 4);
    indices.reserve(sprites.size() * 6);

    prepare_sprites();
}

Model::~Model() {
    if (ref_count == nullptr)
        return;

    (*ref_count)--;

    if (*ref_count) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        delete ref_count;
    }
}
Model::Model(const Model& other) {
    this->ref_count = other.ref_count;

    (*this->ref_count)++;

    this->VAO = other.VAO;
    this->VBO = other.VBO;
    this->EBO = other.EBO;
    this->uni_color = other.uni_color;
    this->palette = other.palette;
    this->vertices = other.vertices;
    this->indices = other.indices;
    this->sprite_shaders = other.sprite_shaders;
    this->sprite_textures = other.sprite_textures;
}
Model& Model::operator=(const Model& other) {
    (*this->ref_count)--;

    if (*this->ref_count == 0) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        delete this->ref_count;
    }

    this->ref_count = other.ref_count;

    (*this->ref_count)++;

    this->VAO = other.VAO;
    this->VBO = other.VBO;
    this->EBO = other.EBO;
    this->uni_color = other.uni_color;
    this->palette = other.palette;
    this->vertices = other.vertices;
    this->indices = other.indices;
    this->sprite_shaders = other.sprite_shaders;
    this->sprite_textures = other.sprite_textures;

    return *this;
}
Model::Model(Model&& other) noexcept {
    this->ref_count = other.ref_count;

    this->VAO = other.VAO;
    this->VBO = other.VBO;
    this->EBO = other.EBO;
    this->uni_color = other.uni_color;
    this->palette = other.palette;
    this->vertices = other.vertices;
    this->indices = other.indices;
    this->sprite_shaders = other.sprite_shaders;
    this->sprite_textures = other.sprite_textures;

    other.ref_count = nullptr;
    other.VAO = 0;
    other.VBO = 0;
    other.EBO = 0;
}
Model& Model::operator=(Model&& other) noexcept {
    (*this->ref_count)--;

    if (*this->ref_count == 0) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        delete this->ref_count;
    }

    this->ref_count = other.ref_count;

    this->VAO = other.VAO;
    this->VBO = other.VBO;
    this->EBO = other.EBO;
    this->uni_color = other.uni_color;
    this->palette = other.palette;
    this->vertices = other.vertices;
    this->indices = other.indices;
    this->sprite_shaders = other.sprite_shaders;
    this->sprite_textures = other.sprite_textures;

    other.ref_count = nullptr;
    other.VAO = 0;
    other.VBO = 0;
    other.EBO = 0;

    return *this;
}

void Model::set_color(glm::vec3 uniform_color) {
    this->uni_color = uniform_color;
}

void Model::prepare_sprites() {

    if (vertices.size() != 0 || indices.size() != 0) {
        vertices.clear();
        indices.clear();
    }

    std::sort(sprites.begin(), sprites.end(), SpriteSortCompare());

    for (auto sprite = sprites.begin(); sprite != sprites.end(); sprite++) {
        std::array<Vertex, 4> verts = sprite_to_verts(*sprite, palette);
        vertices.insert(vertices.end(), verts.begin(), verts.end());

        unsigned int count = indices.size();
        // 0, 2, 1, 2, 3, 1
        std::array<unsigned int, 6> new_ind = { count, count + 2, count + 1, count + 2, count + 3, count + 1 };
        indices.insert(indices.end(), new_ind.begin(), new_ind.end());

        sprite_textures.push_back(sprite->texture);
        sprite_shaders.push_back(sprite->shader);
    }

    bind_buffer();
}

void Model::draw(glm::mat4 transform) {

    unsigned int last_shader = 0, last_texture = 0, last_paletted = 0;
    bool switch_point = false;

    glBindVertexArray(VAO);

    sprite_shaders[0]->use();
    sprite_shaders[0]->set_mat4("transform", transform);
    sprite_shaders[0]->set_int("sprite_texture", 0);
    last_shader = sprite_shaders[0]->get_id();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sprite_textures[0]->id);
    last_texture = sprite_textures[0]->id;
    // TODO: palette

    for (unsigned int idx = 0, startidx = 0, endidx = 6;
        idx < sprite_shaders.size(); idx++, endidx += 6) {

        if (last_shader != sprite_shaders[idx]->get_id()) {
            sprite_shaders[idx]->use();
            sprite_shaders[idx]->set_mat4("transform", transform);
            sprite_shaders[idx]->set_int("sprite_texture", 0);
            last_shader = sprite_shaders[0]->get_id();
        }
        if (last_texture != sprite_textures[idx]->id) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, sprite_textures[idx]->id);
            last_texture = sprite_textures[idx]->id;
        }

        if (switch_point == true || (idx + 1) == sprite_shaders.size()) {
            glDrawElements(GL_TRIANGLES, (endidx - startidx) * sizeof(Vertex), GL_UNSIGNED_INT, (void*)(startidx * sizeof(Vertex)));
            switch_point = true;
            startidx = endidx;
        }

    }
    glBindVertexArray(0);
}

void Model::bind_buffer() {

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(LAYOUT_POSITION);
    glVertexAttribPointer(LAYOUT_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)POSITION_OFFSET);

    glEnableVertexAttribArray(LAYOUT_COLOR);
    glVertexAttribPointer(LAYOUT_COLOR, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(COLOR_OFFSET * sizeof(float)));

    glEnableVertexAttribArray(LAYOUT_TEXTURE);
    glVertexAttribPointer(LAYOUT_TEXTURE, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(TEXTURE_OFFSET * sizeof(float)));

    glBindVertexArray(0);
}