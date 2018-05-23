#include "scene.h"

namespace Framework {
    Scene::Scene(Renderer &renderer) :
        m_shadowMap(4096, 4096),
        m_sunLightPos(glm::vec3(0.0f, 500.0f, 72.581f)),
        m_sunLightColor(glm::vec3(1.0f, 1.0f, 1.0f)),
        m_zNearShadow(10.0f),
        m_zFarShadow(2000.0f),
        m_lightController(100),
        m_prevModelViewMatrix(glm::mat4())
    {
        m_shaderLightingPass.Load("../resources/shaders/deferred_lighting_pass.vert", "../resources/shaders/deferred_lighting_pass.frag");
        m_shaderLightBox.Load("../resources/shaders/deferred_light_box.vert", "../resources/shaders/deferred_light_box.frag");

        m_gbuffer.Init(renderer);
        m_ssao.Init(renderer);
        m_postProcess.Init(renderer);
        m_shadowMap.Init();
        m_skybox.Init("../resources/textures/right.jpg", "../resources/textures/left.jpg", "../resources/textures/top.jpg", "../resources/textures/bottom.jpg", "../resources/textures/back.jpg", "../resources/textures/front.jpg");
    }

    void Scene::ReloadLightingPass() {
        m_shaderLightingPass.Reload();
        m_shaderLightingPass.Use();

        m_shaderLightingPass.SetInt("positionDepthTex", 0);
        m_shaderLightingPass.SetInt("normalTex", 1);
        m_shaderLightingPass.SetInt("albedoSpecTex", 2);
        m_shaderLightingPass.SetInt("velocityTex", 3);
        m_shaderLightingPass.SetInt("ssaoTex", 4);
        m_shaderLightingPass.SetInt("shadowMapTex", 5);
    }

    void Scene::Init() {
        ReloadLightingPass();
        m_ssao.ReloadShaders();
        m_postProcess.ReloadShaders();
        m_shadowMap.ReloadShaders();
        m_skybox.ReloadShader();
    }

    void Scene::GeometryPass(std::vector<RenderObject> &renderObjects, Camera &camera, Renderer &renderer) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_gbuffer.GetFBO());
            renderer.SetViewport();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            m_gbuffer.GetShader().Use();
            m_gbuffer.GetShader().SetFloat("camera.GetNearPlane()", camera.GetNearPlane());
            m_gbuffer.GetShader().SetFloat("camera.GetFarPlane()", camera.GetFarPlane());

            renderer.SetProjectionMatrix(camera);
            m_projMatrix = renderer.GetProjectionMatrix();
            m_viewMatrix = camera.GetViewMatrix();
            m_gbuffer.GetShader().SetMatrix("projMatrix", m_projMatrix);

            // Render each object in the scene and pass their matrices to the m_gbuffer shader
            for (auto& object : renderObjects)
            {
                m_modelMatrix = glm::mat4();
                m_modelMatrix = glm::translate(m_modelMatrix, object.GetPosition());
                m_modelMatrix = glm::scale(m_modelMatrix, object.GetScale());
                m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(object.GetAngle()), object.GetRotation());
                object.SetModelViewMatrix(m_viewMatrix * m_modelMatrix);

                m_gbuffer.GetShader().SetMatrix("modelViewMatrix", object.GetModelViewMatrix());
                m_gbuffer.GetShader().SetMatrix("prevModelViewMatrix", object.GetPreviousModelViewMatrix());

                object.Draw(m_gbuffer.GetShader());
                object.SetPreviousModelViewMatrix(object.GetModelViewMatrix());
            }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Scene::ShadowmapPass(std::vector<RenderObject> &renderObjects) {
        m_shadowMap.Bind();
            m_lightProjectionMatrix = glm::ortho(-550.0f, 550.0f, -550.0f, 550.0f, m_zNearShadow, m_zFarShadow);
            m_lightViewMatrix = glm::lookAt(m_sunLightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            m_lightSpaceMatrix = m_lightProjectionMatrix * m_lightViewMatrix;

            glClear(GL_DEPTH_BUFFER_BIT);
            m_shadowMap.GetShader().Use();
            m_shadowMap.GetShader().SetMatrix("lightSpaceMatrix", m_lightSpaceMatrix);
            for (const auto& object : renderObjects)
            {
                m_modelMatrix = glm::mat4();
                m_modelMatrix = glm::translate(m_modelMatrix, object.GetPosition());
                m_modelMatrix = glm::scale(m_modelMatrix, object.GetScale());
                m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(object.GetAngle()), object.GetRotation());
                m_shadowMap.GetShader().SetMatrix("modelMatrix", m_modelMatrix);
                object.Draw(m_shadowMap.GetShader());
            }
        m_shadowMap.Unbind();
    }

    void Scene::LightingPass(int drawMode, GLfloat ambience, Camera &camera, Renderer &renderer, const Time &time) {
        m_ssao.CreateTexture(m_gbuffer, m_fsQuad, m_projMatrix, renderer);
        m_ssao.BlurTexture(m_fsQuad, renderer);

        glBindFramebuffer(GL_FRAMEBUFFER, m_postProcess.GetFBO());
            renderer.SetViewport();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            m_shaderLightingPass.Use();
            m_gbuffer.SetTextures();
            m_ssao.SetTexture();

            glm::vec3 m_sunLightPosView = glm::vec3(camera.GetViewMatrix() * glm::vec4(m_sunLightPos, 1.0));
            m_shaderLightingPass.SetVector("sunLight.Position", m_sunLightPosView);
            m_shaderLightingPass.SetVector("sunLight.Color", m_sunLightColor);
            m_shaderLightingPass.SetMatrix("lightSpaceMatrix", m_lightSpaceMatrix);
            m_shaderLightingPass.SetMatrix("inverseViewMatrix", glm::inverse(m_viewMatrix));
            m_shadowMap.SetTexture();
            m_lightController.SendBufferData(m_viewMatrix);
            m_shaderLightingPass.SetInt("drawMode", drawMode);
            m_shaderLightingPass.SetFloat("ambience", ambience);
            m_fsQuad.Render();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Scene::RenderLights(Renderer &renderer) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_gbuffer.GetFBO());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_postProcess.GetFBO());
        glBlitFramebuffer(0, 0, renderer.GetScreenWidth(), renderer.GetScreenHeight(), 0, 0, renderer.GetScreenWidth(), renderer.GetScreenHeight(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);
            renderer.SetViewport();
            m_lightController.RenderLightBox(m_projMatrix, m_viewMatrix, m_modelMatrix);
            m_fsQuad.Render();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Scene::RenderSkybox(Camera &camera) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_postProcess.GetFBO());
            m_viewMatrix = glm::mat4(glm::mat3(camera.GetViewMatrix()));
            m_skybox.Render(m_viewMatrix, m_projMatrix);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Scene::PostProcessPass(bool useBloom, bool useMotionBlur, GLfloat exposure, GLfloat motionScale, Camera &camera, Renderer &renderer) {
        m_projMatrix = glm::perspective(camera.GetFov(), (GLfloat)renderer.GetScreenWidth() / (GLfloat)renderer.GetScreenHeight(), camera.GetNearPlane(), camera.GetFarPlane());
        m_viewMatrix = camera.GetViewMatrix();
        m_postProcess.Blur(m_fsQuad, renderer);
        m_postProcess.Use(m_fsQuad, m_gbuffer, useBloom, useMotionBlur, exposure, motionScale);
    }

    GLfloat Scene::GetShadowNear() const {
        return m_zNearShadow;
    }

    void Scene::SetShadowNear(GLfloat near) {
        m_zNearShadow = near;
    }

    GLfloat Scene::GetShadowFar() const {
        return m_zFarShadow;
    }

    void Scene::SetShadowFar(GLfloat far) {
        m_zFarShadow = far;
    }

    glm::vec3 Scene::GetSunLightPos() const {
        return m_sunLightPos;
    }

    void Scene::SetSunLightPos(const glm::vec3 &position) {
        m_sunLightPos = position;
    }
}
