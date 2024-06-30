#include "scene.h"

namespace Framework {
    Scene::Scene(Renderer &renderer) :
        m_shadowMap(4096, 4096),
        m_sunLightPos(glm::vec3(0.0f, 500.0f, 72.581f)),
        m_sunLightColor(glm::vec3(1.0f, 1.0f, 1.0f)),
        m_shadowNearPlane(10.0f),
        m_shadowFarPlane(2000.0f),
        m_lightController(100),
        m_projectionMatrix(1.0f),
        m_viewMatrix(1.0f),
        m_modelMatrix(1.0f),
        m_lightProjectionMatrix(1.0f),
        m_lightViewMatrix(1.0f),
        m_lightSpaceMatrix(1.0f),
        m_modelViewMatrix(1.0f)
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
        m_shaderLightBox.Reload();
    }

    void Scene::GeometryPass(std::vector<RenderObject>& renderObjects, Camera& camera, Renderer& renderer) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_gbuffer.GetFBO());
            renderer.SetViewport();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            m_gbuffer.GetShader().Use();
            m_gbuffer.GetShader().SetFloat("nearPlane", camera.GetNearPlane());
            m_gbuffer.GetShader().SetFloat("farPlane", camera.GetFarPlane());

            renderer.SetProjectionMatrix(camera);
            m_projectionMatrix = renderer.GetProjectionMatrix();
            m_viewMatrix = camera.GetViewMatrix();
            m_gbuffer.GetShader().SetMatrix("projectionMatrix", m_projectionMatrix);

            // Render each object in the scene and pass their matrices to the GBuffer shader
            for (auto& object : renderObjects)
            {
                m_modelMatrix = glm::mat4(1.0f);
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

        CheckForErrors();
    }

    void Scene::ShadowmapPass(std::vector<RenderObject>& renderObjects) {
        m_shadowMap.Bind();
            m_lightProjectionMatrix = glm::ortho(-550.0f, 550.0f, -550.0f, 550.0f, m_shadowNearPlane, m_shadowFarPlane);
            m_lightViewMatrix = glm::lookAt(m_sunLightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            m_lightSpaceMatrix = m_lightProjectionMatrix * m_lightViewMatrix;

            glClear(GL_DEPTH_BUFFER_BIT);
            m_shadowMap.GetShader().Use();
            m_shadowMap.GetShader().SetMatrix("lightSpaceMatrix", m_lightSpaceMatrix);
            for (const auto& object : renderObjects)
            {
                m_modelMatrix = glm::mat4(1.0f);
                m_modelMatrix = glm::translate(m_modelMatrix, object.GetPosition());
                m_modelMatrix = glm::scale(m_modelMatrix, object.GetScale());
                m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(object.GetAngle()), object.GetRotation());
                m_shadowMap.GetShader().SetMatrix("modelMatrix", m_modelMatrix);
                object.Draw(m_shadowMap.GetShader());
            }
        m_shadowMap.Unbind();

        CheckForErrors();
    }

    void Scene::LightingPass(int drawMode, GLfloat ambience, Camera& camera, Renderer& renderer, const Time& time) {
        m_ssao.CreateTexture(m_gbuffer, m_fsQuad, m_projectionMatrix, renderer);
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

        CheckForErrors();
    }

    void Scene::RenderLights(Renderer &renderer) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_gbuffer.GetFBO());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_postProcess.GetFBO());
        glBlitFramebuffer(0, 0, renderer.GetScreenWidth(), renderer.GetScreenHeight(), 0, 0, renderer.GetScreenWidth(), renderer.GetScreenHeight(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);
            renderer.SetViewport();
            m_lightController.RenderLightBox(m_projectionMatrix, m_viewMatrix, m_modelMatrix);
            m_fsQuad.Render();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        CheckForErrors();
    }

    void Scene::RenderSkybox(Camera &camera) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_postProcess.GetFBO());
            m_viewMatrix = glm::mat4(glm::mat3(camera.GetViewMatrix()));
            m_skybox.Render(m_viewMatrix, m_projectionMatrix);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        CheckForErrors();
    }

    void Scene::PostProcessPass(bool useBloom, bool useMotionBlur, GLfloat exposure, GLfloat motionScale, Camera& camera, Renderer& renderer) {
        m_projectionMatrix = glm::perspective(camera.GetFov(), static_cast<GLfloat>(renderer.GetScreenWidth()) / static_cast<GLfloat>(renderer.GetScreenHeight()), camera.GetNearPlane(), camera.GetFarPlane());
        m_viewMatrix = camera.GetViewMatrix();
        m_postProcess.Blur(m_fsQuad, renderer);
        m_postProcess.Use(m_fsQuad, m_gbuffer, useBloom, useMotionBlur, exposure, motionScale);
    }

    glm::vec3 Scene::GetSunLightPos() const {
        return m_sunLightPos;
    }

    GLfloat Scene::GetShadowNearPlane() const {
        return m_shadowNearPlane;
    }

    GLfloat Scene::GetShadowFarPlane() const {
        return m_shadowFarPlane;
    }

    void Scene::SetSunLightPos(const glm::vec3 &position) {
        m_sunLightPos = position;
    }

    void Scene::SetShadowFarPlane(GLfloat far) {
        m_shadowFarPlane = far;
    }

    void Scene::SetShadowNearPlane(GLfloat near) {
        m_shadowNearPlane = near;
    }

    void Scene::CheckForErrors() {
        GLenum errorCode = glGetError();

        if (errorCode != GL_NO_ERROR)
        {
            std::cout << "GL Error Code: " << errorCode << std::endl;
        }
    }
}
