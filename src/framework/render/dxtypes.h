#pragma once

#include <DirectXMath.h>
#include <DirectXColors.h>

#include <d3d.h>
#include <d3d11.h>
#include <d3dtypes.h>
#include <d3dcompiler.h>

struct float2 : DirectX::XMFLOAT2
{
    float2() : XMFLOAT2(0.f, 0.f) {
    }

    float2(float _x, float _y)
        : XMFLOAT2(_x, _y) {
    }

    explicit float2(const float* pArray)
        : XMFLOAT2(pArray) {
    }

    template <class AR>
    void Serialize(AR& ar) { ar & x & y; }
};

struct float3 : DirectX::XMFLOAT3
{
    float3() : XMFLOAT3(0.f, 0.f, 0.f) {
    }

    float3(float _x, float _y, float _z)
        : XMFLOAT3(_x, _y, _z) {
    }

    explicit float3(const float* pArray)
        : XMFLOAT3(pArray) {
    }

    template <class AR>
    void Serialize(AR& ar) { ar & x & y & z; }
};

struct float4 : DirectX::XMFLOAT4
{
    float4() : XMFLOAT4(0.f, 0.f, 0.f, 0.f) {
    }

    float4(float _x, float _y, float _z, float _w)
        : XMFLOAT4(_x, _y, _z, _w) {
    }

    explicit float4(const float* pArray)
        : XMFLOAT4(pArray) {
    }

    template <class AR>
    void Serialize(AR& ar) { ar & x & y & z & w; }
};


typedef DirectX::XMVECTOR dxvector;
typedef DirectX::XMMATRIX matrix;

// usage of left-handed coordinate system
#define USE_LEFT_HANDED_SYSTEM
