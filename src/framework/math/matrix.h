#pragma once
#include <array>
#include "render/dxtypes.h"
#include "quaternion.h"
#include "vector.h"
#include "core/memory.h"

namespace Grafkit
{
    /**
    Extends DirectX::XMMATRIX with operations to make thing easier, and use as a real type.
    https://msdn.microsoft.com/en-us/library/windows/desktop/ee415594(v=vs.85).aspx

    NOTE THIS FUCKER ALL THE TIME http://stackoverflow.com/questions/11285680/crash-after-m-xmmatrixidentity-aligment-memory-in-classes

    `::matrix` is a shortcut to `DirectX::XMMATRIX`
    */

    __declspec(align(16)) class Matrix : public AlignedNew<Matrix>
    {
    public:
        // --- constructors
        Matrix() { this->Identity(); }

        Matrix(
            float a1, float a2, float a3, float a4,
            float b1, float b2, float b3, float b4,
            float c1, float c2, float c3, float c4,
            float d1, float d2, float d3, float d4
        )
        {
            this->m_mat = DirectX::XMMatrixSet(
                a1, a2, a3, a4,
                b1, b2, b3, b4,
                c1, c2, c3, c4,
                d1, d2, d3, d4
            );
        }

        Matrix(const matrix& m) : m_mat(m) {
        }

        Matrix(const Matrix& m) : m_mat(m.m_mat) {
        }

        Matrix(const Quaternion& q) { m_mat = matrix(q); }

        // identity
        void Identity() { this->m_mat = DirectX::XMMatrixIdentity(); }

        ///@{
        /// Multiply
        void Multiply(const matrix& m) { this->m_mat = XMMatrixMultiply(m, this->m_mat); }

        void Multiply(const Matrix& m) { this->m_mat = XMMatrixMultiply(m.m_mat, this->m_mat); }
        ///@}

        ///@{
        ///translation
        void Translate(const float3& v) { this->m_mat = XMMatrixMultiply(this->m_mat, DirectX::XMMatrixTranslation(v.x, v.y, v.z)); }

        void Translate(float x, float y, float z) { this->m_mat = XMMatrixMultiply(this->m_mat, DirectX::XMMatrixTranslation(x, y, z)); }
        ///@}

        /// banana for
        void Scale(const float3 v) { this->m_mat = XMMatrixMultiply(this->m_mat, DirectX::XMMatrixScaling(v.x, v.y, v.z)); }

        void Scale(float s) { Scale(s, s, s); }

        void Scale(float x, float y, float z) { this->m_mat = XMMatrixMultiply(this->m_mat, DirectX::XMMatrixScaling(x, y, z)); }

        /** Rotate around a given axis
            @param v given axis
            @param phi angle, rad */
        void Rotate(const float3 v, float phi)
        {
            const dxvector vv = XMLoadFloat3(&v);
            this->m_mat = XMMatrixMultiply(this->m_mat, DirectX::XMMatrixRotationNormal(vv, phi));
        }

        void Rotate(const Quaternion& q) { Multiply(static_cast<matrix>(q)); }

        /// Roll-Pitch-Yaw rotation, in rad
        void RotateRPY(float roll, float pitch, float yaw) { this->m_mat = XMMatrixMultiply(this->m_mat, DirectX::XMMatrixRotationRollPitchYaw(roll, pitch, yaw)); }

        void RotateRPY(float3 rpy) { this->RotateRPY(rpy.x, rpy.y, rpy.z); }

        // --- lookat
        void LookAtLH(const float3& e, const float3& c = float3(0, 0, 0), const float3& u = float3(0, 1, 0))
        {
            const dxvector ev = XMLoadFloat3(&e);
            const dxvector cv = XMLoadFloat3(&c);
            const dxvector uv = XMLoadFloat3(&u);
            m_mat = DirectX::XMMatrixLookAtLH(ev, cv, uv);
        }

        void LookAtRH(const float3& e, const float3& c = float3(), const float3& u = float3(0, 1, 0))
        {
            const dxvector ev = XMLoadFloat3(&e);
            const dxvector cv = XMLoadFloat3(&c);
            const dxvector uv = XMLoadFloat3(&u);
            m_mat = DirectX::XMMatrixLookAtRH(ev, cv, uv);
        }

        /// Transpose matrix
        void Transpose() { m_mat = XMMatrixTranspose(m_mat); }

        /// Invert matrix
        void Invert() { m_mat = XMMatrixInverse(nullptr, m_mat); }

        // ===
        // --- operations

        void Set(const Matrix& m) { m_mat = m.m_mat; }
        void Set(const matrix& m) { m_mat = m; }

        void Set(std::array<float4, 4> const &v)
        {
            m_mat.r[0] = XMLoadFloat4(&v[0]);
            m_mat.r[1] = XMLoadFloat4(&v[1]);
            m_mat.r[2] = XMLoadFloat4(&v[2]);
            m_mat.r[3] = XMLoadFloat4(&v[3]);
        }

        /// @note operator= valamiert nem mukodik, kiveve, ha ... 
        Matrix& operator=(const Matrix& m)
        {
            m_mat = m.m_mat;
            return *this;
        }

        Matrix& operator=(const matrix& m)
        {
            m_mat = m;
            return *this;
        }

        friend bool operator==(const Matrix& lhs, const Matrix& rhs)
        {
            std::array<float4, 4> lv, rv;
            lhs.Get(lv);
            rhs.Get(rv);
            return rv == lv;
        }

        friend bool operator!=(const Matrix& lhs, const Matrix& rhs) { return !(lhs == rhs); }

        // --- getters
        const matrix& Get() const { return m_mat; }

        void Get(std::array<float4, 4>& v) const
        {
            XMStoreFloat4(&v[0], m_mat.r[0]);
            XMStoreFloat4(&v[1], m_mat.r[1]);
            XMStoreFloat4(&v[2], m_mat.r[2]);
            XMStoreFloat4(&v[3], m_mat.r[3]);
        }

        float Get(size_t row, size_t col) const
        {
#ifdef _XM_NO_INTRINSICS_
            return m_mat.r[row][col];
#else
            return DirectX::XMVectorGetByIndex(m_mat.r[row], col);
#endif
        }

        float4 Get(size_t row) const
        {
#ifdef _XM_NO_INTRINSICS_
            return m_mat.r[row];
#else
            float4 res;
            XMStoreFloat4(&res, m_mat.r[row]);
            return res;
#endif
        }

        bool Decompose(float3& outTranslation, Quaternion& outOrientation, float3& outScale) const
        {
            dxvector translation, orientation, scale;
            bool res = XMMatrixDecompose(&scale, &orientation, &translation, m_mat);
            XMStoreFloat3(&outTranslation, translation);
            outOrientation = Quaternion(orientation);
            XMStoreFloat3(&outScale, scale);
            return res;
        }

        // --- matrix - vector ops
        float4 Transfrom(float4& f4) const
        {
            dxvector v = XMVector4Transform(XMLoadFloat4(&f4), m_mat);
            float4 r;
            XMStoreFloat4(&r, v);
            return r;
        }

        float3 Transfrom(float3& f3) const
        {
            dxvector v = XMVector3Transform(XMLoadFloat3(&f3), m_mat);
            float3 r;
            XMStoreFloat3(&r, v);
            return r;
        }

        template <class AR>
        void Serialize(AR& ar)
        {
            std::array<float4, 4> v;
            Get(v);
            ar&v;
            Set(v);
        }

    private:
        matrix m_mat;
    };
}
