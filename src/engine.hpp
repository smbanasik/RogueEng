// 11/15/2023
// Spencer Banasik

// - engine.hpp & cpp: core engine, runs all functions, initializes things, creates the window
// The core engine of the game, this runs all functions such as render, input, and game logic.
// The engine handles window creation and options
#ifndef UT_ENGINE_HPP
#define UT_ENGINE_HPP

#include <string>

#include <util.hpp>

struct GLFWwindow;
namespace uteng {

// https://en.wikipedia.org/wiki/Opaque_pointer#C++
struct Options {
    int screen_width;
    int screen_height;
    bool fullscreen;
    std::string window_name;
};
struct KeyState {
    bool quit;
    uteng_util::Bitmap<uint8_t> bmap_primary_keys; // 0000DSAW
};

class Engine {
public:

    Engine();

    void run_engine(void);

    const KeyState& get_input(void);
    double get_delta_time(void);

private:
    bool shouldKillGame;
    double delta_time, last_frame;
    struct GLFWwindow* window;
    Options options;
    KeyState input_keys;

    void calc_delta_time(void);
    void process_input(void);
    // We could put these in the constructor & destructor but I don't want to.
    void init_engine(void);
    void end_engine(void);
};

}

#endif