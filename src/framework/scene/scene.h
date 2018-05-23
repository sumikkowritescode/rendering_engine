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

#include "../postprocess/ssao.h"
#include "../postprocess/postprocess.h"

#include "lightcontroller.h"


namespace Framework {
    class Scene {
    public:
        Scene(Renderer &renderer);

        void Init();
        void ReloadLightingPass();

        void GeometryPass(std::vector<RenderObject>& renderObjects, Camera &camera, Renderer& renderer);
        void ShadowmapPass(std::vector<RenderObject>& renderObjects);
        void LightingPass(int drawMode, GLfloat ambience, Camera& camera, Renderer& renderer, const Time& time);

        void RenderLights(Renderer &renderer);
        void RenderSkybox(Camera &camera);
        void PostProcessPass(bool useBloom, bool useMotionBlur, GLfloat exposure, GLfloat motionScale, Camera &camera, Renderer &renderer);

        glm::vec3 GetSunLightPos() const;
        GLfloat GetShadowNearPlane() const;
        GLfloat GetShadowFarPlane() const;

        void SetSunLightPos(const glm::vec3 &position);
        void SetShadowNearPlane(GLfloat near);
        void SetShadowFarPlane(GLfloat far);

        SSAO        m_ssao;
        PostProcess m_postProcess;
        Quad        m_fsQuad;
        ShadowMap   m_shadowMap;

    private:
        glm::mat4   m_projectionMatrix, m_viewMatrix, m_modelMatrix;
        glm::mat4   m_lightProjectionMatrix, m_lightViewMatrix;
        glm::mat4   m_lightSpaceMatrix;
        glm::mat4   m_modelViewMatrix;

        glm::vec3   m_sunLightPos;
        glm::vec3   m_sunLightColor;

        GLfloat     m_shadowNearPlane, m_shadowFarPlane;

        Shader      m_shaderLightingPass;
        Shader      m_shaderLightBox;
        LightController  m_lightController;
        GBuffer     m_gbuffer;
        Skybox      m_skybox;
    };
}

#endif
