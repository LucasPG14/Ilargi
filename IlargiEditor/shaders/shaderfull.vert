#type vertex
#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inTangent;
layout(location = 3) in vec3 inBitangent;
layout(location = 4) in vec2 inTexCoord;

layout(binding = 0) uniform UniformBufferObject 
{
    mat4 viewProj;
} ubo;

layout(push_constant) uniform Constants
{
    mat4 modelMatrix;
    mat4 viewProj;
    vec4 color;
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

layout(push_constant) uniform Material
{
    vec4 color;
} material;

void main() 
{
    outColor = vec4(fragColor, 1.0);
}