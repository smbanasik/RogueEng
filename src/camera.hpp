// 12/22/2023
// Spencer Banasik
// Camera class based on the opengl tutorial
#ifndef UT_CAMERA_HPP
#define UT_CAMERA_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include <util.hpp>

#include <iostream>

enum CameraMovement {
    UP,
    LEFT,
    DOWN,
    RIGHT,
};


const float SPEED = 2.5f;
const glm::vec2 ASPECT_RATIO = { 4.0f, 3.0f };

struct Camera {
public:
    glm::vec3 position;
    glm::vec3 up;
    glm::vec2 ortho_size;
    float movement_speed;
    
    Camera(const glm::vec3& position = glm::vec3(0.0f,0.0f,1.0f), const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f))
        : movement_speed(SPEED), ortho_size(ASPECT_RATIO) {
        this->position = position;
        this->up = up;
    }

    glm::mat4 get_view_matrix() {
        return glm::lookAt(position, glm::vec3(position.x, position.y, 0.0f), up);
    }

    void process_keyboard(uint8_t direction , float delta_time) {
        float velocity = movement_speed * delta_time;
        glm::vec2 direction_vec{ 0, 0 };
        if (uteng_util::get_bit<uint8_t>(direction, UP) == true)
            direction_vec.y += 1;
        if (uteng_util::get_bit<uint8_t>(direction, DOWN) == true)
            direction_vec.y -= 1;
        if (uteng_util::get_bit<uint8_t>(direction, LEFT) == true)
            direction_vec.x -= 1;
        if (uteng_util::get_bit<uint8_t>(direction, RIGHT) == true)
            direction_vec.x += 1;
        
        direction_vec = glm::normalize(direction_vec) * velocity;
        position.x += direction_vec.x;
        position.y += direction_vec.y;
    }

    void process_mouse_scroll(float yoffset) {
        ortho_size.x -= yoffset;
        ortho_size.y -= yoffset;

        if (ortho_size.x < 4)
            ortho_size.x = 4;
        if (ortho_size.y < 3)
            ortho_size.y = 3;
    }

private:
};


#endif