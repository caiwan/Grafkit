#pragma once
#include "../render/dxtypes.h"
#include "memory_align.h"
namespace FWmath {
	/**
	Extends DirectX::XMMATRIX with operations to make thing easier, and use as a real type.
	https://msdn.microsoft.com/en-us/library/windows/desktop/ee415594(v=vs.85).aspx

	`::matrix` is a shortcut to `DirectX::XMMATRIX`
	*/

	__declspec(align(16)) class Matrix : public AlignedNew<Matrix>
//	class Matrix
	{

	private:
		matrix mat;

	public:
		// --- constructors
		Matrix() {
			this->Identity();
		}

		Matrix(
			float a1, float a2, float a3, float a4,
			float b1, float b2, float b3, float b4,
			float c1, float c2, float c3, float c4,
			float d1, float d2, float d3, float d4
		) {
			this->mat = DirectX::XMMatrixSet(
				a1, a2, a3, a4,
				b1, b2, b3, b4,
				c1, c2, c3, c4,
				d1, d2, d3, d4
			);
		}

		// identity
		void Identity() {
			 this->mat = DirectX::XMMatrixIdentity();
		}

		///translation
		void Translate(float3 &v)
		{
			this->mat = DirectX::XMMatrixTranslation(v.x, v.y, v.z);
			
		}

		///@{
		/// Multiply
		void Multiply(matrix &m)
		{
			this->mat = DirectX::XMMatrixMultiply(this->mat, m);
		}

		void Multiply(Matrix &m)
		{
			this->mat = DirectX::XMMatrixMultiply(m.mat, this->mat);
		}
		///@}

		/// banana for
		void Scale(float3 &v)
		{
			this->mat = DirectX::XMMatrixScaling(v.x, v.y, v.z);
		}

		/// Mixed scale and translate operation
		void ScaleAndTranslate(float3 s, float3 t)
		{
			///@todo TBD
		}

		/** Rotate around a given axis
			@param v given axis
			@param phi angle, rad */
		void Rotate(float3 v, float phi)
		{
			dxvector vv;
			DirectX::XMLoadFloat3(&v);
			this->mat = DirectX::XMMatrixRotationNormal(vv, phi);
		}

		/// @todo https://en.wikipedia.org/wiki/Euler_angles
		//void RotateEuler(float3 v) {
		//}

		/// Roll-Pitch-Yaw rotation
		void RotateRPY(float roll, float pitch, float yaw)
		{
			this->mat = DirectX::XMMatrixRotationRollPitchYaw(roll, pitch, yaw);
		}


		/// Do a complex transformation 
		void Transform() {
			/// @todo TBD
		}

		/// Create orthographic projection
		void Ortho(float vwidth, float vheigt, float znear, float zfar) 
		{
#ifdef USE_LEFT_HANDED_SYSTEM
			(*this) = DirectX::XMMatrixOrthographicLH(vwidth, vheigt, znear, zfar);
#else //USE_LEFT_HANDED_SYSTEM
			(*this) = DirectX::XMMatrixOrthographicRH(vwidth, vheigt, znear, zfar);
#endif //USE_LEFT_HANDED_SYSTEM
		}
		
		/**
			Crete perspective projection from screen metrics
		*/
		void PerspectiveXY(float vWidth, float vHeight, float zNear, float zFar) 
		{
#ifdef USE_LEFT_HANDED_SYSTEM
			(*this) = DirectX::XMMatrixPerspectiveLH(vWidth, vHeight, zNear, zFar);
#else //USE_LEFT_HANDED_SYSTEM
			(*this) = DirectX::XMMatrixPerspectiveRH(vWidth, vHeight, zNear, zFar);
#endif //USE_LEFT_HANDED_SYSTEM		
		}

		/**
		Crete perspective projection from screen metrics
		*/
		void Perspective(float aspect, float fov, float zNear, float zFar)
		{
#ifdef USE_LEFT_HANDED_SYSTEM
			(*this) = DirectX::XMMatrixPerspectiveFovLH(aspect, fov, zNear, zFar);
#else //USE_LEFT_HANDED_SYSTEM
			(*this) = DirectX::XMMatrixPerspectiveFovLH(aspect, fov, zNear, zFar);
#endif //USE_LEFT_HANDED_SYSTEM		
		}

		// --- lookat

		/// Transpose matrix
		void Transpose() {
			mat = XMMatrixTranspose(mat);
		}

		// ===
		// --- operations

		/// @note operator= valamiert nem mukodik, kiveve, ha ... 
		Matrix& operator= (const Matrix& m) 
		{
			mat = m.mat;
			return *this;
		}

		Matrix& operator= (const matrix& m)
		{
			mat = m;
			return *this;
		}

		matrix& Get() {
			return mat;
		}

		// --- getters

	};
}
