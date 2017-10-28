#include "mesh.h"

#include <fstream>
#include <sstream>
#include <iostream>

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>


namespace Framework {
    Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices, const std::vector<Texture> &textures)
    {
        m_vertices = vertices;
        m_indices = indices;
        m_textures = textures;
        SetupMesh();
    }

    void Mesh::Draw(const Shader &m_shader)
    {
        GLuint diffuseNr = 1;
        GLuint specularNr = 1;
        GLuint normalNr = 1;
        GLuint heightNr = 1;

        for(GLuint i = 0; i < m_textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            std::stringstream ss;
            std::string number;
            std::string name = m_textures[i].type;
            if(name == "texture_diffuse")
                ss << diffuseNr++;
            else if(name == "texture_specular")
                ss << specularNr++;
            else if(name == "texture_normal")
                ss << normalNr++;
            else if(name == "texture_height")
                ss << heightNr++;
            number = ss.str();

            glUniform1i(glGetUniformLocation(m_shader.m_program, (name + number).c_str()), i);

            glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
        }

        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        for (GLuint i = 0; i < m_textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    void Mesh::SetupMesh()
    {
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);
        glGenBuffers(1, &m_ebo);

        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), &m_indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Tangent));

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Bitangent));

        glBindVertexArray(0);
    }
}
