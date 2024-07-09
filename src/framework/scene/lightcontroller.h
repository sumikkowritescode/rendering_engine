#ifndef LIGHTCONTROLLER_H
#define LIGHTCONTROLLER_H

#include <cmath>
#include <vector>

#include "../gpu/shader.h"
#include "../gpu/shader_storage_buffer.hpp"
#include "../3d/cube.h"
#include "../base/time.h"
#include "../light/light.h"

namespace Framework {
    class LightController {
    public:
        LightController(GLuint lightCount);

        void Init();
        void SendBufferData(const glm::mat4 &view);
        void RenderLightBox(const glm::mat4 &projection, const glm::mat4 &view, glm::mat4 &model);
        void CheckForErrors();

    private:
        glm::vec4 GetRandomPosition();
        glm::vec4 GetRandomColor();

        std::vector<PointLight> m_lights;

        int                     m_lightCount;
        const GLfloat           m_constant;

        ShaderStorageBuffer     m_ssbo;
        Shader                  m_shaderLightingPass;
        Shader                  m_shaderLightBox;
        Cube                    m_lightCube;

    };

}

#endif // LIGHTCONTROLLER_H
