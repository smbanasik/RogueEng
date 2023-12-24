// 12/22/2023
// Spencer Banasik
// Camera class based on the opengl tutorial
#ifndef UT_CAMERA_HPP
#define UT_CAMERA_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// TODO: Redesign camera
enum CameraMovement {
    FORWARD,
    BACKWARDS,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    ROLL_LEFT,
    ROLL_RIGHT
};

// TODO: convert these into variables
const float YAW = -90.f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

struct Camera {
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 world_up;
    float yaw, pitch;
    float movement_speed, mouse_sensitivity, zoom;
    
    Camera(glm::vec3 position = glm::vec3(0.0f,0.0f,0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = YAW, float pitch = PITCH) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed(SPEED),
        mouse_sensitivity(SENSITIVITY), zoom(ZOOM) {
        this->position = position;
        this->world_up = up;
        this->yaw = yaw;
        this->pitch = pitch;
        update_camera_vectors();
    }
    // Optional todo: constructor with scalar values

    glm::mat4 get_view_matrix() {
        return glm::lookAt(position, position + front, up);
    }

    void process_keyboard(CameraMovement direction, float delta_time) {
        float velocity = movement_speed * delta_time;
        if (direction == FORWARD)
            position += front * velocity;
        if (direction == BACKWARDS)
            position -= front * velocity;
        if (direction == LEFT)
            position -= right * velocity;
        if (direction == RIGHT)
            position += right * velocity;
        if (direction == UP)
            position += up * velocity;
        if (direction == DOWN)
            position -= up * velocity;
        // TODO: roll right and left
    }

    void process_mouse_movement(float xoffset, float yoffset, GLboolean constrain_pitch = true) {
        xoffset *= mouse_sensitivity;
        yoffset *= mouse_sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (yaw > 360)
            yaw -= 360;
        if (yaw < 0)
            yaw += 360;

        if (constrain_pitch) {
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;
        }
        update_camera_vectors();
    }

    void process_mouse_scroll(float yoffset) {
        zoom -= static_cast<float>(yoffset);
        if (zoom < 1.0f)
            zoom = 1.0f;
        if (zoom > 45.0f)
            zoom = 45.0f;
    }

private:
    void update_camera_vectors() {
        // TODO: handle roll
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(front);
        right = glm::normalize(glm::cross(front, world_up)); // TODO: utilize deep crevice to remove us from world up
        up = glm::normalize(glm::cross(right, front));
    }

};


#endif