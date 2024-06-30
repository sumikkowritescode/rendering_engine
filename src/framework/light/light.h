#ifndef LIGHT_H
#define LIGHT_H

#include <glm/vec4.hpp>

#define GLEW_STATIC
#include <GL/glew.h>

namespace Framework {
    struct PointLight {
        glm::vec4 Position;
        glm::vec4 Color;
        GLfloat Linear;
        GLfloat Quadratic;
        GLfloat Radius;
        GLfloat padding;
    };

    struct DirectionalLight {

    };
}
#endif // !LIGHT_H