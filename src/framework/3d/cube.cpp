#include "cube.h"
#include <iostream>

namespace Framework {
    Cube::Cube() {
    }

    Cube::~Cube() {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_ebo);

        CheckForErrors();
    }

    void Cube::Init() {
        static const GLfloat m_vertices[] = {
                -1.0f, -1.0f, -1.0f, 1.0f,
                -1.0f, -1.0f,  1.0f, 1.0f,
                -1.0f,  1.0f, -1.0f, 1.0f,
                -1.0f,  1.0f,  1.0f, 1.0f,
                1.0f, -1.0f, -1.0f, 1.0f,
                1.0f, -1.0f,  1.0f, 1.0f,
                1.0f,  1.0f, -1.0f, 1.0f,
                1.0f,  1.0f,  1.0f, 1.0f

        };

        static const GLushort m_elements [] = {
                0, 1, 2, 3, 6, 7, 4, 5,
                0xFFFF, // Using a value that's not used in the code usually
                2, 6, 0, 4, 1, 5, 3, 7
        };

        glGenBuffers(1, &m_ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_elements), m_elements, GL_STATIC_DRAW);

        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

        glEnableVertexAttribArray(0);

        CheckForErrors();
    }

    void Cube::Render() {
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glEnable(GL_PRIMITIVE_RESTART);
        glPrimitiveRestartIndex(0xFFFF);
        glDrawElements(GL_TRIANGLE_STRIP, 17, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);

        CheckForErrors();
    }

    void Cube::CheckForErrors() {
        GLenum errorCode = glGetError();

        if (errorCode != GL_NO_ERROR)
        {
            std::cout << "GL Error Code: " << errorCode << std::endl;
        }
    }
}
