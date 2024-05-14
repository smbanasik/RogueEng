// 11/15/2023
// Spencer Banasik

// - engine.hpp & cpp: core engine, runs all functions, initializes things, creates the window
// The core engine of the game, this runs all functions such as render, input, and game logic.
// The engine handles window creation and options
#ifndef UT_ENGINE_HPP
#define UT_ENGINE_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <util.hpp>
#include <camera.hpp>
#include <shader.hpp>
#include <renderable.hpp>
#include <text.hpp>

struct GLFWwindow;
namespace uteng {

struct Options {
    int screen_width = 0;
    int screen_height = 0;
    bool fullscreen = false;
    std::string window_name;
};
struct KeyState {
    bool quit = false;
    uint8_t bmap_primary_keys = 0; // 0000DSAW
};

class Engine {
public:

    Engine();

    void run_engine(void);

    const KeyState& get_input(void);
    float get_delta_time(void);

private:
    // TODO: pack bool variables in a bitmap
    bool shouldKillGame;
    bool mouse_enabled;
    double last_frame;
    float delta_time;
    
    struct GLFWwindow* window;
    Options options;
    KeyState input_keys;
    Camera camera;      // TODO: Camera sometimes causes all sprites to disappear?
    glm::vec2 last_mcoords;

    FT_Library ft;
    uteng_render::TextManager text_manager;

    std::vector<uteng_render::Model> dynamic_renderables; // Can fall off screen, attached to world
    std::vector<uteng_render::Model> static_renderables; // Always on screen, attached to camera

    std::unordered_map<std::string, uteng_render::ShaderProgram> shader_pool;
    std::unordered_map<std::string, uteng_render::Texture>       texture_pool;

    void calc_delta_time(void);
    void process_input(void);

    void init_engine(void);
    void end_engine(void);
    void load_texture_sprite(const std::string& name, const std::string& path, GLint param);
    void load_texture_palette(const std::string& name, const std::string& path, GLint param);
    void load_shaders(void);
    void load_textures(void);
    void init_render(void); // TODO: return errors
    void run_render_loop(void);
    void prepare_renderables(void);
};

}

#endif