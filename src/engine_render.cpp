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
#include <renderable.hpp>

static struct {
    std::unordered_map<std::string, uteng_render::ShaderProgram>* shader_pool;
    std::unordered_map<std::string, uteng_render::Texture>* texture_pool;
} uteng_render_vars;

void load_shaders(void) {
    
    uteng_render::Shader vertex("../shaders/default.vert");
    uteng_render::Shader fragment("../shaders/texture_color.frag");

    uteng_render_vars.shader_pool->emplace(std::make_pair(std::string("default"), uteng_render::ShaderProgram(vertex.get_id(), fragment.get_id())));

    return;
}

// TODO: given a vector of strings, load textures for those strings
void load_textures(void) {

    (*uteng_render_vars.texture_pool)["temp_sprite"] = 0;
    uteng_render::Texture* texture = &(*uteng_render_vars.texture_pool)["temp_sprite"];

    int width, height, channel_num;
    unsigned char* image_data = stbi_load("../assets/test_atlas.png", &width, &height, &channel_num, 0);

    if (image_data) {
        glGenTextures(1, texture);
        glBindTexture(GL_TEXTURE_2D, *texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
        // * Texture target, mipmap level, texture format, w & h, 0 for legacy things, format and data type of source image, actual data
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    return;
}

void uteng::Engine::init_render(void) {

    uteng_render_vars.shader_pool = &shader_pool;
    uteng_render_vars.texture_pool = &texture_pool;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glCullFace(GL_CCW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    load_shaders();

    stbi_set_flip_vertically_on_load(true);

    load_textures();

    // Set up a renderable for us to use

    dynamic_renderables.emplace_back(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), texture_pool["temp_sprite"], glm::ivec2(0, 0));
    //dynamic_renderables.emplace_back(glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), texture_pool["temp_sprite"], glm::ivec2(1, 0));

    dynamic_renderables[0].scale({ 1.0, 1.0, 1.0 });

}

void uteng::Engine::run_render_loop() {

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 proj = glm::ortho(0.0f, 4.0f, 0.0f, 3.0f, 0.0f, 10.0f);
    glm::mat4 view = camera.get_view_matrix();
    glm::mat4 proj_view = proj * view;

    uteng_render::ShaderProgram& shader = uteng_render_vars.shader_pool->at("default");

    for (auto it = dynamic_renderables.begin(); it != dynamic_renderables.end(); it++) {
        
        shader.set_mat4("transform", proj_view * it->get_model_matrix());
        it->draw(shader);

    }
    for (auto it = static_renderables.begin(); it != static_renderables.end(); it++) {

        shader.set_mat4("transform", proj_view * it->get_model_matrix());
        it->draw(shader);

    }

}