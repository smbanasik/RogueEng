#include <engine.hpp>

#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <core_render.hpp>
#include <util.hpp>

namespace uteng {

// -- Window Functions --
void init_window(GLFWwindow*& window, const Options& options) {

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(options.screen_width, options.screen_height, options.window_name.c_str(), NULL, NULL);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window \n";
        glfwTerminate();
        return;
    }
}

// -- Class Functions --

Engine::Engine() {
    shouldKillGame = false;
    delta_time = 0;
    last_frame = 0;
    window = nullptr;
    options.screen_width = 800;
    options.screen_height = 600;
    options.fullscreen = false;
    options.window_name = "UntitledTextRPG";
    input_keys.quit = false;
}

void Engine::init_engine(void) {

    if (!glfwInit()) {
        shouldKillGame = true;
        return;
    }

    init_window(window, options);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "GLAD failed to initialize.\n";
        shouldKillGame = true;
        return;
    }

    glViewport(0, 0, options.screen_width, options.screen_height);
    glfwSetFramebufferSizeCallback(window, uteng_util::framebuffer_resize_callback);

    core_render::init_render();

}

void Engine::end_engine(void) {

    glfwTerminate();
}


void Engine::run_engine(void) {

    init_engine();

    while (shouldKillGame == false) {

        process_input();

        // TEMP
        if (input_keys.quit == true)
            glfwSetWindowShouldClose(window, true);

        calc_delta_time();

        // render
        core_render::run_render_loop(input_keys);

        glfwSwapBuffers(window);
        glfwPollEvents();
        // ENDTEMP

        shouldKillGame = glfwWindowShouldClose(window);
    }

    end_engine();
}


const KeyState& Engine::get_input() {
    return input_keys;
}

void Engine::process_input(void) {

    input_keys.bmap_primary_keys.clear();
    
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        input_keys.quit = true;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        input_keys.bmap_primary_keys.set_bit(0);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        input_keys.bmap_primary_keys.set_bit(1);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        input_keys.bmap_primary_keys.set_bit(2);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        input_keys.bmap_primary_keys.set_bit(3);

}

double Engine::get_delta_time(void) {
    return delta_time;
}
void Engine::calc_delta_time(void) {
    double current_frame = glfwGetTime();
    delta_time = current_frame - last_frame;
    last_frame = current_frame;
}

}