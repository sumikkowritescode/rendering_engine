#ifndef SHADOWMAP_H
#define SHADOWMAP_H

#include "../3d/quad.h"
#include "../gpu/shader.h"
#include "../renderer/renderer_gl.h"

namespace Framework {
    class ShadowMap {
    public:
        ShadowMap(GLuint width, GLuint height);
        ~ShadowMap();

        void Init();

        void Bind();
        void Unbind();

        void SetTexture();
        void RenderDebug(Quad &fsQuad, GLfloat zNear, GLfloat zFar, Renderer &renderer);
        void ReloadShaders();

        Shader &GetShader();

    private:
        GLuint m_fbo, m_texture;
        GLuint m_width, m_height;

        Shader m_shaderDebug;
        Shader m_shader;
    };
}

#endif
