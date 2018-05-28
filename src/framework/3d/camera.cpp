#include "camera.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace Framework {
    Camera::Camera(const glm::vec3 &position, const glm::vec3 &up, GLfloat yaw, GLfloat pitch) :
        m_position(position),
        m_worldUp(up),
        m_front(glm::vec3(0.0f, 0.0f, -1.0f)),
        m_yaw(yaw),
        m_pitch(pitch),
        m_fov(45.0f)

    {
    }

    Camera::Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) :
        m_position(glm::vec3(posX, posY, posZ)),
        m_worldUp(glm::vec3(upX, upY, upZ)),
        m_front(glm::vec3(0.0f, 0.0f, -1.0f)),
        m_yaw(yaw),
        m_pitch(pitch),
        m_fov(45.0f)
    {
    }

    void Camera::ProcessKeyboard(const Camera_Movement &direction, GLfloat m_movementSpeed) {
        if (direction == FORWARD)
            m_position += m_front * m_movementSpeed;
        if (direction == BACKWARD)
            m_position -= m_front * m_movementSpeed;
        if (direction == LEFT)
            m_position -= m_right * m_movementSpeed;
        if (direction == RIGHT)
            m_position += m_right * m_movementSpeed;
    }

    void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLfloat sensitivity, GLboolean constrainPitch) {
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        m_yaw   += xoffset;
        m_pitch += yoffset;

        if (constrainPitch)
        {
            if (m_pitch > 89.0f)
                m_pitch = 89.0f;
            if (m_pitch < -89.0f)
                m_pitch = -89.0f;
        }

        UpdateCameraVectors();
    }

    void Camera::ProcessMouseScroll(GLfloat yoffset) {
        if (m_fov >= 1.0f && m_fov <= 45.0f)
            m_fov -= yoffset;

        if (m_fov <= 1.0f)
            m_fov = 1.0f;

        if (m_fov >= 45.0f)
            m_fov = 45.0f;
    }

    void Camera::SetPlanes(GLfloat zNear, GLfloat zFar) {
        m_zNear = zNear;
        m_zFar =  zFar;
    }

    glm::mat4 Camera::GetViewMatrix() const {
        return glm::lookAt(m_position, m_position + m_front, m_up);
    }

    glm::vec3 Camera::GetPosition() const {
        return m_position;
    }

    GLfloat Camera::GetFov() const {
        return m_fov;
    }

    GLfloat Camera::GetNearPlane() const {
        return m_zNear;
    }

    GLfloat Camera::GetFarPlane() const {
        return m_zFar;
    }

    void Camera::UpdateCameraVectors() {
        glm::vec3 front;

        front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        front.y = sin(glm::radians(m_pitch));
        front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

        m_front = glm::normalize(front);
        m_right = glm::normalize(glm::cross(m_front, m_worldUp));
        m_up    = glm::normalize(glm::cross(m_right, m_front));
    }
}
