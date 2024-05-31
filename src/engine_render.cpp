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
#include <text.hpp>

void uteng::Engine::load_shaders(void) {
    
    uteng_render::Shader vertex("../shaders/default.vert");
    uteng_render::Shader fragment("../shaders/texture_color.frag");

    shader_pool.emplace(std::make_pair(std::string("default"), uteng_render::ShaderProgram(vertex.get_id(), fragment.get_id())));

    uteng_render::Shader text_vert("../shaders/text.vert");
    uteng_render::Shader text_frag("../shaders/text.frag");

    shader_pool.emplace(std::make_pair(std::string("text"), uteng_render::ShaderProgram(text_vert.get_id(), text_frag.get_id())));

    return;
}

void uteng::Engine::load_texture_sprite(const std::string& name, const std::string& path, GLint param) {
    texture_pool[name] = uteng_render::Texture();
    uteng_render::Texture* texture = &texture_pool.at(name);

    int width, height, channel_num;
    unsigned char* image_data = stbi_load(path.c_str(), &width, &height, &channel_num, 0);

    if (image_data) {
        texture->texture_size = static_cast<float>(width);
        texture->grid_size = 16;
        texture->type = uteng_render::TextureType::NORMAL;
        glGenTextures(1, &texture->id);
        glBindTexture(GL_TEXTURE_2D, texture->id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    }
}
void uteng::Engine::load_texture_palette(const std::string& name, const std::string& path, GLint param) {
    texture_pool[name] = uteng_render::Texture();
    uteng_render::Texture* texture = &texture_pool.at(name);

    int width, height, channel_num;
    unsigned char* image_data = stbi_load(path.c_str(), &width, &height, &channel_num, 0);

    if (image_data) {
        texture->texture_size = static_cast<float>(width);
        texture->grid_size = 1;
        texture->type = uteng_render::TextureType::PALETTE;
        glGenTextures(1, &texture->id);
        glBindTexture(GL_TEXTURE_1D, texture->id);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, width, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    }
}

// TODO: given a vector of strings, load textures for those strings
void uteng::Engine::load_textures(void) {

    load_texture_sprite("temp_sprite", "../assets/filter_atlas.png", GL_NEAREST);
    load_texture_palette("temp_palette", "../assets/test_palette.png", GL_NEAREST);

    return;
}

void uteng::Engine::init_render(void) {

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //glCullFace(GL_CCW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    load_shaders();

    // Set up text manager here!
    text_manager.initialize(&ft, &shader_pool.at("text"));
    text_manager.add_font("arial", "../assets/fonts/arial.ttf");
    text_manager.set_text_font("arial");
    text_manager.set_text_position({ 400, 300 });
    text_manager.render_text("It's happening");
    //
    //text_manager.set_text_position({ 200, 200 });
    //text_manager.render_text("It's still happening");
    //
    //text_manager.set_text_position({ 0, 50 });
    //text_manager.set_text_color({ 0.0f, 0.5f, 1.0f });
    //text_manager.render_text("WOOP");

    stbi_set_flip_vertically_on_load(true);

    load_textures();

    // Set up a renderable for us to use

    std::vector<uteng_render::Sprite> sprites;
    sprites.push_back(uteng_render::Sprite(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f),
        glm::vec3(17.0f, 6.0f, 0.0f), glm::ivec2(2, 2), 0, &shader_pool.at("default"), &texture_pool.at("temp_sprite")));
    sprites.push_back(uteng_render::Sprite(glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f),
        glm::vec3(5.0f, 19.0f, 0.0f), glm::ivec2(0, 0), 1, &shader_pool.at("default"), &texture_pool.at("temp_sprite")));
    sprites.push_back(uteng_render::Sprite(glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f),
        glm::vec3(5.0f), glm::ivec2(0, 0), 2, &shader_pool.at("default"), &texture_pool.at("temp_sprite")));

    // Create the model
    dynamic_renderables.emplace_back(sprites, &texture_pool["temp_palette"]);

    dynamic_renderables[0].sprite_set_texture(1, glm::ivec2(2,0));
}

void uteng::Engine::run_render_loop() {

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 proj = glm::ortho(0.0f, 4.0f, 0.0f, 3.0f, 0.0f, 10.0f);
    glm::mat4 view = camera.get_view_matrix();
    glm::mat4 proj_view = proj * view * glm::mat4(1.0f);

    for (auto it = dynamic_renderables.begin(); it != dynamic_renderables.end(); it++) {
        
        it->draw(proj_view);

    }
    for (auto it = static_renderables.begin(); it != static_renderables.end(); it++) {

        it->draw(proj_view);

    }

    // 800 x 600 - Screen dimensions
    proj = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.0f, 10.0f);
    text_manager.draw(proj);

}

void uteng::Engine::prepare_renderables() {

    // TODO: modify the renderables so that if they are no longer on screen,
    // they should be removed.
    // Unsure how to do this yet.

}