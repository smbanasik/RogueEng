// 11/20/2023
// Spencer Banasik

// Renderable stores all of the structures for our render objects
#ifndef UT_RENDERABLE_HPP
#define UT_RENDERABLE_HPP

#include <string>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace uteng_render {
class Shader;
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texture_coords;
};
// TODO: Redesign, our texture needs an ID and type, but other things as well.
struct Texture {
    unsigned int id;
    std::string type;
};
// TODO: Redesign. Our mesh should contain everything we need to create a set of buffer objects.
// as well as the capabilities to draw these
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

}

#endif