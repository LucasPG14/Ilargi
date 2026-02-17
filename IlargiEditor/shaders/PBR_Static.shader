#type vertex
#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inTangent;
layout(location = 3) in vec2 inTexCoords;

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

layout(set = 0, binding = 0) uniform SceneData
{
    mat4 proj;
    mat4 view;
    vec3 cameraPos;
    uint pointLightsSize;
    PointLight pointLights[1024];
} sceneData;

layout(location = 0) out vec2 vTexCoords;
//layout(location = 1) out vec3 vNormal;
layout(location = 2) out vec3 vFragPos;
layout(location = 3) out mat3 vTBN;
layout(location = 10) out vec3 vCameraPosition;
layout(location = 8) out vec3 vDirectionalColor;
layout(location = 9) out vec3 vDirection;

void main() 
{
    vFragPos = vec3(pushConstants.modelMatrix * vec4(inPosition, 1.0));
    
    vTexCoords = inTexCoords;
    //vNormal = inNormal;
    vDirectionalColor = pushConstants.radiance;
    vDirection = pushConstants.direction;

    vec3 T = normalize(vec3(pushConstants.modelMatrix * vec4(inTangent.xyz, 0.0)));
    vec3 N = normalize(vec3(pushConstants.modelMatrix * vec4(inNormal, 0.0)));
    vec3 B = cross(N, T) * inTangent.w;

    vTBN = mat3(T, B, N);

    vCameraPosition = sceneData.cameraPos;
    
    gl_Position = sceneData.proj * sceneData.view * vec4(vFragPos, 1.0);
}

#type fragment
#version 450

layout(location = 0) in vec2 vTexCoords;
//layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec3 vFragPos;
layout(location = 3) in mat3 vTBN;
layout(location = 10) in vec3 vCameraPosition;
layout(location = 8) in vec3 vDirectionalColor;
layout(location = 9) in vec3 vDirection;

layout(location = 0) out vec4 outColor;

// Material Descriptor Sets
layout(set = 1, binding = 0) uniform sampler2D DiffuseMap;
layout(set = 1, binding = 1) uniform sampler2D NormalMap;
layout(set = 1, binding = 2) uniform sampler2D RoughnessMap;
layout(set = 1, binding = 4) uniform MaterialData
{
    vec4 color;
    float metallic;
    float roughness;
} material;

struct PointLight
{
    vec4 radiance;
    vec3 position;
    float radius;
};

// Descriptor sets
layout(set = 0, binding = 0) uniform SceneData
{
    mat4 proj;
    mat4 view;
    vec3 cameraPos;
    uint pointLightsSize;
    PointLight pointLights[1024];
} sceneData;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

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

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 CalculateDirectionalLight(vec3 N, vec3 V, vec3 F0, vec3 materialColor, vec2 metallicRoughness)
{
    vec3 L = normalize(-vDirection);
    vec3 H = normalize(V + L);

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, metallicRoughness.g);
    float G = GeometrySmith(N, V, L, metallicRoughness.g);
    vec3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
    vec3 specular = numerator / denominator;

    // kS is equal to Fresnel
    vec3 kS = F;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0) - kS;
    // multiply kD by the inverse metalness such that only non-metals 
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallicRoughness.r;

    // scale light by NdotL
    float NdotL = max(dot(N, L), 0.0);

    // add to outgoing radiance Lo
    return (kD * materialColor / PI + specular) * vDirectionalColor * NdotL;
}

vec3 CalculatePointLight(PointLight light, vec3 N, vec3 V, vec3 F0, vec3 materialColor, vec2 metallicRoughness)
{
    // calculate per-light radiance
    vec3 L = normalize(light.position - vFragPos);
    vec3 H = normalize(V + L);
    float distance = length(light.position - vFragPos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = light.radiance.rgb;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, metallicRoughness.g);
    float G = GeometrySmith(N, V, L, metallicRoughness.g);
    vec3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
    vec3 specular = numerator / denominator;

    // kS is equal to Fresnel
    vec3 kS = F;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0) - kS;
    // multiply kD by the inverse metalness such that only non-metals 
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallicRoughness.r;

    // scale light by NdotL
    float NdotL = max(dot(N, L), 0.0);

    // add to outgoing radiance Lo
    return (kD * materialColor / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
}

void main() 
{
    vec3 norm = texture(NormalMap, vTexCoords).rgb * 2.0 - 1.0;
    norm = normalize(vTBN * norm);

    vec2 metallicRoughness = texture(RoughnessMap, vTexCoords).rg;
    
    vec3 lightDir = normalize(-vDirection);
    vec3 V = normalize(vCameraPosition - vFragPos);

    vec3 materialColor = material.color.rgb * texture(DiffuseMap, vTexCoords).rgb;

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, materialColor, metallicRoughness.r);

    // reflectance equation
    vec3 Lo = CalculateDirectionalLight(norm, V, F0, materialColor, metallicRoughness);

    for (int i = 0; i < sceneData.pointLightsSize; ++i)
    {
        Lo += CalculatePointLight(sceneData.pointLights[i], norm, lightDir, F0, materialColor, metallicRoughness);
    }

    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.03) * materialColor * 1.0;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));

    outColor = vec4(color, 1.0);
}