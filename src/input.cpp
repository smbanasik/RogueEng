#include <input.hpp>

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glad/glad.h>

const uteng_input::KeyState& uteng_input::EngInput::get_input() {
    return input_state;
}

void uteng_input::EngInput::process_input(GLFWwindow* window) {

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        input_state.quit = true;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        input_state.bmap_primary_keys.set_bit(0);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        input_state.bmap_primary_keys.set_bit(1);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        input_state.bmap_primary_keys.set_bit(2);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        input_state.bmap_primary_keys.set_bit(3);

}