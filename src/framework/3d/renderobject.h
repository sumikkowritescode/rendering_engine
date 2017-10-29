#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

#include "../3d/model.h"
#include "../gpu/shader.h"

namespace Framework {
    class RenderObject {
    public:
        RenderObject(const std::string &modelPath, const glm::vec3 pos = glm::vec3(0.0f), const glm::vec3 scale = glm::vec3(0.0f), const glm::vec3 rot = glm::vec3(0.0f), const float angle = 0.0f);

        void Draw(Shader &currentShader);

        float m_angle;

        glm::vec3 m_position, m_scale, m_rotation;
        glm::mat4 m_modelViewMatrix, m_prevModelViewMatrix;
        Model m_model;
    };
}

#endif
