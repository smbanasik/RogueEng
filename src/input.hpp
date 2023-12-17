// 11/15/2023
// Spencer Banasik

// - input.hpp & cpp: core input handling, provides a function hook for input listening
// Handles glfw input, and callbacks for input.
#ifndef UT_INPUT_HPP
#define UT_INPUT_HPP
struct GLFWwindow;
namespace uteng_input {

struct UserInput {
    bool quit;

};

extern void process_input(GLFWwindow*, UserInput*);

}
#endif