#version 450

// layout(binding = 0) uniform UniformObject
// {
//     mat4 model;
//     mat4 view;
//     mat4 proj;
// }
// ubo;

layout(binding = 0) uniform UniformObject
{
    mat4 mvp;
}
ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main()
{
    // gl_Position = ubo.model * ubo.view * ubo.proj * vec4(inPosition, 1.0);
    gl_Position = ubo.mvp * vec4(inPosition, 1.0);
    fragColor = inColor;
}