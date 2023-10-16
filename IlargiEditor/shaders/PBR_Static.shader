#type vertex
#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inTangent;
layout(location = 3) in vec3 inBitangent;
layout(location = 4) in vec2 inTexCoord;

layout(set = 0, binding = 0) uniform UniformBufferObject 
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
layout(location = 1) out vec2 vTexCoord;

void main() 
{
    gl_Position = pushConstants.viewProj * pushConstants.modelMatrix * vec4(inPosition, 1.0); 
    fragColor = inPosition;
    vTexCoord = inTexCoord;
}

#type fragment
#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 vTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D texSampler;

void main() 
{
    vec4 col = texture(texSampler, vTexCoord);
    col.rgb = pow(col.rgb, vec3(1.0 / 2.2));
    outColor = vec4(col.rgb, col.a);
}