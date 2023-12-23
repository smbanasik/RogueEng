#include <util.hpp>

#include <fstream>
#include <iostream>

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#define DEBUG_UTIL

std::string uteng_util::read_file(const std::string& file_path) {

    std::ifstream input_file(file_path);

    if (!input_file.is_open()) {
        std::cout << "ERR: FILE PATH NOT FOUND." << "\n";
        return "";
    }

    std::string buffer = "", line = "";
    
    while (std::getline(input_file, line))
        buffer += line + "\n";

    #ifdef DEBUG_UTIL
    std::cout << "Reading File:\n" << buffer << "\n";
    #endif
    
    input_file.close();
    return buffer;
}

void uteng_util::shader_comp_err(const unsigned int shader_id) {
    int success;
    char info_log[512];
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader_id, 512, NULL, info_log);
        std::cout << "ERROR, SHADER HAS FAILED TO COMPILE. INFO: \n\t"
            << info_log << "\n";
    }
}

void uteng_util::shader_link_err(const unsigned int shader_prog_id) {
    int success;
    char info_log[512];
    glGetProgramiv(shader_prog_id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader_prog_id, 512, NULL, info_log);
        std::cout << "ERROR, SHADER PROGRAM FAILED:\n" << info_log << "\n";
    }
}