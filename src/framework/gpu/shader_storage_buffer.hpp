#ifndef SHADER_STORAGE_BUFFER_H
#define SHADER_STORAGE_BUFFER_H

#define GLEW_STATIC
#include <vector>
#include <iostream>
#include <GL/glew.h>

namespace Framework {
    class ShaderStorageBuffer {
    public:
        template <typename S>
        void Create(const S& s, int size, int binding, const std::vector<S>& container) {
            glGenBuffers(1, &m_ssbo);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
            glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(S) * size, container.data(), GL_DYNAMIC_COPY);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, m_ssbo);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

            CheckForErrors();
        }

        template <typename S>
        void SendData(const S& s, int size, const std::vector<S>& container) {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
            glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(S) * size, container.data(), GL_DYNAMIC_COPY);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

            CheckForErrors();
        }

        template <typename S>
        void Create(const S& s, float size, int binding, const std::vector<S>& container) {
            glGenBuffers(1, &m_ssbo);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
            glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(S) * size, container.data(), GL_DYNAMIC_COPY);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, m_ssbo);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

            CheckForErrors();
        }

        template <typename S>
        void SendData(const S& s, float size, const std::vector<S>& container) {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
            glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(S) * size, container.data(), GL_DYNAMIC_COPY);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

            CheckForErrors();
        }

    private:
        GLuint m_ssbo;


        void CheckForErrors() {
            GLenum errorCode = glGetError();

            if (errorCode != GL_NO_ERROR)
                std::cout << errorCode << std::endl;
        }
    };
}

#endif // ! SHADER_STORAGE_BUFFER_H
