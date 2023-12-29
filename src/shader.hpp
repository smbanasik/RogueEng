// 11/16/2023
// Spencer Banasik

// Manages shaders, Shader is for individual programs, ShaderProgram is what is used for most things

#ifndef UT_SHADER_HPP
#define UT_SHADER_HPP

#include <string>
#include <stdexcept>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <util.hpp>

#define DEBUG_SHADER

namespace uteng_render {
class Shader {
public:

    Shader(const std::string& shader_path) {
        auto shader_source = uteng_util::read_file(shader_path);
        const char* shader_code = shader_source.c_str();

        unsigned int shader_type = 0;
        if (shader_path.find(".vert") != std::string::npos)
            shader_type = GL_VERTEX_SHADER;
        else if (shader_path.find(".frag") != std::string::npos)
            shader_type = GL_FRAGMENT_SHADER;
#ifdef DEBUG_SHADER
        if (shader_type == 0)
            throw std::out_of_range("Shader type invalid");
#endif
        shader_id = glCreateShader(shader_type);
        glShaderSource(shader_id, 1, &shader_code, NULL);
        glCompileShader(shader_id);
#ifdef DEBUG_SHADER
        uteng_util::shader_comp_err(shader_id);
#endif

    }

    ~Shader() {
        glDeleteShader(shader_id);
    }

    unsigned int get_id() {
        return shader_id;
    }

private:
    Shader(const Shader& other) = delete; // No copy-constructing shaders
    Shader& operator=(const Shader& other) = delete; // No copy-assigning shaders

    unsigned int shader_id;
};

class ShaderProgram {
public:

    ShaderProgram(const unsigned int vertex_id, const unsigned int fragment_id) {
        id = glCreateProgram();
        glAttachShader(id, vertex_id);
        glAttachShader(id, fragment_id);
        glLinkProgram(id);
#ifdef DEBUG_SHADER
        uteng_util::shader_link_err(id);
#endif DEBUG_SHADER

        ref_counter = new unsigned int;
        *ref_counter = 1;

    }
    ~ShaderProgram() {
        if (ref_counter == nullptr)
            return;

        (*ref_counter)--;
        if (*ref_counter == 0) {
            delete ref_counter;
            glDeleteProgram(id);
        }
    }
    ShaderProgram(const ShaderProgram& other) {
        this->ref_counter = other.ref_counter;
        (*ref_counter)++;
        this->id = other.id;
    }
    ShaderProgram& operator=(const ShaderProgram& other) {

        (*this->ref_counter)--;
        if (*this->ref_counter == 0) {
            delete ref_counter;
            glDeleteProgram(this->id);
        }

        this->ref_counter = other.ref_counter;
        (*ref_counter)++;
        this->id = other.id;

        return *this;
    }
    ShaderProgram(ShaderProgram&& other) noexcept {
        this->ref_counter = other.ref_counter;
        this->id = other.id;

        other.ref_counter = nullptr;
        other.id = 0;
    }
    ShaderProgram& operator=(ShaderProgram&& other) noexcept {
        (*this->ref_counter)--;
        if (*this->ref_counter == 0) {
            delete ref_counter;
            glDeleteProgram(this->id);
        }

        this->ref_counter = other.ref_counter;
        this->id = other.id;

        other.ref_counter = nullptr;
        other.id = 0;

        return *this;
    }

    void use() {
        glUseProgram(id);
    }

    unsigned int get_id() {
        return id;
    }

    void set_bool(const std::string& name, bool value) const {
        glUniform1i(get_uniform(name), (int)value);
    }
    void set_int(const std::string& name, int value) const {
        glUniform1i(get_uniform(name), value);
    }
    void set_float(const std::string& name, float value) const {
        glUniform1f(get_uniform(name), value);
    }
    void set_vec3(const std::string& name, glm::vec3 value) const {
        glUniform3fv(get_uniform(name), 1, glm::value_ptr(value));
    }
    void set_vec3(const std::string& name, float val1, float val2, float val3) {
        glUniform3f(get_uniform(name), val1, val2, val3);
    }
    void set_mat3(const std::string& name, glm::mat3 value) const {
        glUniformMatrix3fv(get_uniform(name), 1, GL_FALSE, glm::value_ptr(value));
    }
    void set_mat4(const std::string& name, glm::mat4 value) const {
        glUniformMatrix4fv(get_uniform(name), 1, GL_FALSE, glm::value_ptr(value));
    }

private:
    unsigned int id;
    unsigned int* ref_counter;

    GLint get_uniform(const std::string& name) const {
        GLint location = glGetUniformLocation(id, name.c_str());
#ifdef DEBUG_SHADER
        if (location == -1)
            throw std::out_of_range("Invalid ShaderProgram Uniform");
#endif
        return location;
    }
};

}
#undef DEBUG_SHADER

#endif