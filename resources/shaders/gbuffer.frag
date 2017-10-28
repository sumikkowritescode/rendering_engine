#version 430
layout (location = 0) out vec4 m_positionDepthTexture;
layout (location = 1) out vec3 m_normalTexture;
layout (location = 2) out vec4 m_albedoSpecTexture;
layout (location = 3) out vec2 m_velocityTexture;

in VERT_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 Normal;
    vec4 vPosition;
    vec4 vPrevPosition;
} frag_in;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

uniform float zNear;
uniform float zFar;

float LinearizeDepth(float m_depth)
{
    float z = m_depth * 2.0 - 1.0;
    return (2.0 * zNear * zFar) / (zFar + zNear - z * (zFar - zNear));
}

void main()
{
    m_positionDepthTexture.xyz = frag_in.FragPos;
    m_positionDepthTexture.a = LinearizeDepth(gl_FragCoord.z);

    m_normalTexture = normalize(frag_in.Normal);

    m_albedoSpecTexture.rgb = texture(texture_diffuse1, frag_in.TexCoords).rgb;
    m_albedoSpecTexture.a = texture(texture_specular1, frag_in.TexCoords).r;

    vec2 a = (frag_in.vPosition.xy / frag_in.vPosition.w) * 0.5f + 0.5f;
    vec2 b = (frag_in.vPrevPosition.xy / frag_in.vPrevPosition.w) * 0.5f + 0.5f;
    m_velocityTexture = a - b;
}
