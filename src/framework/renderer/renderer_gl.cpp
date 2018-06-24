#include "renderer_gl.h"

namespace Framework {

    Renderer::Renderer() :
           m_projectionMatrix(1.0f)
    {}

    void Renderer::SetViewport() {
        glViewport(0, 0, m_width, m_height);
    }

    void Renderer::SetProjectionMatrix(const Camera& camera) {
        m_projectionMatrix = glm::perspective(camera.GetFov(), static_cast<GLfloat>(m_width) / static_cast<GLfloat>(m_height), camera.GetNearPlane(), camera.GetFarPlane());
    }

    glm::mat4 Renderer::GetProjectionMatrix() const {
        return m_projectionMatrix;
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
