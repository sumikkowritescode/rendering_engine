#ifndef MODEL_H
#define MODEL_H

#define GLEW_STATIC
#include <GL/glew.h>

#include <string>
#include <vector>
#include <unordered_map>

#include "../3rdparty/tiny_obj_loader.h"
#include "../gpu/shader.h"
#include "mesh.h"
#include "texture.h"

namespace Framework {
    class Model {
    public:
        Model(const std::string &path, const bool gamma = false);
        void Draw(Shader &shader) const;

    private:
        bool LoadOBJFromFile(const std::string& filename);
        void LoadTexture(const std::string& textureName, const std::string& materialType, const std::string &baseDir);

        const bool m_gammaCorrection;
        std::vector<Mesh> m_meshes;
        std::vector<tinyobj::shape_t> m_shapes;
        std::vector<tinyobj::material_t> m_materials;
        tinyobj::attrib_t m_vertexAttributes;
        std::unordered_map<std::string, Mesh::Texture> m_textures;
    };
}
#endif
