#include "renderobject.h"

namespace Framework {
    RenderObject::RenderObject(const std::string &modelPath, const glm::vec3 pos, const glm::vec3 scale, const glm::vec3 rot, const float angle) :
        m_model(modelPath), m_position(pos), m_scale(scale), m_rotation(rot), m_angle(angle)
    {
    }

    void RenderObject::Draw(const Shader &currentShader) {
        m_model.Draw(currentShader);
    }
}
