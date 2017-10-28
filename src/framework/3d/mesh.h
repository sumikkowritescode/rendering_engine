#ifndef MESH_H
#define MESH_H

#define GLEW_STATIC
#include <GL/glew.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <assimp/types.h>

#include <string>
#include <vector>

#include "../gpu/shader.h"

namespace Framework {
    class Mesh {
    public:
        struct Texture {
            GLuint id;
            std::string type;
            aiString path;
        };

        struct Vertex {
            glm::vec3 Position;
            glm::vec3 Normal;
            glm::vec2 TexCoords;
            glm::vec3 Tangent;
            glm::vec3 Bitangent;
        };

        std::vector<Vertex>  m_vertices;
        std::vector<GLuint>  m_indices;
        std::vector<Texture> m_textures;
        GLuint m_vao;

        Mesh(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices, const std::vector<Texture> &textures);

        void Draw(const Shader &m_shader);

    private:
        GLuint m_vbo, m_ebo;
        void SetupMesh();
    };
}
#endif
