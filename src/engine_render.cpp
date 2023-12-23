#include <engine.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdint.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <util.hpp>
#include <shader.hpp>

// TODO: go back to texture page when doing renderable

// TODO: How do we want to handle VAOs and VBOs
// TODO: this will need copy constructors or copy assignment.
class BufferSet {
public:

    BufferSet() {
        id.vao = 0;
        id.vbo = 0;
        id.ebo = 0;
    }

    ~BufferSet() {
        if (id.vao == 0)
            return;
        glDeleteVertexArrays(1, &id.vao);
        glDeleteBuffers(1, &id.vbo);
        glDeleteBuffers(1, &id.ebo);
    }

    unsigned int get_vao() {
        return id.vao;
    }
    unsigned int get_vbo() {
        return id.vbo;
    }
    unsigned int get_ebo() {
        return id.ebo;
    }

    bool initialize() {
        if (verts.size() == 0)
            return false;

        glGenVertexArrays(1, &id.vao);
        glBindVertexArray(id.vao);

        glGenBuffers(1, &id.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, id.vbo);
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), &verts[0], GL_STATIC_DRAW);

        //glGenBuffers(1, &id.ebo);
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id.ebo);
        //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        constexpr int LAYOUT_VERTEX = 0, LAYOUT_COLOR = 1, LAYOUT_TEXTURE = 2;
        constexpr int LAYOUT_SIZE = 3 + 2;
        // TODO: I don't like the magic numbers we use here. I mitigated it a little bit with layout position
        glVertexAttribPointer(LAYOUT_VERTEX, 3, GL_FLOAT, GL_FALSE, LAYOUT_SIZE * sizeof(float), (void*)0);
        glEnableVertexAttribArray(LAYOUT_VERTEX);

        //glVertexAttribPointer(LAYOUT_COLOR, 3, GL_FLOAT, GL_FALSE, LAYOUT_SIZE * sizeof(float), (void*)(3 * sizeof(float)));
        //glEnableVertexAttribArray(LAYOUT_COLOR);

        glVertexAttribPointer(LAYOUT_TEXTURE, 2, GL_FLOAT, GL_FALSE, LAYOUT_SIZE * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(LAYOUT_TEXTURE);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        return true;
    }

    std::vector<float> verts;
    std::vector<unsigned int> indices;
    unsigned int tex_0;
    unsigned int tex_1;
private:
    struct {
        unsigned int vao;
        unsigned int vbo;
        unsigned int ebo;
    } id;
};

static struct CoreVars {
    BufferSet left;
    // TODO: initializing a shader class is weird right now
    std::unique_ptr<uteng_render::Shader> shader_base;
    std::vector<glm::vec3> cube_positions;
    
} core_data;

// TODO: given a vector of strings, load textures for those strings
void load_textures(void) {
    return;
}

void uteng::Engine::init_render(void) {

    glEnable(GL_DEPTH_TEST);

    // -- Shader Initialization --
    core_data.shader_base = std::unique_ptr<uteng_render::Shader>(
        new uteng_render::Shader("../shaders/default.vert", "../shaders/default.frag"));

    // -- TEXTURE SECTION --
    // -- Texture Loading --
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char* im_data = stbi_load("../assets/container.jpg", &width, &height, &nrChannels, 0); // * Returns width, height, number of color channels, and handle
    if (im_data) {
        glGenTextures(1, &core_data.left.tex_0); // TODO: Textures are also referenced with an ID, note this for our renderable
        glBindTexture(GL_TEXTURE_2D, core_data.left.tex_0);
        // set the texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, im_data);
        // * Texture target, mipmap level, texture format, w & h, 0 for legacy things, format and data type of source image, actual data
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "ITS BROKEN\n";
    }
    stbi_image_free(im_data);
    im_data = stbi_load("../assets/awesomeface.png", &width, &height, &nrChannels, 0); // * Returns width, height, number of color channels, and handle
    if (im_data) {
        glGenTextures(1, &core_data.left.tex_1); // TODO: Textures are also referenced with an ID, note this for our renderable
        glBindTexture(GL_TEXTURE_2D, core_data.left.tex_1);
        // set the texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, im_data);
        // * Texture target, mipmap level, texture format, w & h, 0 for legacy things, format and data type of source image, actual data
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "ITS BROKEN TWO\n";
    }
    stbi_image_free(im_data);

    core_data.shader_base->use();
    core_data.shader_base->setInt("texture_0", 0);
    core_data.shader_base->setInt("texture_1", 1);

    // END TEXTURE SECTION
    
    // -- Vertex and Buffer Initialization --
    
    core_data.left.verts = {
        // Positions,         texture coords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    //core_data.left.indices = {
    //    0, 1, 3,
    //    1, 2, 3,
    //};

    core_data.left.initialize();

    core_data.cube_positions = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    // draw wireframe polygons
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);s
}

// TODO: Currently tutorial code rn
void uteng::Engine::run_render_loop() {

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, core_data.left.tex_0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, core_data.left.tex_1);

    core_data.shader_base->use();
    
    glBindVertexArray(core_data.left.get_vao());

    // TODO: variables with aspect ratio, calculate on screen change
    glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), static_cast<float>(options.screen_width) / static_cast<float>(options.screen_height), 0.1f, 100.0f);
    glm::mat4 view = camera.get_view_matrix();

    // TODO: for every item in renderables, render it
    for (unsigned int i = 0; i < 10; i++) {

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime() * 5.5f * glm::radians(2.0f) * (i % 3 == 0), glm::vec3(0.0f, 1.0f, 1.0f));
        model = glm::translate(model, core_data.cube_positions[i]);
        float angle = 20.f * i;
        model = glm::rotate(model, (float)glfwGetTime() * 1.5f * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

        // TODO: It is most efficient to calculate MVP here, but if we need to separate it later on we should precalculate MVP, MV, VP, etc and send that to GPU as the shader needs them.
        glm::mat4 transform = projection * view * model;
        core_data.shader_base->setMat4("transform", transform);

        glDrawArrays(GL_TRIANGLES, 0, 36);

    }
}