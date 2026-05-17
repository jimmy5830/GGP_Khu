//***************************************************************************************
// LightingUtil.hlsl by Frank Luna (C) 2015 All Rights Reserved.
// Modified by Shinyoung Yi 2026
//
// Contains API for shader lighting.
//***************************************************************************************
#define MaxLights 16

struct Light
{
    float3 Strength;
    float FalloffStart; // point/spot light only
    float3 Direction; // directional/spot light only
    float FalloffEnd; // point/spot light only
    float3 Position; // point light only
    float SpotPower; // spot light only
};

struct Material
{
    float3 diffAlbedo;
    float3 specAlbedo;
    float Shininess;
};

float CalcAttenuation(float d, float falloffStart, float falloffEnd)
{
    return saturate((falloffEnd - d) / (falloffEnd - falloffStart));
}

float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye, 
                  Material mat)
{
    float3 halfVec = normalize(toEye + lightVec);
    const float m = mat.Shininess * 256.0f;
    float roughnessFactor = (m + 8.0f) * pow(max(dot(halfVec, normal), 0.0f), m) / 8.0f;
    float3 specFactor = mat.specAlbedo * roughnessFactor;
    // Our spec formula goes outside [0,1] range, but we are
    // doing LDR rendering. So scale it down a bit.
    specFactor = specFactor / (specFactor + 1.0f);
    
    return (mat.diffAlbedo + specFactor) * lightStrength;
}

//---------------------------------------------------------------------------------------
// Evaluates the lighting equation for directional lights.
//---------------------------------------------------------------------------------------
float3 ComputeDirectionalLight(Light L, Material mat, float3 normal, float3 toEye)
{
    float3 lightVec = -L.Direction;
    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.Strength * ndotl;
    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
 
}


//---------------------------------------------------------------------------------------
// Evaluates the lighting equation for point lights.
//---------------------------------------------------------------------------------------
float3 ComputePointLight(Light L, Material mat, float3 pos, float3 normal, float3 toEye)
{
    float3 lightVec = L.Position - pos;
    float d = length(lightVec);
    lightVec /= d;

    float att = CalcAttenuation(d, L.FalloffStart, L.FalloffEnd);
    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.Strength * ndotl * att;

    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}


//---------------------------------------------------------------------------------------
// Evaluates the lighting equation for spot lights.
//---------------------------------------------------------------------------------------
float3 ComputeSpotLight(Light L, Material mat, float3 pos, float3 normal, float3 toEye)
{
    float3 lightVec = L.Position - pos;
    float d = length(lightVec);
    lightVec /= d;

    float att = CalcAttenuation(d, L.FalloffStart, L.FalloffEnd);
    float spotFactor = pow(max(dot(-lightVec, L.Direction), 0.0f), L.SpotPower);
    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.Strength * ndotl * att * spotFactor;

    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}


float3 ComputeLighting(Light gLights[MaxLights],
                       int numDirLights, int numPointLights, int numSpotLights,
                       Material mat, float3 pos, float3 normal, float3 toEye)
{
    float3 result = 0.0f;
    int i = 0;

    for (i = 0; i < numDirLights; ++i)
        result += ComputeDirectionalLight(gLights[i], mat, normal, toEye);

    for (i = numDirLights; i < numDirLights + numPointLights; ++i)
        result += ComputePointLight(gLights[i], mat, pos, normal, toEye);

    for (i = numDirLights + numPointLights; i < numDirLights + numPointLights + numSpotLights; ++i)
        result += ComputeSpotLight(gLights[i], mat, pos, normal, toEye);

    return result;
}