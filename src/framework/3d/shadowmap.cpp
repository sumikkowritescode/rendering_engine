#include "shadowmap.h"

namespace Framework {
    ShadowMap::ShadowMap(GLuint width, GLuint height)
    {
        m_width = width;
        m_height = height;

        m_shader.Load("../resources/shaders/shadowmap.vert", "../resources/shaders/shadowmap.frag");
        m_shaderDebug.Load("../resources/shaders/shadowmap_debug.vert", "../resources/shaders/shadowmap_debug.frag");
    }

    void ShadowMap::Init() {
        glGenFramebuffers(1, &m_fbo);

        glGenTextures(1, &m_texture);
        glBindTexture(GL_TEXTURE_2D, m_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        GLfloat m_borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, m_borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_texture, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    ShadowMap::~ShadowMap() {
        if (m_fbo != 0) {
            glDeleteFramebuffers(1, &m_fbo);
            glDeleteTextures(1, &m_texture);
        }
    }

    void ShadowMap::Bind() {
        glViewport(0, 0, m_width, m_height);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    }

    void ShadowMap::Unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void ShadowMap::SetTexture() {
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D, m_texture);
    }

    void ShadowMap::RenderDebug(Quad &fsQuad, GLfloat zNear, GLfloat zFar, Renderer &renderer) {
        renderer.SetViewport();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_shaderDebug.Use();
        glUniform1f(glGetUniformLocation(m_shaderDebug.m_program, "zNear"), zNear);
        glUniform1f(glGetUniformLocation(m_shaderDebug.m_program, "zFar"), zFar);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texture);
        fsQuad.Render();
    }

    void ShadowMap::ReloadShaders() {
        m_shader.Reload();
        m_shaderDebug.Reload();

        m_shaderDebug.Use();
        glUniform1i(glGetUniformLocation(m_shaderDebug.m_program, "m_depthTex"), 0);
    }

    Shader &ShadowMap::GetShader() {
        return m_shader;
    }
}
