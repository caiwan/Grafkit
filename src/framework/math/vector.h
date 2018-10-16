#pragma once
#include "render/dxtypes.h"

namespace Grafkit { namespace Vector
{
    static float2 Normalize(float2 v)
    {
        auto xv = XMLoadFloat2(&v);
        auto xRes = DirectX::XMVector2Normalize(xv);
        float2 res;
        XMStoreFloat2(&res, xRes);
        return res;
    }

    static float3 Normalize(float3 v)
    {
        auto xv = XMLoadFloat3(&v);
        auto xRes = DirectX::XMVector3Normalize(xv);
        float3 res;
        XMStoreFloat3(&res, xRes);
        return res;
    }

    static float4 Normalize(float4 v)
    {
        auto xv = XMLoadFloat4(&v);
        auto xRes = DirectX::XMVector4Normalize(xv);
        float4 res;
        XMStoreFloat4(&res, xRes);
        return res;
    }
};
};

inline float3 operator+(const float3& a, const float3& b) { return float3(a.x + b.x, a.y + b.y, a.z + b.z); }
inline float3 operator-(const float3& a, const float3& b) { return float3(a.x - b.x, a.y - b.y, a.z - b.z); }

inline float3 operator+(const float3& a, const float& b) { return float3(a.x + b, a.y + b, a.z + b); }

inline float3 operator-(const float3& a, const float& b) { return float3(a.x - b, a.y - b, a.z - b); }

inline float3 operator*(const float3& a, const float3& b) { return float3(a.x * b.x, a.y * b.y, a.z * b.z); }
inline void operator*=(float3& a, const float3& b) { a = float3(a.x * b.x, a.y * b.y, a.z * b.z); }

inline float3 operator*(const float3& a, const float& b) { return float3(a.x * b, a.y * b, a.z * b); }
inline void operator*=(float3& a, const float& b) { a = float3(a.x * b, a.y * b, a.z * b); }

inline float4 operator+(const float4& a, const float4& b) { return float4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w); }
inline float4 operator-(const float4& a, const float4& b) { return float4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w); }

inline float4 operator+(const float4& a, const float& b) { return float4(a.x + b, a.y + b, a.z + b, a.w + b); }

inline float4 operator-(const float4& a, const float& b) { return float4(a.x - b, a.y - b, a.z - b, a.w - b); }

inline float4 operator*(const float4& a, const float4& b) { return float4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w); }
inline void operator*=(float4& a, const float4& b) { a = float4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w); }

inline float4 operator*(const float4& a, const float& b) { return float4(a.x * b, a.y * b, a.z * b, a.w * b); }
inline void operator*=(float4& a, const float& b) { a = float4(a.x * b, a.y * b, a.z * b, a.w * b); }

inline bool operator==(const float2& a, const float2& b) { return a.x == b.x && a.y == b.y; }
inline bool operator==(const float3& a, const float3& b) { return a.x == b.x && a.y == b.y && a.z == b.z; }
inline bool operator==(const float4& a, const float4& b) { return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w; }

inline bool operator!=(const float2& a, const float2& b) { return !(a == b); }
inline bool operator!=(const float3& a, const float3& b) { return !(a == b); }
inline bool operator!=(const float4& a, const float4& b) { return !(a == b); }

// This would be the best if we'd override it somehow
//template <class ARCHIVE>
//void operator&(ARCHIVE& ar, float2& v) { ar & v.x & v.y; }
//
//template <class ARCHIVE>
//void operator&(ARCHIVE& ar, float3& v) { ar & v.x & v.y & v.z; }
//
//template <class ARCHIVE>
//void operator&(ARCHIVE& ar, float4& v) { ar & v.x & v.y & v.z & v.w; }
