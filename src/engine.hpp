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

#include <util.hpp>
#include <camera.hpp>
#include <shader.hpp>
#include <renderable.hpp>

struct GLFWwindow;
namespace uteng {

struct Options {
    int screen_width;
    int screen_height;
    bool fullscreen;
    std::string window_name;
};
struct KeyState {
    bool quit;
    uint8_t bmap_primary_keys; // 0000DSAW
};

class Engine {
public:

    Engine();

    void run_engine(void);

    const KeyState& get_input(void);
    float get_delta_time(void);

private:
    // TODO: back bool variables in a bitmap
    bool shouldKillGame;
    bool mouse_enabled;
    double last_frame;
    float delta_time;
    
    struct GLFWwindow* window;
    Options options;
    KeyState input_keys;
    Camera camera;
    glm::vec2 last_mcoords;

    std::vector<uteng_render::Model> dynamic_renderables; // Can fall off screen, attached to world
    std::vector<uteng_render::Model> static_renderables; // Always on screen, attached to camera

    std::unordered_map<std::string, uteng_render::ShaderProgram> shader_pool;
    std::unordered_map<std::string, uteng_render::Texture>       texture_pool;

    void calc_delta_time(void);
    void process_input(void);

    void init_engine(void);
    void end_engine(void);
    void init_render(void);
    void run_render_loop(void);
};

}

#endif