#include "shader.h"
#include <iostream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

namespace Framework {
    void Shader::Load(const GLchar* vert, const GLchar* frag, const GLchar* geom) {
        m_vertexPath = vert;
        m_fragmentPath = frag;
        m_geometryPath = geom;

        m_vShaderFile.exceptions(std::ifstream::failbit);
        m_fShaderFile.exceptions(std::ifstream::failbit);
        m_gShaderFile.exceptions(std::ifstream::failbit);
        try
        {
            m_vShaderFile.open(m_vertexPath);
            m_fShaderFile.open(m_fragmentPath);

            std::stringstream m_VShaderStream, m_FShaderStream;

            m_VShaderStream << m_vShaderFile.rdbuf();
            m_FShaderStream << m_fShaderFile.rdbuf();

            m_vShaderFile.close();
            m_fShaderFile.close();

            m_vertexCode = m_VShaderStream.str();
            m_fragmentCode = m_FShaderStream.str();

            if (m_geometryPath != nullptr)
            {
                std::stringstream m_GShaderStream;

                m_gShaderFile.open(m_geometryPath);

                m_GShaderStream << m_gShaderFile.rdbuf();

                m_gShaderFile.close();

                m_geometryCode = m_GShaderStream.str();
            }
        }
        catch (const std::ios_base::failure &e)
        {
            std::cerr << "Exception occured when reading a shader file\n"
                << e.what() << "\n"
                << "Please check that all of the shader files can be found."
                << std::endl;
        }

        m_vShaderCode = m_vertexCode.c_str();
        m_fShaderCode = m_fragmentCode.c_str();

        m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(m_vertexShader, 1, &m_vShaderCode, nullptr);
        glCompileShader(m_vertexShader);

        glGetShaderiv(m_vertexShader, GL_COMPILE_STATUS, &m_loadSuccess);
        if (!m_loadSuccess)
        {
            glGetShaderInfoLog(m_vertexShader, 512, nullptr, m_log);
            std::cout << m_vertexPath << " ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << m_log << std::endl;
        }

        m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(m_fragmentShader, 1, &m_fShaderCode, nullptr);
        glCompileShader(m_fragmentShader);

        glGetShaderiv(m_fragmentShader, GL_COMPILE_STATUS, &m_loadSuccess);
        if (!m_loadSuccess)
        {
            glGetShaderInfoLog(m_fragmentShader, 512, nullptr, m_log);
            std::cout << m_fragmentPath << " ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << m_log << std::endl;
        }

        if (m_geometryPath != nullptr)
        {
            m_gShaderCode = m_geometryCode.c_str();
            m_geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(m_geometryShader, 1, &m_gShaderCode, nullptr);
            glCompileShader(m_geometryShader);

            glGetShaderiv(m_geometryShader, GL_COMPILE_STATUS, &m_loadSuccess);
            if (!m_loadSuccess)
            {
                glGetShaderInfoLog(m_geometryShader, 512, nullptr, m_log);
                std::cout << m_geometryPath << " ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << m_log << std::endl;
            }
        }

        m_program = glCreateProgram();
        glAttachShader(m_program, m_vertexShader);
        glAttachShader(m_program, m_fragmentShader);
        if (m_geometryPath != nullptr)
            glAttachShader(m_program, m_geometryShader);
        glLinkProgram(m_program);

        glGetProgramiv(m_program, GL_LINK_STATUS, &m_loadSuccess);
        if (!m_loadSuccess)
        {
            glGetProgramInfoLog(m_program, 512, nullptr, m_log);
            std::cout << m_vertexPath << " " << m_fragmentPath << " ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << m_log << std::endl;
        }

        glDeleteShader(m_vertexShader);
        glDeleteShader(m_fragmentShader);
        if (m_geometryPath != nullptr)
            glDeleteShader(m_geometryShader);
    }

    void Shader::Use() {
        glUseProgram(m_program);
    }

    void Shader::Reload() {
        Load(m_vertexPath, m_fragmentPath, m_geometryPath);
    }

    void Shader::SetInt(const GLchar *name, int value) {
        glUniform1i(glGetUniformLocation(m_program, name), value);
    }

    void Shader::SetBool(const GLchar *name, bool value) {
        glUniform1i(glGetUniformLocation(m_program, name), value);
    }

    void Shader::SetFloat(const GLchar *name, float value) {
        glUniform1f(glGetUniformLocation(m_program, name), value);
    }

    void Shader::SetVector(const GLchar *name, const glm::vec2 &value) {
        glUniform2fv(glGetUniformLocation(m_program, name), 1, &value[0]);
    }

    void Shader::SetVector(const GLchar *name, const glm::vec3 &value) {
        glUniform3fv(glGetUniformLocation(m_program, name), 1, &value[0]);
    }

    void Shader::SetVector(const GLchar *name, const glm::vec4 &value) {
        glUniform4fv(glGetUniformLocation(m_program, name), 1, &value[0]);
    }

    void Shader::SetMatrix(const GLchar *name, const glm::mat2 &value) {
        glUniformMatrix2fv(glGetUniformLocation(m_program, name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::SetMatrix(const GLchar *name, const glm::mat3 &value) {
        glUniformMatrix3fv(glGetUniformLocation(m_program, name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::SetMatrix(const GLchar *name, const glm::mat4 &value) {
        glUniformMatrix4fv(glGetUniformLocation(m_program, name), 1, GL_FALSE, glm::value_ptr(value));
    }
}
