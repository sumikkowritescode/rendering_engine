#ifndef SHADER_H
#define SHADER_H

#define GLEW_STATIC
#include <GL/glew.h>

#include <fstream>
#include <string>
#include <vector>

#include <glm/glm.hpp>

namespace Framework {
    class Shader
    {
    public:
        void Load(const GLchar* vert, const GLchar* frag, const GLchar* geom = nullptr);
        void Use();
        void Reload();

        void SetInt(const GLchar *name, int value);
        void SetBool(const GLchar *name, bool value);
        void SetFloat(const GLchar *name, float value);

        void SetVector(const GLchar *name, const glm::vec2 &value);
        void SetVector(const GLchar *name, const glm::vec3 &value);
        void SetVector(const GLchar *name, const glm::vec4 &value);

        void SetMatrix(const GLchar *name, const glm::mat2 &value);
        void SetMatrix(const GLchar *name, const glm::mat3 &value);
        void SetMatrix(const GLchar *name, const glm::mat4 &value);

        GLuint m_program;

    private:
        void CheckForErrors();

        std::string   m_vertexCode, m_fragmentCode, m_geometryCode;
        std::ifstream m_vShaderFile, m_fShaderFile, m_gShaderFile;

        GLuint m_vertexShader, m_fragmentShader, m_geometryShader;
        GLint  m_loadSuccess;
        GLchar m_log[512];

        const GLchar* m_vertexPath;
        const GLchar* m_fragmentPath;
        const GLchar* m_geometryPath;

        const GLchar* m_vShaderCode;
        const GLchar* m_fShaderCode;
        const GLchar* m_gShaderCode;
    };
}

#endif // ! SHADER_H
