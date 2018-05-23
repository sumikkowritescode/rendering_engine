#version 430
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D sceneTex;
uniform sampler2D bloomBlurTex;
uniform sampler2D velocityTex;

uniform bool useBloom;
uniform bool useMotionBlur;
uniform float exposure;
uniform float motionScale;

const int m_maxSamples = 128;

vec3 MotionBlur() {
    vec2 velocity = texture(velocityTex, TexCoords).rg;
    velocity *= motionScale;

    vec2 texelSize = 1.0f / vec2(textureSize(sceneTex, 0));
    float speed = length(velocity / texelSize);
    int m_numSamples = clamp(int(speed), 1, m_maxSamples);
    vec3 result = vec3(0.0f);

    result = texture(sceneTex, TexCoords).rgb;
    for (int i = 1; i < m_numSamples; ++i)
    {
        vec2 offset = velocity * (float(i) / float(m_numSamples - 1) - 0.5f);
        result += texture(sceneTex, TexCoords + offset).rgb;
    }

    result /= float(m_numSamples);

    return result;
}

// ACES tone mapping curve
// https://github.com/ampas/aces-dev
// https://docs.unrealengine.com/latest/INT/Support/Builds/ReleaseNotes/2015/4_8/
vec3 ACESFilm(vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    vec3 result = clamp(((x * (a * x + b)) / (x * (c * x + d) + e)), 0.0f, 1.0f);
    return result;
}

void main()
{
    const float gamma = 2.2f;
    //vec3 hdrColor = vec3(0.0f);

    vec3 hdrColor = texture(sceneTex, TexCoords).rgb;
    vec3 bloomColor = texture(bloomBlurTex, TexCoords).rgb;
    vec3 result = vec3(0.0f);

/*
    if(useMotionBlur)
        hdrColor = MotionBlur();
    else
        hdrColor = texture(sceneTex, TexCoords).rgb;
*/

    if(useBloom)
        hdrColor += bloomColor;

    // tone mapping
    result += vec3(1.0f) - exp(-hdrColor * exposure);
    result += ACESFilm(hdrColor);

    // gamma correction
    result = pow(result, vec3(1.0f / gamma));

    FragColor = vec4(result, 1.0f);
}
