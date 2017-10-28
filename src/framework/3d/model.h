#ifndef MODEL_H
#define MODEL_H

#define GLEW_STATIC
#include <GL/glew.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <string>
#include <vector>

#include "../gpu/shader.h"
#include "mesh.h"
#include "texture.h"

namespace Framework {
    class Model {
    public:
        Model(const std::string &path, const bool gamma = false);
        void Draw(const Shader &shader);
        aiMesh* GetMesh() const;

        std::vector<Mesh::Texture> m_loadedTextures;
        std::vector<Mesh> m_meshes;

    private:
        void LoadModel(const std::string &path);
        const bool m_gammaCorrection;

        void ProcessNode(const aiNode* node, const aiScene* scene);
        Mesh ProcessMesh(const aiMesh* mesh, const aiScene* scene);
        std::vector<Mesh::Texture> LoadMaterialTextures(const aiMaterial* mat, const aiTextureType type, const std::string &typeName);

        Assimp::Importer m_importer;
        aiMesh* m_mesh;

        Texture m_texture;

        std::string m_directory;
    };
}
#endif
