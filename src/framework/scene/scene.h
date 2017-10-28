#ifndef SCENE_H
#define SCENE_H

#include "../3d/camera.h"
#include "../3d/model.h"
#include "../3d/cube.h"
#include "../3d/quad.h"
#include "../3d/skybox.h"
#include "../3d/shadowmap.h"
#include "../3d/renderobject.h"

#include "../gpu/shader.h"
#include "../gpu/gbuffer.h"

#include "../light/pointlight.h"

#include "../postprocess/ssao.h"
#include "../postprocess/postprocess.h"

namespace Framework {
    class Scene {
    public:
        Scene(Renderer &renderer);

        void Init();
        void ReloadLightingPass();

        void GeometryPass(std::vector<RenderObject> &renderObjects, Camera &camera, Renderer &renderer);
        void ShadowmapPass(std::vector<RenderObject> &renderObjects);
        void LightingPass(int drawMode, GLfloat ambience, Camera &camera, Renderer &renderer);

        void RenderLights(Renderer &renderer);
        void RenderSkybox(Camera &camera);
        void PostProcessPass(bool useBloom, bool useMotionBlur, GLfloat exposure, GLfloat motionScale, Camera &camera, Renderer &renderer);

        GLfloat GetShadowNear() const;
        void SetShadowNear(GLfloat near);

        GLfloat GetShadowFar() const;
        void SetShadowFar(GLfloat far);

        glm::vec3 GetSunLightPos() const;
        void SetSunLightPos(const glm::vec3 &position);

        SSAO        m_ssao;
        PostProcess m_postProcess;
        Quad        m_fsQuad;
        ShadowMap   m_shadowMap;

    private:
        glm::mat4   m_projMatrix, m_viewMatrix, m_modelMatrix;
        glm::mat4   m_lightProjectionMatrix, m_lightViewMatrix;
        glm::mat4   m_lightSpaceMatrix;
        glm::mat4   m_modelViewMatrix;
        glm::mat4   m_prevModelViewMatrix;

        glm::vec3   m_sunLightPos;
        glm::vec3   m_sunLightColor;

        GLfloat     m_zNearShadow, m_zFarShadow;

        Shader      m_shaderLightingPass;
        Shader      m_shaderLightBox;
        Pointlight  m_pointlights;
        GBuffer     m_gbuffer;
        Skybox      m_skybox;
    };
}

#endif
