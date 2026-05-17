//***************************************************************************************
// color.hlsl by Frank Luna (C) 2015 All Rights Reserved.
// Modified by Shinyoung Yi 2026
//
// Transforms and colors geometry.
//***************************************************************************************
#include "LightingUtil.hlsl"

cbuffer cbPerObject : register(b0) // The C++ Program recognize this buffer through `register(b0), not names`
	// cbuffer = Constant Buffer
{
    float4x4 gWorld;
    float3 gDiffAlbedo;
    float gPerObjectPad1;
    float3 gSpecAlbedo;
    float gPerObjectPad2;
    float gShininess;
};

cbuffer cbPass : register(b1)
{
    float4x4 gView;
    float4x4 gProj;
    float3 gEyePosW;
    float cbPerPassPad1;
    float3 gAmbientLight;
    float cbPerPassPad2;
    int gNumDirLights;
    int gNumPointLights;
    int gNumSpotLights;
    float cbPerPassPad3;
    Light gLights[MaxLights];
};

struct VertexIn
{
	float3 PosL    : POSITION;
    float4 Color   : COLOR;
    float3 NormalL : NORMAL;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
};

VertexOut VS(VertexIn vin) // Vertex Shader
{
    VertexOut vout;
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosW = posW.xyz;
    // Transform to homogeneous clip space
    vout.PosH = mul(mul(posW, gView), gProj);
    // Assume there is no non-uniform scaling and shear
    // [WARNING] If we add an object with anisotropic scale transform,
    // we need to add gWorldInv to the object constant buffer.
    vout.NormalW = normalize( mul(vin.NormalL, (float3x3) gWorld) );
    return vout;
}

float4 PS(VertexOut pin) : SV_Target // Pixel Shader
{
    // interpolating normal can unnormalize it, so renormalize it.
    pin.NormalW = normalize(pin.NormalW);
    float3 toEyeW = normalize(gEyePosW - pin.PosW);
    float3 ambient = gAmbientLight * gDiffAlbedo;
    Material mat = { gDiffAlbedo, gSpecAlbedo, gShininess };
    float3 directLight = ComputeLighting(gLights, gNumDirLights, gNumPointLights, gNumSpotLights,
                                         mat, pin.PosW, pin.NormalW, toEyeW);
    float4 litColor;
    litColor.rgb = ambient + directLight;
    litColor.a = 1.0;
    return litColor;
}


