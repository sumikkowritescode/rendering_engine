#ifndef CAMERA_H
#define CAMERA_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace Framework {
    class Camera {
    public:
        enum CameraMovement {
            FORWARD,
            BACKWARD,
            LEFT,
            RIGHT
        };

        Camera(const glm::vec3& position, const glm::vec3& up, GLfloat yaw, GLfloat pitch);
        Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch);

        void ProcessKeyboard(const CameraMovement& direction, GLfloat m_movementSpeed);
        void ProcessMouseMovement(GLfloat xOffset, GLfloat yOffset, GLfloat sensitivity, GLboolean constrainPitch = true);
        void ProcessMouseScroll(GLfloat yOffset);
        void SetPlanes(GLfloat zNear, GLfloat zFar);

        glm::mat4 GetViewMatrix() const;
        glm::vec3 GetPosition() const;

        GLfloat GetFov() const;
        GLfloat GetNearPlane() const;
        GLfloat GetFarPlane() const;

    private:
        glm::vec3 m_position;
        glm::vec3 m_worldUp;
        glm::vec3 m_front;
        glm::vec3 m_up;
        glm::vec3 m_right;

        GLfloat m_yaw;
        GLfloat m_pitch;
        GLfloat m_fov;

        GLfloat m_nearPlane, m_farPlane;

        void UpdateCameraVectors();
    };
}
#endif
