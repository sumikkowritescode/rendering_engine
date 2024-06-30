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

        for (auto& light : m_lights)
        {
            light.Position = GetRandomPosition();
            light.Color = GetRandomColor();
            light.Linear = 0.0f;
            light.Quadratic = 0.0f;
            light.Radius = 0.0f;
        }

        m_shaderLightingPass.Use();
        m_ssbo.Create(PointLight(), 0, m_lights);

        CheckForErrors();
    }

    void LightController::SendBufferData(const glm::mat4& view) {
        for (auto& light : m_lights)
        {
            light.Position = glm::vec4(view * light.Position);

            light.Linear = 1.2f;
            light.Quadratic = 1.8f;

            const GLfloat maxBrightness = std::fmaxf(std::fmaxf(light.Color.r, light.Color.g), light.Color.b);
            const GLfloat radius = (-light.Linear + sqrtf(light.Linear * light.Linear - 4 * light.Quadratic * (m_constant - static_cast<GLfloat>(256.0f / 5.0f) * maxBrightness))) / (static_cast<GLfloat>(2.0f) * light.Quadratic);
            light.Radius = radius;
        }

        m_ssbo.SendData(PointLight(), m_lights);

        CheckForErrors();
    }

    void LightController::RenderLightBox(const glm::mat4& projection, const glm::mat4& view, glm::mat4& model) {
       m_shaderLightBox.Use();
       m_shaderLightBox.SetMatrix("projection", projection);
       m_shaderLightBox.SetMatrix("view", view);
       for (auto& light : m_lights)
       {
           light.Position = glm::vec4(glm::inverse(view) * light.Position);
           model = glm::mat4(1.0f);
           model = glm::translate(model, glm::vec3(light.Position.x, light.Position.y, light.Position.z));
           model = glm::scale(model, glm::vec3(5.0f));

           m_shaderLightBox.SetMatrix("model", model);
           m_shaderLightBox.SetVector("lightColor", light.Color);
           m_lightCube.Render();
       }

        CheckForErrors();
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

    void LightController::CheckForErrors() {
        GLenum errorCode = glGetError();

        if (errorCode != GL_NO_ERROR)
        {
            std::cout << "GL Error Code: " << errorCode << std::endl;
        }
    }
}
