#include "lightcontroller.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../../utils/random_generator.h"

namespace Framework {
    LightController::LightController(GLuint lightCount) :
        m_lightCount(lightCount),
        m_constant(0.6f)
    {
        m_shaderLightingPass.Load("../resources/shaders/deferred_lighting_pass.vert", "../resources/shaders/deferred_lighting_pass.frag");
        m_shaderLightBox.Load("../resources/shaders/deferred_light_box.vert", "../resources/shaders/deferred_light_box.frag");

        m_lights.resize(m_lightCount);

        for (auto it = m_lights.begin(); it != m_lights.end(); ++it)
        {
            it->Position = GetRandomPosition();
            it->Color = GetRandomColor();
        }

        for (auto it = m_lights.begin(); it != m_lights.end(); ++it)
        {
            m_lightRandoms.push_back(glm::vec3(
                                               Utils::GetRandomFloat(0.0f, 200.0f) / 100.0f,
                                               Utils::GetRandomFloat(0.0f, 100.0f) / 100.0f,
                                               Utils::GetRandomFloat(0.0f, 300.0f) / 100.0f
                                            ));
        }

        m_shaderLightingPass.Use();
        m_ssbo.Create(PointLight(), m_lightCount, 0, m_lights);
    }

    void LightController::RandomizePosition(const Time &time) {
        auto lightRandomsIt = m_lightRandoms.begin();
        for (auto it = m_lights.begin(); it != m_lights.end(); ++it)
        {
            it->Position.x += sin(lightRandomsIt->x + time.deltaTime) * 0.1f;
            it->Position.y -= lightRandomsIt->y * 0.1f;
            if (it->Position.y <= -5.0f)
                it->Position.y = 300.0f;
            it->Position.z += sin(lightRandomsIt->z + time.deltaTime) * 0.05f;
            ++lightRandomsIt;
        }
    }

    void LightController::SendBufferData(const glm::mat4 &view) {
        for (auto it = m_lights.begin(); it != m_lights.end(); ++it)
        {
            it->Position = glm::vec4(view * it->Position);

            it->Linear = 1.2f;
            it->Quadratic = 1.8f;

            const GLfloat maxBrigthness = std::fmaxf(std::fmaxf(it->Color.r, it->Color.g), it->Color.b);
            const GLfloat radius = (-it->Linear + sqrtf(it->Linear * it->Linear - 4 * it->Quadratic * (m_constant - (GLfloat)(256.0 / 5.0) * maxBrigthness))) / ((GLfloat)2.0f * it->Quadratic);
            it->Radius = radius;
        }

        m_ssbo.SendData(PointLight(), m_lightCount, m_lights);
    }

    void LightController::RenderLightBox(const glm::mat4 &projection, const glm::mat4 &view, glm::mat4 &model) {
        m_shaderLightBox.Use();
        m_shaderLightBox.SetMatrix("projection", projection);
        m_shaderLightBox.SetMatrix("view", view);
        for (auto it = m_lights.begin(); it != m_lights.end(); ++it)
        {
            it->Position = glm::vec4(glm::inverse(view) * it->Position);
            model = glm::mat4();
            model = glm::translate(model, glm::vec3(it->Position.x, it->Position.y, it->Position.z));
            model = glm::scale(model, glm::vec3(0.25f));
            m_shaderLightBox.SetMatrix("model", model);
            m_shaderLightBox.SetVector("lightColor", it->Color);
            m_lightCube.Render();
        }
    }

    glm::vec4 LightController::GetRandomPosition() {
        return glm::vec4( 
                         Utils::GetRandomFloat(0.0f, 600.0f) - 300.0f,
                         Utils::GetRandomFloat(5.0f, 250.0f) + 20.0f,
                         Utils::GetRandomFloat(0.0f, 250.0f) - 125.0f,
                         1.0f
                        );
    }

    glm::vec4 LightController::GetRandomColor() {
        return glm::vec4( 
                        (Utils::GetRandomFloat(0.0f, 100.0f) / 100.0f) + 1.0f,
                        (Utils::GetRandomFloat(0.0f, 100.0f) / 100.0f) + 1.0f,
                        (Utils::GetRandomFloat(0.0f, 100.0f) / 100.0f) + 1.0f,
                         1.0f
                        );
    }
}
