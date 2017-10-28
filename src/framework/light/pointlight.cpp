#include "pointlight.h"

namespace Framework {
    Pointlight::Pointlight() :
        constant(0.6f),
        linear(1.2f),
        quadratic(1.8f)
    {
        m_shaderLightingPass.Load("../resources/shaders/deferred_lighting_pass.vert", "../resources/shaders/deferred_lighting_pass.frag");
        m_shaderLightBox.Load("../resources/shaders/deferred_light_box.vert", "../resources/shaders/deferred_light_box.frag");
    }

    void Pointlight::Init(const GLuint NR_LIGHTS) {
        m_lightCount = NR_LIGHTS;

        // TODO: Refactor to use a random generator and own functions
        srand((unsigned int)time(nullptr));
        for (GLuint i = 0; i != NR_LIGHTS; ++i)
        {
            Light tempLight;
            tempLight.Position = GetRandomPosition();
            tempLight.Color = GetRandomColor();
            m_lights.push_back(tempLight);
        }

        // TODO: Split to own function
        for (auto it = m_lights.begin(); it != m_lights.end(); ++it)
        {
            m_lightRandoms.push_back(glm::vec3( (rand() % 200) / 100.0f,
                                              (rand() % 100) / 100.0f,
                                              (rand() % 300) / 100.0f
                                            ));
        }

        m_shaderLightingPass.Use();
        m_ssbo.Create(Light(), m_lightCount, 0, m_lights);
    }

    // TODO: Make simpler, use a random generator
    void Pointlight::RandomizePosition() {
        auto lightRandomsIt = m_lightRandoms.begin();
        for (auto it = m_lights.begin(); it != m_lights.end(); ++it)
        {
            it->Position.x += sin(lightRandomsIt->x + SDL_GetTicks() * 0.001f) * 0.1f;
            it->Position.y -= lightRandomsIt->y * 0.1f;
            if (it->Position.y <= -5.0f)
                it->Position.y = 300.0f;
            it->Position.z += sin(lightRandomsIt->z + SDL_GetTicks() * 0.001f) * 0.05f;
            ++lightRandomsIt;
        }
    }

    void Pointlight::Render(const glm::mat4 &view) {
        for (auto it = m_lights.begin(); it != m_lights.end(); ++it)
        {
            it->Position = glm::vec4(view * it->Position);

            it->Linear = linear;
            it->Quadratic = quadratic;

            const GLfloat maxBrigthness = std::fmaxf(std::fmaxf(it->Color.r, it->Color.g), it->Color.b);
            const GLfloat radius = (-linear + sqrtf(linear * linear - 4 * quadratic * (constant - (GLfloat)(256.0 / 5.0) * maxBrigthness))) / ((GLfloat)2.0f * quadratic);
            it->Radius = radius;
        }

        m_ssbo.SendData(Light(), m_lightCount, m_lights);
    }

    void Pointlight::RenderLightBox(const glm::mat4 &projection, const glm::mat4 &view, glm::mat4 &model) {
        m_shaderLightBox.Use();
        glUniformMatrix4fv(glGetUniformLocation(m_shaderLightBox.m_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(m_shaderLightBox.m_program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        for (auto it = m_lights.begin(); it != m_lights.end(); ++it)
        {
            it->Position = glm::vec4(glm::inverse(view) * it->Position);
            model = glm::mat4();
            model = glm::translate(model, glm::vec3(it->Position.x, it->Position.y, it->Position.z));
            model = glm::scale(model, glm::vec3(0.25f));
            glUniformMatrix4fv(glGetUniformLocation(m_shaderLightBox.m_program, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(glGetUniformLocation(m_shaderLightBox.m_program, "lightColor"), 1, &it->Color[0]);
            m_lightCube.Render();
        }
    }

    // TODO: Refactor to use the random generator in utils instead
    glm::vec4 Pointlight::GetRandomPosition() {
        return glm::vec4( (rand() % 600) - 300.0f,
                          (rand() % 250 + 5) + 20.0f,
                          (rand() % 250) - 125.0f,
                           1.0f
                        );
    }

    glm::vec4 Pointlight::GetRandomColor() {
        return glm::vec4( ((rand() % 100) / 100.0) + 1.0f,
                          ((rand() % 100) / 100.0) + 1.0f,
                          ((rand() % 100) / 100.0) + 1.0f,
                           1.0f
                        );
    }
}
