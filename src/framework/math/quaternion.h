#pragma once

#include "../render/dxtypes.h"

namespace Grafkit
{
    class Quaternion
    {
    public:
        // ctor + copy ctor 
        Quaternion() { m_q = DirectX::XMVectorSet(0, 0, 0, 0); }

        Quaternion(float x, float y, float z, float w) { m_q = DirectX::XMVectorSet(x, y, z, w); }

        explicit Quaternion(const dxvector& in) : m_q(in) {
        }

        Quaternion(const Quaternion& in) : m_q(in.m_q) {
        }

        explicit Quaternion(const float4& in) { m_q = XMLoadFloat4(&in); }

        // --- setters
        void Set(float x, float y, float z, float w) { m_q = DirectX::XMVectorSet(x, y, z, w); }

        // --- operations
        void Slerp(const Quaternion& q0, const Quaternion& q1, float t) { m_q = DirectX::XMQuaternionSlerp(q0.m_q, q1.m_q, t); }

        Quaternion& operator*(const Quaternion& in)
        {
            m_q = DirectX::XMQuaternionMultiply(m_q, in.m_q);
            return *this;
        }

        // --- operator = 
        Quaternion& operator=(const Quaternion& in)
        {
            m_q = in.m_q;
            return *this;
        }

        Quaternion& operator=(const dxvector& in)
        {
            m_q = in;
            return *this;
        }

        Quaternion& operator=(const float4& in)
        {
            m_q = XMLoadFloat4(&in);
            return *this;
        }

        // --- conversion

        operator matrix() const { return DirectX::XMMatrixRotationQuaternion(m_q); }

        static Quaternion FromEuler(float r, float p, float y) { return Quaternion(DirectX::XMQuaternionRotationRollPitchYaw(r, p, y)); }

        static Quaternion FromEuler(float3 rpy) { return Quaternion(DirectX::XMQuaternionRotationRollPitchYaw(rpy.x, rpy.y, rpy.z)); }

        float4 ToAxisAngle() const
        {
            dxvector axis;
            float angle = 0;
            DirectX::XMQuaternionToAxisAngle(&axis, &angle, m_q);
            float4 res;
            XMStoreFloat4(&res, axis);
            res.w = angle;
            return res;
        }

        operator float4() const
        {
            float4 res;
            XMStoreFloat4(&res, m_q);
            return res;
        }

        friend bool operator==(const Quaternion& lhs, const Quaternion& rhs)
        {
            uint32_t cr = 0;
            DirectX::XMVectorEqualR(&cr, lhs.m_q, rhs.m_q);
            return DirectX::XMComparisonAllTrue(cr);
        }

        friend bool operator!=(const Quaternion& lhs, const Quaternion& rhs) { return !(lhs == rhs); }

        template <class ARCHIVE>
        void Serialize(ARCHIVE& ar);

    private:
        dxvector m_q;
    };

    template <class ARCHIVE>
    void Quaternion::Serialize(ARCHIVE& ar)
    {
        float4 q = float4(*this);
        ar & q;
        m_q = DirectX::XMLoadFloat4(&q);
    }
}
