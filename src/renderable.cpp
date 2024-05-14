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
    normalized_coords.y = 1.0f - (atlas_coords.y * normalized_coords.z);
    return normalized_coords;
}
glm::vec3 calculate_palette_location(const Texture& palette, const glm::vec3& palette_colors) {
    return (palette_colors / palette.texture_size);
}
std::array<Vertex, 4> sprite_to_verts(const Sprite& sprite, const Texture* palette) {
    std::array<Vertex, 4> vertices;
    glm::vec3 normalized_coords = calculate_atlas_location(*sprite.texture, sprite.atlas_coords);

    glm::vec3 normalized_colors = calculate_palette_location(*palette, sprite.palette_colors);

    // 1 3
    // 0 2
    vertices[0] = { {sprite.botleft_pos.x, sprite.botleft_pos.y, 0.0f},
                    {normalized_colors},
                    {normalized_coords.x, normalized_coords.y - normalized_coords.z} };
    vertices[1] = { {sprite.botleft_pos.x, sprite.botleft_pos.y + sprite.dimensions.y, 0.0f},
                    {normalized_colors},
                    {normalized_coords.x, normalized_coords.y} };
    vertices[2] = { {sprite.botleft_pos.x + sprite.dimensions.x, sprite.botleft_pos.y, 0.0f},
                    {normalized_colors},
                    {normalized_coords.x + normalized_coords.z, normalized_coords.y - normalized_coords.z} };
    vertices[3] = { {sprite.botleft_pos.x + sprite.dimensions.x, sprite.botleft_pos.y + sprite.dimensions.y, 0.0f},
                    {normalized_colors},
                    {normalized_coords.x + normalized_coords.z, normalized_coords.y} };

    return vertices;
}
size_t idx_to_vert_idx(const size_t sprite_idx) {
    return sprite_idx * 4;
}
Model::Model(const std::vector<Sprite>& sprites, Texture* palette) :
    sprites(sprites), palette(palette), uni_color(1.0f), sprite_changed(false) {

    VAO = 0;
    VBO = 0;
    EBO = 0;

    ref_count = new unsigned int;
    *ref_count = 1;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    vertices.reserve(sprites.size() * 4);
    indices.reserve(sprites.size() * 6);

    verts_changed_min = sprites.size() * 4;
    verts_changed_max = 0;

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

    copy_vars(other);
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

    copy_vars(other);

    return *this;
}
Model::Model(Model&& other) noexcept {
    this->ref_count = other.ref_count;

    copy_vars(other);

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

    copy_vars(other);

    other.ref_count = nullptr;
    other.VAO = 0;
    other.VBO = 0;
    other.EBO = 0;

    return *this;
}
void Model::set_model_color(const glm::vec3& uniform_color) {
    this->uni_color = uniform_color;
}
void Model::sprite_set_texture(const size_t id, const glm::ivec2& atlas_coords) {
    size_t sprite_idx = find_sprite(id);
    size_t start_vert = idx_to_vert_idx(sprite_idx);
    glm::vec3 normalized_coords = calculate_atlas_location(*sprite_textures[sprite_idx], atlas_coords);
    vertices[start_vert].texture_coords = { normalized_coords.x, normalized_coords.y - normalized_coords.z };
    vertices[start_vert + 1].texture_coords = { normalized_coords.x, normalized_coords.y };
    vertices[start_vert + 2].texture_coords = { normalized_coords.x + normalized_coords.z, normalized_coords.y - normalized_coords.z };
    vertices[start_vert + 3].texture_coords = { normalized_coords.x + normalized_coords.z, normalized_coords.y };
    set_sprite_changed(start_vert);
}
void Model::sprite_set_color(const size_t id, const glm::vec3& palette_keys) {
    size_t sprite_idx = find_sprite(id);
    size_t start_vert = idx_to_vert_idx(sprite_idx);
    glm::vec3 palette_colors = calculate_palette_location(*palette, palette_keys);
    vertices[start_vert].color_keys = { palette_colors };
    vertices[start_vert + 1].color_keys = { palette_colors };
    vertices[start_vert + 2].color_keys = { palette_colors };
    vertices[start_vert + 3].color_keys = { palette_colors };
    set_sprite_changed(start_vert);
}
void Model::sprite_set_position(const size_t id, const glm::vec2 coordinates) {
    size_t sprite_idx = find_sprite(id);
    size_t start_vert = idx_to_vert_idx(sprite_idx);
    float x_diff = vertices[start_vert + 2].position.x - vertices[start_vert].position.x;
    float y_diff = vertices[start_vert + 1].position.y - vertices[start_vert].position.y;
    vertices[start_vert].position = glm::vec3(coordinates, vertices[start_vert].position.z);
    vertices[start_vert + 1].position = glm::vec3(coordinates.x, coordinates.y + y_diff, vertices[start_vert].position.z);
    vertices[start_vert + 2].position = glm::vec3(coordinates.x + x_diff, coordinates.y, vertices[start_vert].position.z);
    vertices[start_vert + 3].position = glm::vec3(coordinates.x + x_diff, coordinates.y + y_diff, vertices[start_vert].position.z);
    set_sprite_changed(start_vert);
}
void Model::sprite_set_scale(const size_t id, const glm::vec2 dimensions) {
    size_t sprite_idx = find_sprite(id);
    size_t start_vert = idx_to_vert_idx(sprite_idx);
    glm::vec2 bot_left = vertices[start_vert].position;
    vertices[start_vert + 1].position = glm::vec3(bot_left.x, bot_left.y + dimensions.y, vertices[start_vert].position.z);
    vertices[start_vert + 2].position = glm::vec3(bot_left.x + dimensions.x, bot_left.y, vertices[start_vert].position.z);
    vertices[start_vert + 3].position = glm::vec3(bot_left.x + dimensions.x, bot_left.y + dimensions.y, vertices[start_vert].position.z);
    set_sprite_changed(start_vert);
}
void Model::sprite_set_layer(const size_t id, const float layer) {
    size_t sprite_idx = find_sprite(id);
    size_t start_vert = idx_to_vert_idx(sprite_idx);
    vertices[start_vert].position.z = layer;
    vertices[start_vert + 1].position.z = layer;
    vertices[start_vert + 2].position.z = layer;
    vertices[start_vert + 3].position.z = layer;
    set_sprite_changed(start_vert);
}
//void Model::sprite_set_rotate(const size_t id, const glm::vec2 rotation) {
//    size_t sprite_idx = find_sprite(id);
//    size_t start_vert = idx_to_vert_idx(sprite_idx);
//    set_sprite_changed(start_vert);
//}
//void Model::sprite_set_flip(const size_t id, const bool horizontal) {
//    size_t sprite_idx = find_sprite(id);
//    size_t start_vert = idx_to_vert_idx(sprite_idx);
//    set_sprite_changed(start_vert);
//}
void Model::prepare_sprites() {

    if (vertices.size() != 0 || indices.size() != 0) {
        vertices.clear();
        indices.clear();
    }

    std::sort(sprites.begin(), sprites.end(), SpriteSortCompare());

    for (auto sprite = sprites.begin(); sprite != sprites.end(); sprite++) {
        std::array<Vertex, 4> verts = sprite_to_verts(*sprite, palette);
        vertices.insert(vertices.end(), verts.begin(), verts.end());

        unsigned int count = (indices.size() / 6) * 4;
        // 0, 2, 1, 2, 3, 1
        std::array<unsigned int, 6> new_ind = { count, count + 2, count + 1, count + 2, count + 3, count + 1 };
        indices.insert(indices.end(), new_ind.begin(), new_ind.end());

        sprite_textures.push_back(sprite->texture);
        sprite_shaders.push_back(sprite->shader);
    }

    bind_buffer();
}
void Model::draw(const glm::mat4& transform) {

    unsigned int last_shader = 0, last_texture = 0, last_palette = 0;
    bool switch_point = false;

    if (sprite_changed == true)
        handle_sprite_changed();

    glBindVertexArray(VAO);

    sprite_shaders[0]->use();
    sprite_shaders[0]->set_mat4("transform", transform);
    sprite_shaders[0]->set_int("sprite_texture", 0);
    sprite_shaders[0]->set_int("palette_texture", 1);
    sprite_shaders[0]->set_vec3("uni_color", uni_color);
    last_shader = sprite_shaders[0]->get_id();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sprite_textures[0]->id);
    last_texture = sprite_textures[0]->id;
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, palette->id);
    last_palette = palette->id;

    for (unsigned int idx = 0, startidx = 0, endidx = 6;
        idx < sprite_shaders.size(); idx++, endidx += 6) {

        if (last_shader != sprite_shaders[idx]->get_id()) {
            sprite_shaders[idx]->use();
            sprite_shaders[idx]->set_mat4("transform", transform);
            sprite_shaders[idx]->set_int("sprite_texture", 0);
            sprite_shaders[idx]->set_int("palette_texture", 1);
            last_shader = sprite_shaders[0]->get_id();
            switch_point = true;
        }
        if (last_texture != sprite_textures[idx]->id) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, sprite_textures[idx]->id);
            last_texture = sprite_textures[idx]->id;
            switch_point = true;
        }
        if (last_palette != palette->id) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_1D, palette->id);
            last_palette = palette->id;
            switch_point = true;
        }

        if (switch_point == true || (idx + 1) == sprite_shaders.size()) {
            glDrawElements(GL_TRIANGLES, (endidx - startidx) * sizeof(Vertex), GL_UNSIGNED_INT, (void*)(startidx * sizeof(Vertex)));
            switch_point = false;
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
size_t Model::find_sprite(size_t id) {
    for (size_t idx = 0; idx < sprites.size(); idx++) {
        if (sprites[idx].id == id)
            return idx;
    }
    return sprites.size();
}
void Model::copy_vars(const Model& other) {
    this->VAO = other.VAO;
    this->VBO = other.VBO;
    this->EBO = other.EBO;
    this->uni_color = other.uni_color;
    this->palette = other.palette;
    this->vertices = other.vertices;
    this->indices = other.indices;
    this->sprite_shaders = other.sprite_shaders;
    this->sprite_textures = other.sprite_textures;
    this->sprite_changed = other.sprite_changed;
    this->verts_changed_min = other.verts_changed_min;
    this->verts_changed_max = other.verts_changed_max;
}
void Model::set_sprite_changed(size_t vert_idx) {
    sprite_changed = true;
    if (verts_changed_max < vert_idx)
        verts_changed_max = vert_idx + 3;
    if (verts_changed_min > vert_idx)
        verts_changed_min = vert_idx;
}
void Model::handle_sprite_changed() {
    glBindVertexArray(VAO);
    glBufferSubData(GL_ARRAY_BUFFER, verts_changed_min * sizeof(Vertex),
        (1 + verts_changed_max - verts_changed_min) * sizeof(Vertex), &vertices[verts_changed_min]);

    glBindVertexArray(0);
    sprite_changed = false;
    verts_changed_max = 0;
    verts_changed_min = vertices.size();
}