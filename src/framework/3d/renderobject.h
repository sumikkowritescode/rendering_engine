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
        RenderObject(const std::string& modelPath, const glm::vec3 position = glm::vec3(0.0f), const glm::vec3 scale = glm::vec3(0.0f), const glm::vec3 rotation = glm::vec3(0.0f), const float angle = 0.0f);

        glm::vec3 GetPosition() const;
        glm::vec3 GetRotation() const;
        glm::vec3 GetScale() const;

        glm::mat4 GetModelViewMatrix() const;
        glm::mat4 GetPreviousModelViewMatrix() const;

        float GetAngle() const;

        void SetPosition(const glm::vec3& position);
        void SetRotation(const glm::vec3& rotation);
        void SetScale(const glm::vec3& scale);

        void SetModelViewMatrix(const glm::mat4& matrix);
        void SetPreviousModelViewMatrix(const glm::mat4& matrix);

        void SetAngle(float angle);

        void Draw(Shader& currentShader) const;

    private:
        glm::vec3 m_position, m_scale, m_rotation;
        glm::mat4 m_modelViewMatrix, m_prevModelViewMatrix;
        float m_angle;

        Model m_model;
    };
}

#endif
