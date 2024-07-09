#include "postprocess.h"
#include <iostream>

namespace Framework {
    PostProcess::PostProcess()
    {
    }
    
    void PostProcess::Init(const GLuint &screenWidth, const GLuint &screenHeight) {
        m_shaderBlur.Load("../resources/shaders/deferred_lighting_pass.vert", "../resources/shaders/blur.frag");
        m_shaderPostProcess.Load("../resources/shaders/deferred_lighting_pass.vert", "../resources/shaders/postprocess.frag");

        glGenFramebuffers(1, &m_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

        glGenTextures(2, m_colorBuffers);
        for (GLuint i = 0; i < 2; i++)
        {
            glBindTexture(GL_TEXTURE_2D, m_colorBuffers[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_colorBuffers[i], 0);
        }

        glGenRenderbuffers(1, &m_rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

        GLuint hdrAttachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, hdrAttachments);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glGenFramebuffers(2, m_pingpong_fbo);
        glGenTextures(2, m_pingpongColorbuffers);
        for (GLuint i = 0; i < 2; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, m_pingpong_fbo[i]);
            glBindTexture(GL_TEXTURE_2D, m_pingpongColorbuffers[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pingpongColorbuffers[i], 0);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "Framebuffer not complete!" << std::endl;
        }
    }

    PostProcess::~PostProcess() {
        if (m_fbo != 0)
            glDeleteFramebuffers(1, &m_fbo);
        if (m_rbo != 0)
            glDeleteRenderbuffers(1, &m_rbo);

        glDeleteTextures(2, m_colorBuffers);
        glDeleteFramebuffers(2, m_pingpong_fbo);
        glDeleteTextures(2, m_pingpongColorbuffers);
    }

    GLuint PostProcess::GetFBO() const {
        return m_fbo;
    }

    void PostProcess::Blur(Quad &fsQuad, const GLuint &screenWidth, const GLuint &screenHeight) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_horizontal = true;
        m_firstIteration = true;
        GLuint amount = 10;
        m_shaderBlur.Use();
        for (GLuint i = 0; i < amount; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, m_pingpong_fbo[m_horizontal]);
            glViewport(0, 0, screenWidth, screenHeight);
            m_shaderBlur.SetInt("horizontal", m_horizontal);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_firstIteration ? m_colorBuffers[1] : m_pingpongColorbuffers[!m_horizontal]);
            fsQuad.Render();
            m_horizontal = !m_horizontal;

            if (m_firstIteration)
                m_firstIteration = false;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void PostProcess::Use(Quad &fsQuad, GBuffer &gbuffer, bool useBloom, bool useMotionBlur, GLfloat exposure, GLfloat motionScale) {
            m_shaderPostProcess.Use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_colorBuffers[0]);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, m_pingpongColorbuffers[!m_horizontal]);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, gbuffer.GetVelocity());
            m_shaderPostProcess.SetInt("useBloom", useBloom);
            m_shaderPostProcess.SetInt("useMotionBlur", useMotionBlur);
            m_shaderPostProcess.SetFloat("exposure", exposure);
            m_shaderPostProcess.SetFloat("motionScale", motionScale);
            fsQuad.Render();
    }

    void PostProcess::ReloadShaders() {
        m_shaderPostProcess.Reload();
        m_shaderBlur.Reload();

        m_shaderPostProcess.Use();
        m_shaderPostProcess.SetInt("sceneTex", 0);
        m_shaderPostProcess.SetInt("bloomBlurTex", 1);
        m_shaderPostProcess.SetInt("velocityTex", 2);

        m_shaderBlur.Use();
        m_shaderBlur.SetInt("imageTex", 0);
    }
}
