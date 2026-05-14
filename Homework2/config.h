#pragma once
#include<DirectXMath.h>
#include "d3dUtil.h"

#define CAMERA_INIT 0 // 0, 1, 2, 3: animuthal rotation by 90 degeress, 4: top-view
#define MATERIAL_INIT 0 // 0: Default (diffuse + specular)
						// 1: Diffuse only
					    // 2: Specular only
						// 3: Specular only (strong)
#define LIGHT_OPTION 3 // 0: dir only
					   // 1: point only
					   // 2: spot only
					   // 3: 1 dir + 1 point + 1 spot

#if CAMERA_INIT < 4
const float THETA_INIT = (1.5f + 0.5f*CAMERA_INIT) * DirectX::XM_PI;
const float PHI_INIT = DirectX::XM_PI * 60/180;
#else
const float THETA_INIT = 1.5f * DirectX::XM_PI;
const float PHI_INIT = 0.001f;
#endif
const float RADIUS_INIT= 15.0f;

//---------------------------------------------------------------------------------------
// Light Sources
//---------------------------------------------------------------------------------------

// Ambient Light
//const DirectX::XMFLOAT3 AMBIENT_LIGHT_INIT = { 0.f, 0.f, 0.f };
const DirectX::XMFLOAT3 AMBIENT_LIGHT_INIT = { 0.25f, 0.25f, 0.35f };

// Dirctional Light Sources
#if LIGHT_OPTION == 0 || LIGHT_OPTION == 3
#define NUM_DIR_LIGHTS 1
const DirectX::XMFLOAT3 DIR_LIGHT_DIRECTIONS[NUM_DIR_LIGHTS] = {
	{ 0.57735f, -0.57735f, 0.57735f } // {0.f, -1.f, 0.f}
};
const DirectX::XMFLOAT3 DIR_LIGHT_STRENGTHS[NUM_DIR_LIGHTS] = {
	{0.6f, 0.6f, 0.6f}
};
#else
#define NUM_DIR_LIGHTS 0
#endif

// Point Light Sources
#if LIGHT_OPTION == 1 || LIGHT_OPTION == 3
#define NUM_POINT_LIGHTS 1
const DirectX::XMFLOAT3 POINT_LIGHT_POSITIONS[NUM_POINT_LIGHTS] = {
	{ 2.0f, 3.0f, 0.0f }
};
const DirectX::XMFLOAT3 POINT_LIGHT_STRENGTHS[NUM_POINT_LIGHTS] = {
	{ 0.8f, 0.8f, 0.0f }
};
const float              POINT_LIGHT_FALLOFF_STARTS[NUM_POINT_LIGHTS] = {
	1.0f
};
const float              POINT_LIGHT_FALLOFF_ENDS[NUM_POINT_LIGHTS] = {
	10.0f
};
#else
#define NUM_POINT_LIGHTS 0
#endif

// Spot Light Sources
#if LIGHT_OPTION == 2 || LIGHT_OPTION == 3
#define NUM_SPOT_LIGHTS 1
const DirectX::XMFLOAT3 SPOT_LIGHT_POSITIONS[NUM_SPOT_LIGHTS] = {
	{ -2.0f, 4.0f, 0.0f }
};
const DirectX::XMFLOAT3 SPOT_LIGHT_DIRECTIONS[NUM_SPOT_LIGHTS] = {
	{ 2.0f / sqrt(20.f) * 1.1f, -4.0f / sqrt(20.f) * 1.1f, 0.0f}
};
const DirectX::XMFLOAT3 SPOT_LIGHT_STRENGTHS[NUM_SPOT_LIGHTS] = {
	{ 0.2f, 1.0f, 4.0f }
};
const float              SPOT_LIGHT_FALLOFF_STARTS[NUM_SPOT_LIGHTS] = {
	1.0f
};
const float              SPOT_LIGHT_FALLOFF_ENDS[NUM_SPOT_LIGHTS] = {
	10.0f
};
const float              SPOT_LIGHT_SPOTPOWER[NUM_SPOT_LIGHTS] = {
	32.0f
};
#else
#define NUM_SPOT_LIGHTS 0
#endif

static_assert(NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS <= MaxLights,
	"To total number of directional, positional, and spot lights cannot be exceed `MaxLights`.");

//---------------------------------------------------------------------------------------
// Materials
//---------------------------------------------------------------------------------------
#if MATERIAL_INIT == 0 // Default (diffuse + specular)
const DirectX::XMFLOAT3 MAT_BRICKS_DIFF = DirectX::XMFLOAT3(DirectX::Colors::ForestGreen);
const DirectX::XMFLOAT3 MAT_BRICKS_SPEC = DirectX::XMFLOAT3(0.02f, 0.02f, 0.02f);
const float             MAT_BRICKS_SHIN = 0.9f;

const DirectX::XMFLOAT3 MAT_STONE_DIFF = DirectX::XMFLOAT3(DirectX::Colors::LightSteelBlue);
const DirectX::XMFLOAT3 MAT_STONE_SPEC = DirectX::XMFLOAT3(0.05f, 0.05f, 0.05f);
const float             MAT_STONE_SHIN = 0.7f;

const DirectX::XMFLOAT3 MAT_TILE_DIFF = DirectX::XMFLOAT3(DirectX::Colors::LightGray);
const DirectX::XMFLOAT3 MAT_TILE_SPEC = DirectX::XMFLOAT3(0.02f, 0.02f, 0.02f);
const float             MAT_TILE_SHIN = 0.8f;

const DirectX::XMFLOAT3 MAT_SKULL_DIFF = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
const DirectX::XMFLOAT3 MAT_SKULL_SPEC = DirectX::XMFLOAT3(0.05f, 0.05f, 0.05);
const float             MAT_SKULL_SHIN = 0.7f;

#elif MATERIAL_INIT == 1 // Diffuse only
const DirectX::XMFLOAT3 MAT_BRICKS_DIFF = DirectX::XMFLOAT3(DirectX::Colors::ForestGreen);
const DirectX::XMFLOAT3 MAT_BRICKS_SPEC = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
const float             MAT_BRICKS_SHIN = 0.9f;

const DirectX::XMFLOAT3 MAT_STONE_DIFF = DirectX::XMFLOAT3(DirectX::Colors::LightSteelBlue);
const DirectX::XMFLOAT3 MAT_STONE_SPEC = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
const float             MAT_STONE_SHIN = 0.7f;

const DirectX::XMFLOAT3 MAT_TILE_DIFF = DirectX::XMFLOAT3(DirectX::Colors::LightGray);
const DirectX::XMFLOAT3 MAT_TILE_SPEC = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
const float             MAT_TILE_SHIN = 0.8f;

const DirectX::XMFLOAT3 MAT_SKULL_DIFF = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
const DirectX::XMFLOAT3 MAT_SKULL_SPEC = DirectX::XMFLOAT3(0.f, 0.f, 0.);
const float             MAT_SKULL_SHIN = 0.7f;

#elif MATERIAL_INIT == 2 // Specular only
const DirectX::XMFLOAT3 MAT_BRICKS_DIFF = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
const DirectX::XMFLOAT3 MAT_BRICKS_SPEC = DirectX::XMFLOAT3(0.02f, 0.02f, 0.02f);
const float             MAT_BRICKS_SHIN = 0.9f;

const DirectX::XMFLOAT3 MAT_STONE_DIFF = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
const DirectX::XMFLOAT3 MAT_STONE_SPEC = DirectX::XMFLOAT3(0.05f, 0.05f, 0.05f);
const float             MAT_STONE_SHIN = 0.7f;

const DirectX::XMFLOAT3 MAT_TILE_DIFF = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
const DirectX::XMFLOAT3 MAT_TILE_SPEC = DirectX::XMFLOAT3(0.02f, 0.02f, 0.02f);
const float             MAT_TILE_SHIN = 0.8f;

const DirectX::XMFLOAT3 MAT_SKULL_DIFF = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
const DirectX::XMFLOAT3 MAT_SKULL_SPEC = DirectX::XMFLOAT3(0.05f, 0.05f, 0.05);
const float             MAT_SKULL_SHIN = 0.7f;

#else // 3: Specular only (strong)
const DirectX::XMFLOAT3 MAT_BRICKS_DIFF = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
const DirectX::XMFLOAT3 MAT_BRICKS_SPEC = DirectX::XMFLOAT3(0.4f, 0.4f, 0.4f);
const float             MAT_BRICKS_SHIN = 0.9f;

const DirectX::XMFLOAT3 MAT_STONE_DIFF = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
const DirectX::XMFLOAT3 MAT_STONE_SPEC = DirectX::XMFLOAT3(1.f, 1.f, 1.f);
const float             MAT_STONE_SHIN = 0.7f;

const DirectX::XMFLOAT3 MAT_TILE_DIFF = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
const DirectX::XMFLOAT3 MAT_TILE_SPEC = DirectX::XMFLOAT3(0.4f, 0.4f, 0.4f);
const float             MAT_TILE_SHIN = 0.8f;

const DirectX::XMFLOAT3 MAT_SKULL_DIFF = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
const DirectX::XMFLOAT3 MAT_SKULL_SPEC = DirectX::XMFLOAT3(1.f, 1.f, 1.f);
const float             MAT_SKULL_SHIN = 0.7f;

#endif
