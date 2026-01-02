#type vertex
#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inTangent;
layout(location = 3) in vec3 inBitangent;
layout(location = 4) in vec2 inTexCoord;

layout(push_constant) uniform Constants
{
    mat4 modelMatrix;
    vec3 radiance;
    vec3 direction;
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

layout(location = 0) out vec2 vTexCoord;
layout(location = 1) out vec3 vLightColor;
layout(location = 2) out vec3 vNormal;
layout(location = 3) out vec3 vLightDirection;
layout(location = 4) out vec3 vFragPos;
layout(location = 5) out vec3 vViewPos;

void main() 
{
    gl_Position = sceneData.proj * sceneData.view * pushConstants.modelMatrix * vec4(inPosition, 1.0);
    vTexCoord = inTexCoord;
    vLightColor = pushConstants.radiance.rgb;
    vNormal = mat3(transpose(inverse(pushConstants.modelMatrix))) * inNormal;
    vLightDirection = pushConstants.direction;
    vFragPos = vec3(pushConstants.modelMatrix * vec4(inPosition, 1.0));
    vViewPos = sceneData.cameraPos;
}

#type fragment
#version 450

layout(location = 0) in vec2 vTexCoord;
layout(location = 1) in vec3 vLightColor;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in vec3 vLightDirection;
layout(location = 4) in vec3 vFragPos;
layout(location = 5) in vec3 vViewPos;

layout(location = 0) out vec4 outColor;

// Material Descriptor Sets
layout(set = 0, binding = 0) uniform sampler2D diffuseTex;
layout(set = 0, binding = 1) uniform sampler2D normalTex;
layout(set = 0, binding = 2) uniform sampler2D metallicTex;
layout(set = 0, binding = 3) uniform sampler2D roughnessTex;
layout(set = 0, binding = 4) uniform MaterialData
{
    vec4 color;
    float metallic;
    float roughness;
} material;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = 3.1415 * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

struct PointLight
{
    vec4 radiance;
    vec3 position;
    float radius;
};

// Descriptor sets
layout(set = 1, binding = 0) uniform SceneData
{
    mat4 viewProjMatrix;
    vec3 cameraPos;
    uint pointLightsSize;
    PointLight pointLights[1024];
} sceneData;

vec3 CalculateDirectionalLight(vec3 F0)
{
    vec3 N = normalize(vNormal);
    vec3 V = normalize(vViewPos - vFragPos);
    vec3 color = vec3(0.0f);
    
    // calculate per-light radiance
    vec3 L = -vLightDirection;
    vec3 H = normalize(V + L);

    vec3 radiance = vLightColor;

    // cook-torrance brdf
    float NDF = DistributionGGX(N, H, material.roughness);
    float G = GeometrySmith(N, V, L, material.roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - material.metallic;

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    // add to outgoing radiance Lo
    float NdotL = max(dot(N, L), 0.0);
    return (kD * material.color.rgb / 3.1415 + specular) * radiance * NdotL;
}

void main() 
{
    vec3 N = normalize(vNormal);
    vec3 V = normalize(vViewPos - vFragPos);

    vec3 materialColor = material.color.rgb * texture(diffuseTex, vTexCoord).rgb;

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, materialColor, material.metallic);

    // reflectance equation
    vec3 Lo = CalculateDirectionalLight(F0);
    for (int i = 0; i < sceneData.pointLightsSize; ++i)
    {
        // calculate per-light radiance
        vec3 L = normalize(sceneData.pointLights[i].position - vFragPos);
        vec3 H = normalize(V + L);
        float distance = length(sceneData.pointLights[i].position - vFragPos);

        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = sceneData.pointLights[i].radiance.rgb * attenuation;

        // cook-torrance brdf
        float NDF = DistributionGGX(N, H, material.roughness);
        float G = GeometrySmith(N, V, L, material.roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - material.metallic;

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        // add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * material.color.rgb / 3.1415 + specular) * sceneData.pointLights[i].radiance.rgb * NdotL;
    }

    vec3 ambient = vec3(0.03) * materialColor * 1.0;
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    outColor = vec4(color, 1.0);
}