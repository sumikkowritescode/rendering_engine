#ifndef RENDERER_H
#define RENDERER_H

#include "../3d/camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Framework {
    class Renderer {
    public:
        void SetViewport();
        void SetProjectionMatrix(const Camera& camera);
        void SetResolution(GLuint width, GLuint height);
        glm::mat4 GetProjectionMatrix() const;
        int GetScreenWidth() const;
        int GetScreenHeight() const;

    private:
        GLuint    m_width;
        GLuint    m_height;
        glm::mat4 m_projectionMatrix;
    };
}

#endif
