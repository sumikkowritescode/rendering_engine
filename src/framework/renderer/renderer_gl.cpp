#include "renderer_gl.h"

namespace Framework {
    void Renderer::SetViewport() {
        glViewport(0, 0, m_width, m_height);
    }

    void Renderer::SetProjectionMatrix(Camera &m_camera) {
        u_projMatrix = glm::perspective(m_camera.GetFov(), (GLfloat)m_width / (GLfloat)m_height, m_camera.GetNearPlane(), m_camera.GetFarPlane());
    }

    glm::mat4 Renderer::GetProjectionMatrix() const {
        return u_projMatrix;
    }

    // Move these to a screen manager or something later
    void Renderer::SetResolution(GLuint width, GLuint height) {
        m_width = width;
        m_height = height;
    }
    int Renderer::GetScreenWidth() const {
        return m_width;
    }

    int Renderer::GetScreenHeight() const {
        return m_height;
    }
}
