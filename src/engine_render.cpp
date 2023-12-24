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

    bool initialize(bool use_normal = true, unsigned int vbo = 0) {
        if (verts.size() == 0 && vbo == 0)
            return false;

        glGenVertexArrays(1, &id.vao);
        glBindVertexArray(id.vao);
        if (vbo == 0) {
            glGenBuffers(1, &id.vbo);
            glBindBuffer(GL_ARRAY_BUFFER, id.vbo);
            glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), &verts[0], GL_STATIC_DRAW);
        }
        else {
            id.vbo = vbo;
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
        }
        //glGenBuffers(1, &id.ebo);
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id.ebo);
        //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        constexpr int LAYOUT_VERTEX = 0, LAYOUT_NORMAL = 1, LAYOUT_TEXTURE = 2;
        constexpr int LAYOUT_SIZE = 3 + 3 + 2;
        // TODO: I don't like the magic numbers we use here. I mitigated it a little bit with layout position
        glVertexAttribPointer(LAYOUT_VERTEX, 3, GL_FLOAT, GL_FALSE, LAYOUT_SIZE * sizeof(float), (void*)0);
        glEnableVertexAttribArray(LAYOUT_VERTEX);

        if (use_normal) {
            glVertexAttribPointer(LAYOUT_NORMAL, 3, GL_FLOAT, GL_FALSE, LAYOUT_SIZE * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(LAYOUT_NORMAL);

            glVertexAttribPointer(LAYOUT_TEXTURE, 2, GL_FLOAT, GL_FALSE, LAYOUT_SIZE * sizeof(float), (void*)(6 * sizeof(float)));
            glEnableVertexAttribArray(LAYOUT_TEXTURE);
        }
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
    BufferSet light;
    // TODO: initializing a shader class is weird right now
    std::unique_ptr<uteng_render::Shader> shader_base;
    std::unique_ptr<uteng_render::Shader> shader_light_source;
    std::vector<glm::vec3> cube_positions;
    glm::vec3 light_pos;
    
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

    core_data.shader_light_source = std::unique_ptr<uteng_render::Shader>(
        new uteng_render::Shader("../shaders/default.vert", "../shaders/light_source.frag"));

    // -- TEXTURE SECTION --
    // -- Texture Loading --
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char* im_data = stbi_load("../assets/container2.png", &width, &height, &nrChannels, 0); // * Returns width, height, number of color channels, and handle
    if (im_data) {
        glGenTextures(1, &core_data.left.tex_0); // TODO: Textures are also referenced with an ID, note this for our renderable
        glBindTexture(GL_TEXTURE_2D, core_data.left.tex_0);
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
        std::cout << "ITS BROKEN\n";
    }
    stbi_image_free(im_data);
    im_data = stbi_load("../assets/container2_specular.png", &width, &height, &nrChannels, 0); // * Returns width, height, number of color channels, and handle
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
    core_data.shader_base->setInt("material.diffuse", 0);
    core_data.shader_base->setInt("material.specular", 1);

    // END TEXTURE SECTION
    
    // -- Vertex and Buffer Initialization --
    
    core_data.left.verts = {
     // positions         // normals           // texture coords
       -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
       -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

       -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
       -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
       -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

       -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
       -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
       -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
       -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
       -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
       -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

       -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
       -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
       -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

       -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
       -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
       -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    };

    //core_data.left.indices = {
    //    0, 1, 3,
    //    1, 2, 3,
    //};

    core_data.left.initialize();

    //core_data.light.verts = core_data.left.verts;
    core_data.light.initialize(false, core_data.left.get_vbo());

    core_data.light_pos = glm::vec3(1.2f, 1.0f, 2.0f);

    // draw wireframe polygons
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);s
}

// TODO: Currently tutorial code rn
void uteng::Engine::run_render_loop() {

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, core_data.left.tex_0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, core_data.left.tex_1);

    glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), static_cast<float>(options.screen_width) / static_cast<float>(options.screen_height), 0.1f, 100.0f);
    glm::mat4 view = camera.get_view_matrix();

    // Render the light cube
    {
        core_data.light_pos = glm::vec3(sin(glfwGetTime()), 1.0f, sin(glfwGetTime()));
        core_data.shader_light_source->use();
        core_data.shader_light_source->setVec3("light_color", 1.0f, 1.0f, 1.0f);
        glBindVertexArray(core_data.light.get_vao());

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, core_data.light_pos);
        model = glm::scale(model, glm::vec3(0.2f));

        glm::mat4 transform = projection * view * model;
        core_data.shader_light_source->setMat4("transform", transform);
        
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // Render the other cube
    {
        core_data.shader_base->use();
        core_data.shader_base->setInt("material.diffuse", 0);
        core_data.shader_base->setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        core_data.shader_base->setFloat("material.shininess", 32.0f);

        core_data.shader_base->setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        core_data.shader_base->setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        core_data.shader_base->setVec3("light.specular", 1.0f, 1.0f, 1.0f); 
        core_data.shader_base->setVec3("light.position", glm::vec3(view * glm::vec4(core_data.light_pos,1.0)));

        glBindVertexArray(core_data.left.get_vao());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, core_data.left.tex_0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, core_data.left.tex_1);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0, 0.0, 0.0));
        model = glm::scale(model, glm::vec3(0.5f));

        // TODO: It is most efficient to calculate MVP here, but if we need to separate it later on we should precalculate MVP, MV, VP, etc and send that to GPU as the shader needs them.
        glm::mat4 model_view = view * model;
        glm::mat4 transform = projection * model_view;
        glm::mat3 normal_matrix = glm::mat3(glm::transpose(glm::inverse(model_view)));
        core_data.shader_base->setMat4("transform", transform);
        core_data.shader_base->setMat4("model_view", model_view);
        core_data.shader_base->setMat3("normal_matrix", normal_matrix);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

}