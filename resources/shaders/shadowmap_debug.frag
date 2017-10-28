#version 430
out vec4 color;
in vec2 TexCoords;

uniform sampler2D m_depthTex;

void main()
{
    float depthValue = texture(m_depthTex, TexCoords).r;
    color = vec4(vec3(depthValue), 1.0f);
}
