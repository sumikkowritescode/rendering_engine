#ifndef CUBE_H
#define CUBE_H

#define GLEW_STATIC
#include <GL/glew.h>

namespace Framework {
    class Cube {

    public:
        Cube();
        ~Cube();
        void Render();

    private:
        GLuint m_vao, m_vbo, m_ebo;
    };
}

#endif
