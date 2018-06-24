#include "renderobject.h"

namespace Framework {
    RenderObject::RenderObject(const std::string &modelPath, const glm::vec3 position, const glm::vec3 scale, const glm::vec3 rotation, const float angle) :
        m_model(modelPath), m_position(position), m_scale(scale), m_rotation(rotation), m_angle(angle), m_modelViewMatrix(1.0f), m_prevModelViewMatrix(1.0f)
    {
    }

    glm::vec3 RenderObject::GetPosition() const
    {
        return m_position;
    }

    glm::vec3 RenderObject::GetRotation() const
    {
        return m_rotation;
    }

    glm::vec3 RenderObject::GetScale() const
    {
        return m_scale;
    }

    glm::mat4 RenderObject::GetModelViewMatrix() const
    {
        return m_modelViewMatrix;
    }

    glm::mat4 RenderObject::GetPreviousModelViewMatrix() const // TODO: Make something else around this?
    {
        return m_prevModelViewMatrix;
    }

    float RenderObject::GetAngle() const
    {
        return m_angle;
    }

    void RenderObject::SetPosition(const glm::vec3& position)
    {
        m_position = position;
    }

    void RenderObject::SetRotation(const glm::vec3& rotation)
    {
        m_rotation = rotation;
    }

    void RenderObject::SetScale(const glm::vec3& scale)
    {
        m_scale = scale;
    }

    void RenderObject::SetModelViewMatrix(const glm::mat4& matrix)
    {
        m_modelViewMatrix = matrix;
    }

    void RenderObject::SetPreviousModelViewMatrix(const glm::mat4& matrix)
    {
        m_prevModelViewMatrix = matrix;
    }

    void RenderObject::SetAngle(float angle)
    {
        m_angle = angle;
    }

    void RenderObject::Draw(Shader& currentShader) const {
        m_model.Draw(currentShader);
    }
}
