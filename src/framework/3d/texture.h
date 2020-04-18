#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>
#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>

namespace Framework {
    class Texture {
    public:
        GLuint Load(const char* path);
        GLuint LoadCubeMap(std::vector<const GLchar*> &m_faces);
    };
}
#endif
