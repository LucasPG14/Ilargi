#type vertex
#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(binding = 0) uniform UniformBufferObject 
{
    mat4 viewProj;
} ubo;

layout(push_constant) uniform Constants
{
    mat4 modelMatrix;
    mat4 viewProj;
} pushConstants;

layout(location = 0) out vec3 fragColor;

void main() 
{
    gl_Position = pushConstants.viewProj * pushConstants.modelMatrix * vec4(inPosition, 1.0); 
    fragColor = inPosition;
}
#type fragment
#version 450

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

void main() 
{
    outColor = vec4(fragColor, 1.0);
}