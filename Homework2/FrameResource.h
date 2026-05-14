#pragma once
#include <string>
#include "d3dUtil.h"
#include "MathHelper.h"
#include "UploadBuffer.h"
#include "config.h"

struct Vertex
{
    DirectX::XMFLOAT3 Pos;
    DirectX::XMFLOAT4 Color;
    DirectX::XMFLOAT3 Normal;
};

struct ObjectConstants
{
    DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
    DirectX::XMFLOAT3 DiffAlbedo;
    float cbPerObjectPad1 = 0.0f; // Must be 16 bytes-aligned
    DirectX::XMFLOAT3 SpecAlbedo;
    float cbPerObjectPad2 = 0.0f; // Must be 16 bytes-aligned
    float Shininess;
};

struct PassConstants
{
    DirectX::XMFLOAT4X4 View = MathHelper::Identity4x4();
    DirectX::XMFLOAT4X4 Proj = MathHelper::Identity4x4();
    DirectX::XMFLOAT3 EyePos;
    float cbPerPassPad1 = 0.0f;
    DirectX::XMFLOAT3 AmbientLight;
    float cbPerPassPad2 = 0.0f;
    Light Lights[1];
};


struct FrameResource
{
    FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount);
    FrameResource(const FrameResource&) = delete;
    FrameResource& operator=(const FrameResource&) = delete;
    ~FrameResource() {}

    Microsoft::WRL::ComPtr<ID3D12CommandAllocator>   CmdListAlloc;
    std::unique_ptr<UploadBuffer<PassConstants>>      PassCB = nullptr;
    std::unique_ptr<UploadBuffer<ObjectConstants>>    ObjectCB = nullptr;
    UINT64 Fence = 0;
};

struct MaterialHW2 {
    std::string Name;
    DirectX::XMFLOAT3 DiffAlbedo;
    DirectX::XMFLOAT3 SpecAlbedo;
    float Shininess;
};