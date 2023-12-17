// 11/15/2023
// Spencer Banasik

// - input.hpp & cpp: core input handling, provides a function hook for input listening
// Handles glfw input, and callbacks for input.
#ifndef UT_INPUT_HPP
#define UT_INPUT_HPP

#include <stdint.h>
#include <util.hpp>

struct GLFWwindow;
namespace uteng {
class Engine;
};
namespace uteng_input {

struct KeyState {
    bool quit;
    uteng_util::Bitmap<uint8_t> bmap_primary_keys; // 0000DSAW
    // TODO: when we implement the ability to change keys, we should have these correspond to bit positions,
    // so we don't have to use magic numbers in our code. EX: KEY_W rather than 0
};

class EngInput {
public:
    const KeyState& get_input(void);


private:
    friend uteng::Engine;
    void process_input(GLFWwindow*);

    KeyState input_state;

};
}
#endif