#ifndef SKYBOX_H
#define SKYBOX_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "../gpu/shader.h"
#include "../3d/texture.h"

namespace Framework {
    class Skybox {
    public:
         Skybox();
        ~Skybox();
        void Init(const GLchar* right, const GLchar* left, const GLchar* top, const GLchar* bottom, const GLchar* back, const GLchar* front);
        void Render(const glm::mat4 &view, const glm::mat4 &projection);
        void ReloadShader();

    private:
        GLuint m_vao;
        GLuint m_vbo;
        GLuint m_cubemapTexture;

        std::vector<const GLchar*> m_faces;

        Texture m_texture;
        Shader m_shaderSkybox;
    };
}

#endif
