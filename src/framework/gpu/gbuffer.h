#ifndef GBUFFER_H
#define GBUFFER_H

#include "../renderer/renderer_gl.h"
#include "shader.h"

namespace Framework {
    class GBuffer {
    public:
        GBuffer();
        ~GBuffer();
        void Init(Renderer &m_renderer);

        GLuint GetFBO() const;
        GLuint GetPosDepth() const;
        GLuint GetNormal() const;
        GLuint GetAlbedoSpec() const;
        GLuint GetVelocity() const;

        Shader &GetShader();

        void SetTextures();
        void ReloadShader();

    private:
        GLuint m_fbo;
        GLuint m_rbo;
        GLuint m_positionDepthTexture, m_normalTexture, m_albedoSpecTexture, m_velocityTexture;

        Shader m_shader;
    };
}

#endif
