#include "model.h"

#include <assimp/postprocess.h>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <fstream>
#include <sstream>
#include <iostream>

namespace Framework {
    Model::Model(const std::string &path, const bool gamma) :
        m_gammaCorrection(gamma) {
        LoadModel(path);
    }

    void Model::Draw(const Shader &m_shader) {
        for(GLuint i = 0; i < m_meshes.size(); i++)
            m_meshes[i].Draw(m_shader);
    }

    aiMesh *Model::GetMesh() const {
        return m_mesh;
    }

    void Model::LoadModel(const std::string &path) {
        const aiScene* scene = m_importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP:: " << m_importer.GetErrorString() << std::endl;
            return;
        }

        m_directory = path.substr(0, path.find_last_of('/'));

        ProcessNode(scene->mRootNode, scene);
    }

    void Model::ProcessNode(const aiNode* node, const aiScene* scene) {
        for(GLuint i = 0; i < node->mNumMeshes; i++)
        {
            m_mesh = scene->mMeshes[node->mMeshes[i]];
            m_meshes.push_back(ProcessMesh(m_mesh, scene));
        }

        for(GLuint i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene);
        }

    }

    Mesh Model::ProcessMesh(const aiMesh* mesh, const aiScene* scene) {
        std::vector<Mesh::Vertex> vertices;
        std::vector<GLuint> indices;
        std::vector<Mesh::Texture> textures;

        for(GLuint i = 0; i < mesh->mNumVertices; i++)
        {
            Mesh::Vertex vertex;
            glm::vec3 vector;

            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }

            if(mesh->HasTextureCoords(0))
            {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            if (mesh->HasTangentsAndBitangents())
            {
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;

                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }

            vertices.push_back(vertex);
        }

        if (mesh->HasFaces())
        {
            for(GLuint i = 0; i < mesh->mNumFaces; i++)
            {
                aiFace face = mesh->mFaces[i];

                for(GLuint j = 0; j < face.mNumIndices; j++)
                    indices.push_back(face.mIndices[j]);
            }
        }

        if(mesh->mMaterialIndex)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            std::vector<Mesh::Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            std::vector<Mesh::Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

            std::vector<Mesh::Texture> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

            std::vector<Mesh::Texture> heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        }

        return Mesh(vertices, indices, textures);
    }

    std::vector<Mesh::Texture> Model::LoadMaterialTextures(const aiMaterial* material, const aiTextureType type, const std::string &typeName) {
        std::vector<Mesh::Texture> textures;
        for(GLuint i = 0; i < material->GetTextureCount(type); i++)
        {
            aiString str;
            material->GetTexture(type, i, &str);

            GLboolean skip = false;

            for(GLuint j = 0; j < m_loadedTextures.size(); j++)
            {
                if(m_loadedTextures[j].path == str)
                {
                    textures.push_back(m_loadedTextures[j]);
                    skip = true;
                    break;
                }
            }

            if(!skip)
            {
                Mesh::Texture texture;
                texture.id = m_texture.Load(str.C_Str(), m_directory);
                texture.type = typeName;
                texture.path = str;
                textures.push_back(texture);
                m_loadedTextures.push_back(texture);
            }
        }
        return textures;
    }
}
