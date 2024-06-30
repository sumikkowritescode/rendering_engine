#define TINYOBJLOADER_IMPLEMENTATION
#include "model.h"
#include "../3rdparty/stb_image.h"

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <fstream>
#include <sstream>
#include <iostream>

namespace Framework
{
    Model::Model(const std::string &path, const bool gamma) :
        m_gammaCorrection(gamma)
    {
        LoadOBJFromFile(path);
    }

    void Model::Draw(Shader &shader) const
    {
        for (const auto &mesh : m_meshes)
        {
            mesh.Draw(shader);
        }
    }

    static std::string GetBaseDir(const std::string &filepath)
    {
        std::string result;
        if (filepath.find_last_of("/\\") != std::string::npos)
        {
            result = filepath.substr(0, filepath.find_last_of("/\\"));
        }
        else
        {
            result = ".";
        }

#ifdef _WIN32
        result = "\\";
#else
        result += "/";
#endif

        return result;
    }

    static bool FileExists(const std::string &absFileName)
    {
        bool result;
        FILE *filePtr = fopen(absFileName.c_str(), "rb");

        if (filePtr)
        {
            result = true;
            fclose(filePtr);
        }
        else
        {
            result = false;
        }

        return result;
    }

    bool Model::LoadOBJFromFile(const std::string &filename)
    {
        std::string err;
        std::string warn;

        std::vector<glm::vec3> vertices;

        std::string baseDir = GetBaseDir(filename);
        
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        tinyobj::attrib_t vertexAttributes;

        auto result = tinyobj::LoadObj(&vertexAttributes, &shapes, &materials, &warn, &err, filename.c_str(), baseDir.c_str());

        if (!err.empty())
        {
            std::cerr << err << std::endl;
        }

        if (result)
        {
            for (size_t i = 0; i < vertexAttributes.vertices.size(); i += 3)
            {
                vertices.emplace_back(glm::vec3(
                        vertexAttributes.vertices[i],
                        vertexAttributes.vertices[i + 1],
                        vertexAttributes.vertices[i + 2]));
            }

            for (size_t m = 0; m < materials.size(); m++)
            {
                tinyobj::material_t *materialPtr = &materials[m];

                if (materialPtr->diffuse_texname.length() > 0)
                {
                    LoadTexture(materialPtr->diffuse_texname, DIFFUSE_TEXTURE_TYPE_NAME, baseDir);
                }

                if (materialPtr->specular_texname.length() > 0)
                {
                    LoadTexture(materialPtr->specular_texname, SPECULAR_TEXTURE_TYPE_NAME, baseDir);
                }

                if (materialPtr->normal_texname.length() > 0)
                {
                    LoadTexture(materialPtr->normal_texname, NORMAL_TEXTURE_TYPE_NAME, baseDir);
                }
            }

            for (const auto &shape : shapes)
            {
                std::vector<Mesh::Vertex> vertices;
                std::vector<GLuint> indices;
                std::vector<int> materialIDs;

                size_t indexOffset = 0;
                for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
                {
                    const size_t faceVertices = shape.mesh.num_face_vertices[f];
                    Mesh::Vertex meshVertex;

                    for (size_t v = 0; v < faceVertices; v++)
                    {
                        const auto index = indexOffset + v;
                        const tinyobj::index_t &tinyObjVertex = shape.mesh.indices[index];

                        meshVertex.Position = glm::vec3(
                                vertexAttributes.vertices[3 * tinyObjVertex.vertex_index + 0],
                                vertexAttributes.vertices[3 * tinyObjVertex.vertex_index + 1],
                                vertexAttributes.vertices[3 * tinyObjVertex.vertex_index + 2]);

                        if (vertexAttributes.normals.size() > 0)
                        {
                            meshVertex.Normal = glm::vec3(
                                    vertexAttributes.normals[3 * tinyObjVertex.normal_index + 0],
                                    vertexAttributes.normals[3 * tinyObjVertex.normal_index + 1],
                                    vertexAttributes.normals[3 * tinyObjVertex.normal_index + 2]);
                        }

                        if (vertexAttributes.texcoords.size() > 0)
                        {
                            meshVertex.TexCoords = glm::vec2(
                                    vertexAttributes.texcoords[2 * tinyObjVertex.texcoord_index + 0],
                                    vertexAttributes.texcoords[2 * tinyObjVertex.texcoord_index + 1]);
                        }
                        else
                        {
                            meshVertex.TexCoords = glm::vec2(0.0f, 0.0f);
                        }

                        auto materialID = shape.mesh.material_ids[f];
                        if ((materialID < 0) ||
                            (materialID >= static_cast<int>(materials.size())))
                        {
                            materialID = materials.size() - 1;
                        }

                        indices.emplace_back(index);
                        vertices.emplace_back(meshVertex);
                        materialIDs.emplace_back(materialID);
                    }

                    indexOffset += faceVertices;
                }

                std::unordered_map<std::string, Mesh::Texture> addedTextures;
                for (const auto& materialID : materialIDs)
                {
                    auto textureName = materials[materialID].diffuse_texname;

                    auto it = m_textures.find(textureName);
                    if (it != m_textures.end())
                    {
                        auto texture = std::make_pair(it->first, it->second);
                        addedTextures.insert(texture);
                    }

                    textureName = materials[materialID].normal_texname;
                    it = m_textures.find(textureName);
                    if (it != m_textures.end())
                    {
                        auto texture = std::make_pair(it->first, it->second);
                        addedTextures.insert(texture);
                    }

                    textureName = materials[materialID].specular_texname;
                    it = m_textures.find(textureName);
                    if (it != m_textures.end())
                    {
                        auto texture = std::make_pair(it->first, it->second);
                        addedTextures.insert(texture);
                    }
                }

                std::vector<Mesh::Texture> textures;
                for (const auto& texture : addedTextures)
                {
                    textures.emplace_back(texture.second);
                }

                m_meshes.emplace_back(Mesh(vertices, indices, textures));
            }
        }
        else
        {
            std::cerr << "Failed to load " << filename << std::endl;
        }

        return result;
    }

    void Model::LoadTexture(const std::string &textureName, const std::string &materialType, const std::string &baseDir)
    {
        if (m_textures.find(textureName) == m_textures.end())
        {
            auto textureFileName = textureName;
            auto fileExists = FileExists(textureFileName);

            if (!fileExists)
            {
                textureFileName = baseDir + textureName;
                fileExists = FileExists(textureFileName);

                if (!fileExists)
                {
                    std::cerr << "Unable to find file: " << textureName
                              << std::endl;
                }
            }

            if (fileExists)
            {
                auto texture = Texture();
                auto textureID = texture.Load(textureFileName.c_str());
                auto meshTexture = Mesh::Texture {
                        textureID,
                        materialType,
                        textureFileName
                };

                m_textures.insert(std::make_pair(textureName, meshTexture));
            }
        }
    }
}
