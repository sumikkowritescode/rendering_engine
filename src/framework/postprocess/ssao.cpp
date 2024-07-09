#include "ssao.h"
#include "../../utils/random_generator.h"

#include <iostream>

namespace Framework {
    SSAO::SSAO() :
    m_kernelSize(64),
    m_radius(5.0f),
    m_power(2.0f)
    {
    }

    void SSAO::Init(const GLuint &screenWidth, const GLuint &screenHeight) {
        m_shaderSSAO.Load("../resources/shaders/ssao.vert", "../resources/shaders/ssao.frag");
        m_shaderBlur.Load("../resources/shaders/ssao.vert", "../resources/shaders/ssao_blur.frag");

        glGenFramebuffers(1, &m_fbo);
        glGenFramebuffers(1, &m_blur_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

        glGenTextures(1, &m_colorBuffer);
        glGenTextures(1, &m_colorBufferBlur);

        glBindTexture(GL_TEXTURE_2D, m_colorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorBuffer, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "SSAO Framebuffer not complete!" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, m_blur_fbo);
        glBindTexture(GL_TEXTURE_2D, m_colorBufferBlur);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorBufferBlur, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        for (GLuint i = 0; i < 64; i++)
        {
            glm::vec3 sample(Utils::GetRandomFloat(0.0f, 1.0f) * 2.0f - 1.0f,
                             Utils::GetRandomFloat(0.0f, 1.0f) * 2.0f - 1.0f,
                             Utils::GetRandomFloat(0.0f, 1.0f)
                            );
            sample = glm::normalize(sample);
            sample *= Utils::GetRandomFloat(0.0f, 1.0f);
            GLfloat scale = static_cast<GLfloat>(i) / 64.0f;

            scale = lerp(0.1f, 1.0f, scale * scale);
            sample *= scale;
            m_ssaoKernel.push_back(sample);
        }

        for (GLuint i = 0; i < 16; i++)
        {
            glm::vec3 noise(Utils::GetRandomFloat(0.0f, 1.0f) * 2.0f - 1.0f,
                            Utils::GetRandomFloat(0.0f, 1.0f) * 2.0f - 1.0f,
                            0.0f
                           );
            m_ssaoNoise.push_back(noise);
        }

        glGenTextures(1, &m_noiseTexture);
        glBindTexture(GL_TEXTURE_2D, m_noiseTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &m_ssaoNoise[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    
    SSAO::~SSAO() {
        if (m_fbo != 0)
            glDeleteFramebuffers(1, &m_fbo);
        if (m_blur_fbo != 0)
            glDeleteFramebuffers(1, &m_blur_fbo);

        glDeleteTextures(1, &m_colorBuffer);
        glDeleteTextures(1, &m_colorBufferBlur);
    }

    GLuint SSAO::GetFBO() {
        return m_fbo;
    }

    GLuint SSAO::GetBlurFBO() {
        return m_blur_fbo;
    }

    void SSAO::SetTexture() {
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, m_colorBufferBlur);
    }

    void SSAO::CreateTexture(GBuffer &gbuffer, Quad& fsQuad, const glm::mat4 &projMatrix, const GLuint &screenWidth, const GLuint &screenHeight) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
            glViewport(0, 0, screenWidth, screenHeight);
            glClear(GL_COLOR_BUFFER_BIT);
            m_shaderSSAO.Use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, gbuffer.GetPosDepth());
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, gbuffer.GetNormal());
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, m_noiseTexture);

            for (GLuint i = 0; i < 64; i++)
                m_shaderSSAO.SetVector( ("samples[" + std::to_string(i) + "]").c_str(), m_ssaoKernel[i]);

            m_shaderSSAO.SetMatrix("projMatrix", projMatrix);
            m_shaderSSAO.SetInt("m_kernelSize", m_kernelSize);
            m_shaderSSAO.SetFloat("m_radius", m_radius);
            m_shaderSSAO.SetFloat("m_power", m_power);
            fsQuad.Render();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void SSAO::BlurTexture(Quad &fsQuad, const GLuint &screenWidth, const GLuint &screenHeight) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_blur_fbo);
            glViewport(0, 0, screenWidth, screenHeight);
            glClear(GL_COLOR_BUFFER_BIT);
            m_shaderBlur.Use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_colorBuffer);
            fsQuad.Render();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void SSAO::ReloadShaders() {
        m_shaderSSAO.Reload();
        m_shaderBlur.Reload();

        m_shaderSSAO.Use();
        m_shaderSSAO.SetInt("m_positionTex", 0);
        m_shaderSSAO.SetInt("m_normalTex", 1);
        m_shaderSSAO.SetInt("m_noiseTex", 2);
    }

    GLint SSAO::GetKernelSize() const {
        return m_kernelSize;
    }

    const void SSAO::SetKernelSize(GLint size) {
        m_kernelSize = size;
    }

    GLfloat SSAO::GetRadius() const {
        return m_radius;
    }

    const void SSAO::SetRadius(GLfloat radius) {
        m_radius = radius;
    }

    GLfloat SSAO::GetPower() const {
        return m_power;
    }

    const void SSAO::SetPower(GLfloat power) {
        m_power = power;
    }

    GLfloat SSAO::lerp(GLfloat a, GLfloat b, GLfloat f) const {
        return a + f * (b - a);
    }
}
