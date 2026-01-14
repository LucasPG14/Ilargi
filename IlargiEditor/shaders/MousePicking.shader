#type vertex
#version 450

layout(location = 0) in vec3 inPosition;

layout(push_constant) uniform Constants
{
    mat4 modelMatrix;
    uint objectID;
} pushConstants;

struct PointLight
{
    vec3 radiance;
    float radius;
    vec3 position;
};

// Descriptor sets
layout(set = 1, binding = 0) uniform SceneData
{
    mat4 proj;
    mat4 view;
    vec3 cameraPos;
    uint pointLightsSize;
    PointLight pointLights[1024];
} sceneData;

void main() 
{
    gl_Position = sceneData.proj * sceneData.view * pushConstants.modelMatrix * vec4(inPosition, 1.0);
}

#type fragment
#version 450

layout(push_constant) uniform Constants
{
    mat4 modelMatrix;
    uint objectID;
} pushConstants;

layout(location = 0) out uint outID;

void main() 
{
    outID = pushConstants.objectID;
}