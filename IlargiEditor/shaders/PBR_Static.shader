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
    vec4 radiance;
    vec3 direction;
} pushConstants;

layout(location = 0) out vec2 vTexCoord;
layout(location = 1) out vec3 normal;

layout(location = 2) out vec3 dir;
layout(location = 3) out vec4 lightColor;

void main() 
{
    gl_Position = pushConstants.viewProj * pushConstants.modelMatrix * vec4(inPosition, 1.0); 
    vTexCoord = inTexCoord;
    normal = inNormal;
    dir = pushConstants.direction;
    lightColor = pushConstants.radiance;
}

#type fragment
#version 450

layout(location = 0) in vec2 vTexCoord;
layout(location = 1) in vec3 normal;

layout(location = 2) in vec3 dir;
layout(location = 3) in vec4 lightColor;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D texSampler;

void main() 
{
    vec4 col = texture(texSampler, vTexCoord);
    
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-dir);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor.rgb;
	 
    //col.rgb *= diffuse;

    col.rgb = pow(col.rgb, vec3(1.0 / 2.2));
    outColor = vec4(col.rgb, col.a);
}