#pragma once

#include <DirectXMath.h>
#include <DirectXColors.h>

#include <d3d.h>
#include <d3d11.h>
#include <d3dtypes.h>	
#include <d3dcompiler.h>

//using namespace DirectX;

typedef DirectX::XMFLOAT2 float2;
typedef DirectX::XMFLOAT3 float3;
typedef DirectX::XMFLOAT4 float4;

typedef DirectX::XMVECTOR dxvector;
typedef DirectX::XMMATRIX matrix;

// usage of left-handed coordinate system
#define USE_LEFT_HANDED_SYSTEM
