#include <input.hpp>

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glad/glad.h>

void uteng_input::process_input(GLFWwindow* window, UserInput* input) {

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        input->quit = true;

}