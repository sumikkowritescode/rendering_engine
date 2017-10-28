#include "gbuffer.h"
#include <iostream>

namespace Framework {
    GBuffer::GBuffer()
    {
       m_shader.Load("../resources/shaders/gbuffer.vert", "../resources/shaders/gbuffer.frag");
    }

    void GBuffer::Init(Renderer &m_renderer) {
        glGenFramebuffers(1, &m_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

        glGenTextures(1, &m_positionDepthTexture);
        glGenTextures(1, &m_normalTexture);
        glGenTextures(1, &m_albedoSpecTexture);
        glGenTextures(1, &m_velocityTexture);

        glBindTexture(GL_TEXTURE_2D, m_positionDepthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_renderer.GetScreenWidth(), m_renderer.GetScreenHeight(), 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, m_normalTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_renderer.GetScreenWidth(), m_renderer.GetScreenHeight(), 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, m_albedoSpecTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_renderer.GetScreenWidth(), m_renderer.GetScreenHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, m_velocityTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, m_renderer.GetScreenWidth(), m_renderer.GetScreenHeight(), 0, GL_RG, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_positionDepthTexture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_normalTexture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_albedoSpecTexture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_velocityTexture, 0);

        GLuint m_attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
        glDrawBuffers(4, m_attachments);

        glGenRenderbuffers(1, &m_rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_renderer.GetScreenWidth(), m_renderer.GetScreenHeight());
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0); 
    }

    GBuffer::~GBuffer() {
        if (m_fbo != 0)
            glDeleteFramebuffers(1, &m_fbo);

        if (m_rbo != 0)
            glDeleteRenderbuffers(1, &m_rbo);

        glDeleteTextures(1, &m_positionDepthTexture);
        glDeleteTextures(1, &m_normalTexture);
        glDeleteTextures(1, &m_albedoSpecTexture);
        glDeleteTextures(1, &m_velocityTexture);
    }

    GLuint GBuffer::GetFBO() const {
        return m_fbo;
    }

    GLuint GBuffer::GetPosDepth() const {
        return m_positionDepthTexture;
    }

    GLuint GBuffer::GetNormal() const {
        return m_normalTexture;
    }

    GLuint GBuffer::GetAlbedoSpec() const {
        return m_albedoSpecTexture;
    }

    GLuint GBuffer::GetVelocity() const {
        return m_velocityTexture;
    }

    Shader &GBuffer::GetShader() {
        return m_shader;
    }

    void GBuffer::SetTextures() {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_positionDepthTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_normalTexture);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_albedoSpecTexture);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, m_velocityTexture);
    }

    void GBuffer::ReloadShader() {
        m_shader.Reload();
    }
}
