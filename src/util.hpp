// 11/15/2023
// Spencer Banasik

// util.hpp & cpp provide miscellaneous functions such as error handling
#ifndef UT_UTIL_HPP
#define UT_UTIL_HPP

// TODO: I'd like to avoid including string here, but I don't think I can avoid it
#include <string>

struct GLFWwindow;
namespace uteng_util{
    extern void framebuffer_resize_callback(GLFWwindow*, int, int);
    extern std::string read_file(const std::string& file_path);
    extern void shader_comp_err(const unsigned int shader_id);
    extern void shader_link_err(const unsigned int shader_prog_id);
}

#endif