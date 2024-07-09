#ifndef QUAD_H
#define QUAD_H

#define GLEW_STATIC
#include <GL/glew.h>

namespace Framework {
    class Quad {
    public:
        Quad();
        ~Quad();
        void Init();
        void Render();

    private:
        GLuint m_vao;
        GLuint m_vbo;
    };
}
#endif
