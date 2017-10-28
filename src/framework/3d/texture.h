#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>
#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>

namespace Framework {
    class Texture {
    public:
        GLuint Load(const char* path, const std::string &directory);
        GLuint Load(std::vector<const GLchar*> &m_faces);

    };
}
#endif
