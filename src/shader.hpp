// 11/16/2023
// Spencer Banasik

// Header only class that manages our shader

#ifndef UT_SHADER_HPP
#define UT_SHADER_HPP

#include <string>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

// TODO: don't like this include in a header file, maybe we can consider other options
#include <util.hpp>

namespace uteng_render {
class Shader {
public:

    Shader(const std::string& vertex_path, const std::string& fragment_path) {

        auto vertex_source = uteng_util::read_file(vertex_path);
        auto fragment_source = uteng_util::read_file(fragment_path);
        const char* vertex_code = vertex_source.c_str();
        const char* fragment_code = fragment_source.c_str();
        
        unsigned int vertex_id, fragment_id;
        vertex_id = glCreateShader(GL_VERTEX_SHADER);
        fragment_id = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(vertex_id, 1, &vertex_code, NULL);
        glCompileShader(vertex_id);
        uteng_util::shader_comp_err(vertex_id);
        glShaderSource(fragment_id, 1, &fragment_code, NULL);
        glCompileShader(fragment_id);
        uteng_util::shader_comp_err(fragment_id);

        id = glCreateProgram();
        glAttachShader(id, vertex_id);
        glAttachShader(id, fragment_id);
        glLinkProgram(id);
        uteng_util::shader_link_err(id);

        glDeleteShader(vertex_id);
        glDeleteShader(fragment_id);

    }

    ~Shader() {
        glDeleteProgram(id);
    }

    void use() {
        glUseProgram(id);
    }

    unsigned int get_id() {
        return id;
    }

    void setBool(const std::string& name, bool value) const {
        glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
    }
    void setInt(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(id, name.c_str()), value);
    }
    void setFloat(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(id, name.c_str()), value);
    }
    void setVec3(const std::string& name, glm::vec3 value) const {
        glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(value));
    }
    void setVec3(const std::string& name, float val1, float val2, float val3) {
        glUniform3f(glGetUniformLocation(id, name.c_str()), val1, val2, val3);
    }
    void setMat3(const std::string& name, glm::mat3 value) const {
        glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }
    void setMat4(const std::string& name, glm::mat4 value) const {
        glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }

private:
    unsigned int id;
};
}
#endif