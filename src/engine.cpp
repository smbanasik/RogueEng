#include <engine.hpp>

#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <util.hpp>
#include <input.hpp>
#include <render.hpp>

namespace uteng {

// -- Window Functions --
void init_window(GLFWwindow* &window, const Options &options) {

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
    window = nullptr;
    options.screen_width = 800;
    options.screen_height = 600;
    options.fullscreen = false;
    options.window_name = "UntitledTextRPG";
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

    core_render::terminate_render();
    glfwTerminate();
}


void Engine::run_engine(void) {

    init_engine();

    while (shouldKillGame == false) {

        //process input
        uteng_input::process_input(window, &user_input);

        // TEMP
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        
        // render
        core_render::run_render_loop();

        glfwSwapBuffers(window);
        glfwPollEvents();
        // ENDTEMP

        shouldKillGame = glfwWindowShouldClose(window);
    }

    end_engine();
}
}