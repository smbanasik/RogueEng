#include <core_render.hpp>

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

#include <engine.hpp>
#include <util.hpp>
#include <shader.hpp>

// Organization:
// We want an init_core_render function that handles initialization of shaders
// and a static datastructure that manages our VBO and VAOs
// We want a run_core_loop function or something similar.
// We'll want a terminate_core_render function that handles removal of buffers
// We'll want a some interface function that lets us submit verts to the core

struct Camera {
    float camera_speed;
    glm::vec3 camera_position;
    glm::vec3 camera_front;
    glm::vec3 camera_up;
    glm::vec3 camera_right;
};

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
    std::unique_ptr<core_render::Shader> shader_base;
    std::vector<glm::vec3> cube_positions;
    Camera camera;
    
} core_data;

// TODO: given a vector of strings, load textures for those strings
void load_textures(void) {
    return;
}

// TODO: Temp, we'll need to use this elsewhere
void camera_movement(const uteng::KeyState& input_keys, Camera& camera) {
    const uint8_t KEY_W = 0, KEY_A = 1, KEY_S = 2, KEY_D = 3;
    if (input_keys.bmap_primary_keys.get_bit(KEY_W)) {
        camera.camera_position += camera.camera_speed * camera.camera_front;
    }
    if (input_keys.bmap_primary_keys.get_bit(KEY_A)) {
        camera.camera_position -= glm::normalize(glm::cross(camera.camera_front, camera.camera_up)) * camera.camera_speed;
    }
    if (input_keys.bmap_primary_keys.get_bit(KEY_S)) {
        camera.camera_position -= camera.camera_speed * camera.camera_front;
    }
    if (input_keys.bmap_primary_keys.get_bit(KEY_D)) {
        camera.camera_position += glm::normalize(glm::cross(camera.camera_front, camera.camera_up)) * camera.camera_speed;
    }
}

void core_render::init_render(void) {

    // -- Shader Initialization --
    core_data.shader_base = std::unique_ptr<core_render::Shader>(
        new Shader("../shaders/default.vert", "../shaders/default.frag"));

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

    glEnable(GL_DEPTH_TEST);

    // Done further down temporarily
    // Free image
    //stbi_image_free(im_data);
    
    // -- Vertex and Buffer Initialization --
    
    core_data.left.verts = {
        // Positions,         colors,         texture coords
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
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    core_data.camera.camera_speed = 0.00175;
    core_data.camera.camera_position = glm::vec3(0.0f, 0.0f, 3.0f);
    core_data.camera.camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
    core_data.camera.camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
}

// TODO: Currently tutorial code rn
void core_render::run_render_loop(const uteng::KeyState& input_keys) {

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    core_data.shader_base->use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, core_data.left.tex_0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, core_data.left.tex_1);
    glBindVertexArray(core_data.left.get_vao());

    camera_movement(input_keys, core_data.camera);
    glm::mat4 view = glm::lookAt(core_data.camera.camera_position, core_data.camera.camera_position + core_data.camera.camera_front,
        core_data.camera.camera_up);

    // TODO: variables with aspect ratio, calculate on screen change
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    // TODO: for every item in renderables, render it
    for (unsigned int i = 0; i < 10; i++) {

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime() * 5.5f * glm::radians(2.0f) * (i % 3 == 0), glm::vec3(0.0f, 1.0f, 1.0f));
        model = glm::translate(model, core_data.cube_positions[i]);
        float angle = 20.f * i;
        model = glm::rotate(model, (float)glfwGetTime() * 1.5f * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        //model = glm::rotate(model, (float)glfwGetTime() * 1.5f * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

        // TODO: It is most efficient to calculate MVP here, but if we need to separate it later on we should precalculate MVP, MV, VP, etc and send that to GPU as the shader needs them.
        glm::mat4 transform = projection * view * model;

        core_data.shader_base->setMat4("transform", transform);
        glDrawArrays(GL_TRIANGLES, 0, 36);

    }
}