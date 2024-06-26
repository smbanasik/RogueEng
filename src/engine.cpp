#include <engine.hpp>

#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <util.hpp>

//#define DEBUG_MOUSE

namespace uteng {

// Since we have some callback functions that *cannot* be a member function,
// we're just going to do this. Could we do some static functions? no.
// Am I going to make the member variables public? no.
// You get a static (this file only) anonymous structure that contains these pointers.
static struct {
    Options* options = nullptr; // For screen resizing
    Camera* camera = nullptr; // For mouse callback
    glm::vec2* last_mcoords = nullptr; // For mouse callback
} uteng_vars;


// -- Window Functions --
bool init_window(GLFWwindow*& window, const Options& options) {

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(options.screen_width, options.screen_height, options.window_name.c_str(), NULL, NULL);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window \n";
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    return true;
}

// TODO: figure out what we're doing here and how we can adjust this
// I'm not too fond of it rn, refer to the deep crevice
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static bool first_mouse = true;
    if (first_mouse) {
        uteng_vars.last_mcoords->x = static_cast<float>(xpos);
        uteng_vars.last_mcoords->y = static_cast<float>(ypos);
        first_mouse = false;
    }
    float xoffset = static_cast<float>(xpos) - uteng_vars.last_mcoords->x;
    float yoffset = uteng_vars.last_mcoords->y - static_cast<float>(ypos); // Y coordinates go bottom to top so negate it.
#ifdef DEBUG_MOUSE
    // TODO: the pos has no upper bound so we'll need to find a way to wrap it around
    std::cout << "Xpos: " << xpos << " | Ypos: " << ypos << "\nxlast: "
        << uteng_vars.last_mcoords->x << " | ylast: " << uteng_vars.last_mcoords->y
        << "\nxoff: " << xoffset << " | yoff: " << yoffset << "\n";
#endif
    uteng_vars.last_mcoords->x = static_cast<float>(xpos);
    uteng_vars.last_mcoords->y = static_cast<float>(ypos);

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    uteng_vars.camera->process_mouse_scroll(static_cast<float>(yoffset));
}

void framebuffer_resize_callback(GLFWwindow* window, int n_width, int n_height) {
    uteng_vars.options->screen_width = n_width;
    uteng_vars.options->screen_height = n_height;
    glViewport(0, 0, n_width, n_height);
}

// -- Class Functions --

Engine::Engine() : camera(glm::vec3(0.0f, 0.0f, 1.0f)) {
    shouldKillGame = false;
    delta_time = 0;
    last_frame = 0;
    window = nullptr;
    options.screen_width = 800;
    options.screen_height = 600;
    options.fullscreen = false;
    options.window_name = "UntitledTextRPG";
    input_keys.quit = false;
    last_mcoords.x = options.screen_width / 2.0f;
    last_mcoords.y = options.screen_height / 2.0f;
    mouse_enabled = true;
    ft = nullptr;

    // The variables
    uteng_vars.camera = &camera;
    uteng_vars.options = &options;
    uteng_vars.last_mcoords = &last_mcoords;
}

void Engine::init_engine(void) {

    if (!glfwInit()) {
        goto eng_init_error;
    }

    if (!init_window(window, options)) {
        goto eng_init_error;
    }

    if (FT_Init_FreeType(&ft)) {
        goto eng_init_error;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "GLAD failed to initialize.\n";
        goto eng_init_error;
    }

    glViewport(0, 0, options.screen_width, options.screen_height);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    init_render();
    return;

eng_init_error:
    shouldKillGame = true;
    return;
}

void Engine::end_engine(void) {

    glfwTerminate();
    FT_Done_FreeType(ft);
}

void Engine::run_engine(void) {

    // TODO: move init to constructor.
    init_engine();

    while (shouldKillGame == false) {

        calc_delta_time();

        process_input();

        if (input_keys.quit == true)
            glfwSetWindowShouldClose(window, true);

        // decide what gets rendered and what needs to be discarded

        // render
        run_render_loop();

        glfwSwapBuffers(window);

        glfwPollEvents();

        shouldKillGame = glfwWindowShouldClose(window);
    }

    // TODO: move to destructor
    end_engine();
}


const KeyState& Engine::get_input() {
    return input_keys;
}

void Engine::process_input(void) {

    input_keys.bmap_primary_keys = 0;
    
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        input_keys.quit = true;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        input_keys.bmap_primary_keys = uteng_util::set_bit<uint8_t>(input_keys.bmap_primary_keys, 0);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        input_keys.bmap_primary_keys = uteng_util::set_bit<uint8_t>(input_keys.bmap_primary_keys, 1);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        input_keys.bmap_primary_keys = uteng_util::set_bit<uint8_t>(input_keys.bmap_primary_keys, 2);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        input_keys.bmap_primary_keys = uteng_util::set_bit<uint8_t>(input_keys.bmap_primary_keys, 3);

    //std::cout << "bmap: " << std::to_string(input_keys.bmap_primary_keys) << "\n";
    if (input_keys.bmap_primary_keys)
        camera.process_keyboard(input_keys.bmap_primary_keys, delta_time);

}

float Engine::get_delta_time(void) {
    return delta_time;
}
void Engine::calc_delta_time(void) {
    double current_frame = glfwGetTime();
    delta_time = static_cast<float>(current_frame - last_frame);
    last_frame = current_frame;
}

}