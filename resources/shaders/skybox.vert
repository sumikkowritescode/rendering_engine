#version 430
layout (location = 0) in vec3 position;
out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    vec4 pos = projection * view * vec4(position, 1.0);
    gl_Position = pos.xyww; // Set the skyobx's depth to 1.0 always - w/w = 1.0

    TexCoords = position;
}
