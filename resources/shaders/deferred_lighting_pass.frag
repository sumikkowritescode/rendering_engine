#version 430
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;

struct Light {
    vec3 Position;
    vec3 Color;
    float Linear;
    float Quadratic;
    float Radius;
};

const int NR_LIGHTS = 100;

layout (std430, binding = 0) buffer CircleSSBO {
    Light lights[];
};

struct DirLight {
    vec3 Position;
    vec3 Color;
};

uniform DirLight sunLight;

uniform sampler2D positionDepthTex;
uniform sampler2D normalTex;
uniform sampler2D albedoSpecTex;
uniform sampler2D velocityTex;
uniform sampler2D ssaoTex;
uniform sampler2D shadowMapTex;

uniform mat4 lightSpaceMatrix;
uniform mat4 inverseViewMatrix;
uniform int drawMode;
uniform float ambience;

vec3 FragPos = texture(positionDepthTex, TexCoords).rgb;
vec3 Normal = texture(normalTex, TexCoords).rgb;
vec3 Diffuse = texture(albedoSpecTex, TexCoords).rgb;
vec2 Velocity = texture(velocityTex, TexCoords).rg;

float Depth = texture(positionDepthTex, TexCoords).a;
float AmbientOcclusion = texture(ssaoTex, TexCoords).r;
float Specular = texture(albedoSpecTex, TexCoords).a;

float constant = 0.6f;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // Perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(shadowMapTex, projCoords.xy).r;

    float currentDepth = projCoords.z;

    // Calculate bias
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(sunLight.Position - FragPos);

    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMapTex, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMapTex, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    // Keep the shadow at 0.0 when outside the zFar region of the light's frustum
    if(projCoords.z > 1.0)
       shadow = 0.0;

    return shadow;
}

vec3 calcSunLight() {
    vec3 ambient = Diffuse * vec3(ambience * AmbientOcclusion);
    vec3 viewDir = normalize(-FragPos.xyz);

    vec3 lightDir = normalize(sunLight.Position -  FragPos);
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * sunLight.Color;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
    vec3 specular = sunLight.Color * spec * Specular;

    float attenuation = 1.0;
    diffuse *= attenuation;
    specular *= attenuation;

    // Calculate shadows
    float shadow = ShadowCalculation(lightSpaceMatrix * inverseViewMatrix * vec4(FragPos, 1.0f));
    vec3 sunLighting = (ambient + (1.0 - shadow) * (diffuse + specular));
    return sunLighting;
}

vec3 calcSpotLights() {
    // vec3 ambient = vec3(0.05f * AmbientOcclusion);
    // vec3 lighting = Diffuse * ambient;
    vec3 lighting = vec3(0.0f);
    vec3 viewDir = normalize(-FragPos);

    // Point lights
    // We reduce 1.0f from the light color as they're already too bright for Bloom.
    // This way the light keeps it's real color instead of being bright white.
    for (int i = 0; i < NR_LIGHTS; ++i)
    {
        // Calculate distance between light source and current fragment
        float distance = length(lights[i].Position - FragPos);
        // Diffuse
        vec3 lightDir = normalize(lights[i].Position - FragPos);
        vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * (lights[i].Color - vec3(1.5f));

        // Specular
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
        vec3 specular = (lights[i].Color - vec3(1.5f)) * spec * Specular;

        // Attenuation
        float attenuation = constant / (constant + lights[i].Linear * distance + lights[i].Quadratic * (distance * distance));
        diffuse  *= attenuation;
        specular *= attenuation;
        lighting += diffuse + specular + attenuation;
    }
    return lighting;
}

void main()
{
    // Calculate little lights
    vec3 result = calcSpotLights();

    // Add directional sun light
    result += calcSunLight();

    // Send bright stuff for post processing
    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));

    if (brightness > 1.0f)
        BrightColor = vec4(result, 1.0f);
    else
        BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    // Different rendering modes
    if(drawMode == 1)
        FragColor = vec4(result, 1.0f);
    else if(drawMode == 2)
        FragColor = vec4(vec3(Depth / 750.0f), 1.0f);
    else if(drawMode == 3)
        FragColor = vec4(FragPos, 1.0f);
    else if(drawMode == 4)
        FragColor = vec4(Normal, 1.0f);
    else if(drawMode == 5)
        FragColor = vec4(vec2(Velocity), 0.0f, 1.0f);
    else if(drawMode == 6)
        FragColor = vec4(vec3(AmbientOcclusion), 1.0f);
}
