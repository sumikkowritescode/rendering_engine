#version 430
out float FragColor;
in vec2 TexCoords;

uniform sampler2D m_positionTex;
uniform sampler2D m_normalTex;
uniform sampler2D m_noiseTex;

uniform vec3 samples[64];

uniform int m_kernelSize;
uniform float m_radius;
uniform float m_power;

const vec2 noiseScale = vec2(1024.0/4.0f, 768.0f/4.0f);

uniform mat4 projMatrix;

void main()
{
    vec3 fragPos = texture(m_positionTex, TexCoords).xyz;
    vec3 normal = texture(m_normalTex, TexCoords).rgb;
    vec3 randomVec = texture(m_noiseTex, TexCoords * noiseScale).xyz;

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0f;
    for(int i = 0; i < m_kernelSize; i++)
    {
        vec3 samplePosition = TBN * samples[i]; // From tangent to view-space
        samplePosition = fragPos + samplePosition * m_radius;

        vec4 offset = vec4(samplePosition, 1.0f);
        offset = projMatrix * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5f + 0.5f; // transform to range 0.0 - 1.0

        // The position vector is already in view-space and thus already linearized
        float sampleDepth = texture(m_positionTex, offset.xy).z;

        float rangeCheck = smoothstep(0.0f, 1.0f, m_radius / abs(fragPos.z - sampleDepth));

        occlusion += (sampleDepth >= samplePosition.z ? 1.0f : 0.0f) * rangeCheck;
    }
    occlusion = 1.0f - (occlusion / m_kernelSize);

    FragColor = pow(occlusion, m_power);
}
