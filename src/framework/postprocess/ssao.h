#ifndef SSAO_H
#define SSAO_H

#include <random>

#include "../3d/quad.h"
#include "../gpu/shader.h"
#include "../gpu/gbuffer.h"

namespace Framework {
    class SSAO {
    public:
        SSAO();
        ~SSAO();

        void Init(const GLuint &screenWidth, const GLuint &screenHeight);
        void CreateTexture(GBuffer &gbuffer, Quad &fsQuad, const glm::mat4 &u_projMat, const GLuint &screenWidth, const GLuint &screenHeight);
        void BlurTexture(Quad &fsQuad, const GLuint &screenWidth, const GLuint &screenHeight);
        void SetTexture();
        void ReloadShaders();

        GLuint  GetFBO();
        GLuint  GetBlurFBO();

        GLint   GetKernelSize() const;
        const void    SetKernelSize(GLint size);

        GLfloat GetRadius() const;
        const void    SetRadius(GLfloat radius);

        GLfloat GetPower() const;
        const void    SetPower(GLfloat power);

    private:
        Shader  m_shaderSSAO;
        Shader  m_shaderBlur;

        GLuint  m_fbo, m_blur_fbo;
        GLuint  m_colorBuffer, m_colorBufferBlur;
        GLuint  m_noiseTexture;

        GLint   m_kernelSize;
        GLfloat m_radius;
        GLfloat m_power;

        GLfloat lerp(GLfloat a, GLfloat b, GLfloat f) const;

        std::vector<glm::vec3> m_ssaoKernel;
        std::vector<glm::vec3> m_ssaoNoise;
    };
}

#endif
