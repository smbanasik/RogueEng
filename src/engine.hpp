// 11/15/2023
// Spencer Banasik

// - engine.hpp & cpp: core engine, runs all functions, initializes things, creates the window
// The core engine of the game, this runs all functions such as render, input, and game logic.
// The engine handles window creation and options
#ifndef UT_ENGINE_HPP
#define UT_ENGINE_HPP

#include <string>

#include <input.hpp>

struct GLFWwindow;
namespace uteng {

// https://en.wikipedia.org/wiki/Opaque_pointer#C++
struct Options {
    int screen_width;
    int screen_height;
    bool fullscreen;
    std::string window_name;
};

class Engine {
public:

    Engine();

    void run_engine(void);

    //KeyState get_input(void);

private:
    bool shouldKillGame;
    struct GLFWwindow* window;
    Options options;
    uteng_input::EngInput input_manager;

    void process_input(void);
    // We could put these in the constructor & destructor but I don't want to.
    void init_engine(void);
    void end_engine(void);
};

}

#endif