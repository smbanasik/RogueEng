// 11/20/2023
// Spencer Banasik

// Renderable stores all of the structures for our render objects
#ifndef UT_RENDERABLE_HPP
#define UT_RENDERABLE_HPP

#include <string>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace core_render {
class Shader;
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texture_coords;
};
struct Texture {
    unsigned int id;
    std::string type;
};
class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void draw(Shader& shader);
private:
    unsigned int VBO, VAO, EBO;

    void setupMesh();
};
//class RenderSet {
//public:
//    RenderSet() {
//        buff_id.vao = 0;
//        buff_id.vbo = 0;
//        buff_id.ebo = 0;
//        //mesh = data_ptr;
//        texture = nullptr;
//        material = 0;
//
//        glGenVertexArrays(1, &buff_id.vao);
//        glBindVertexArray(buff_id.vao);
//
//        glGenBuffers(1, &buff_id.vbo);
//        glBindBuffer(GL_ARRAY_BUFFER, buff_id.vbo);
//        glBufferData(GL_ARRAY_BUFFER, mesh->get_data_size(), mesh->get_data(), GL_STATIC_DRAW);
//
//        glGenBuffers(1, &buff_id.ebo);
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buff_id.ebo);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->get_idx_size(), mesh->get_indices(), GL_STATIC_DRAW);
//
//        // TODO: import this data from shader
//        constexpr int LAYOUT_VERTEX = 0, LAYOUT_COLOR = 1;
//        glVertexAttribPointer(LAYOUT_VERTEX, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
//        glEnableVertexAttribArray(LAYOUT_VERTEX);
//
//        glVertexAttribPointer(LAYOUT_COLOR, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
//        glEnableVertexAttribArray(LAYOUT_COLOR);
//
//        glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//    }
//    ~RenderSet() {
//        if (mesh == nullptr)
//            return;
//        glDeleteVertexArrays(1, &buff_id.vao);
//        glDeleteBuffers(1, &buff_id.vbo);
//        glDeleteBuffers(1, &buff_id.ebo);
//    }
//
//    unsigned int get_vao() {
//        return buff_id.vao;
//    }
//    unsigned int get_vbo() {
//        return buff_id.vbo;
//    }
//    unsigned int get_ebo() {
//        return buff_id.ebo;
//    }
//
//private:
//    struct {
//        unsigned int vao;
//        unsigned int vbo;
//        unsigned int ebo;
//    } buff_id;
//    unsigned int material;
//    VBOData* mesh;
//    void* texture; // is an id that we'll add
//
//};
}

#endif