#version 430
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform vec3 lightColor;

void main()
{
    FragColor = vec4(lightColor, 1.0f);
    float brightness = dot(lightColor, vec3(0.2126f, 0.7152f, 0.0722f));

    if(brightness > 1.0f)
        BrightColor = vec4(lightColor, 1.0f);
    else
        BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
