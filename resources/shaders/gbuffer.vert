#version 430
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out VERT_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 Normal;
    vec4 vPosition;
    vec4 vPrevPosition;
} vert_out;

uniform mat4 projMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 prevModelViewMatrix;

void main()
{
    vec4 viewPos = modelViewMatrix * vec4(position, 1.0f);
    mat3 normalMatrix = transpose(inverse(mat3(modelViewMatrix)));

    vert_out.FragPos = viewPos.xyz;
    vert_out.TexCoords = texCoords;
    vert_out.Normal = normalMatrix * normal;

    vert_out.vPosition = projMatrix * modelViewMatrix * vec4(position, 1.0f);
    vert_out.vPrevPosition = projMatrix * prevModelViewMatrix * vec4(position, 1.0f);

    gl_Position = vert_out.vPosition;

}
