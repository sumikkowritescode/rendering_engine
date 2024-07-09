#ifndef POSTPROCESS_H
#define POSTPROCESS_H

#include "../3d/quad.h"
#include "../gpu/shader.h"
#include "../gpu/gbuffer.h"

namespace Framework {
    class PostProcess {
    public:
        PostProcess();
        ~PostProcess();

        void Init(const GLuint &screenWidth, const GLuint &screenHeight);
        void Blur(Quad &fsQuad, const GLuint &screenWidth, const GLuint &screenHeight);
        void Use(Quad &fsQuad, GBuffer &gbuffer, bool useBloom, bool useMotionBlur, GLfloat exposure, GLfloat motionScale);
        void ReloadShaders();

        GLuint GetFBO() const;

    private:
        GLuint m_fbo;
        GLuint m_rbo;
        GLuint m_pingpong_fbo[2];
        GLuint m_colorBuffers[2];
        GLuint m_pingpongColorbuffers[2];

        GLboolean m_horizontal, m_firstIteration;

        Shader m_shaderBlur;
        Shader m_shaderPostProcess;
    };
}
#endif
