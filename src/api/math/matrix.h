#include "../render/dxtypes.h"

namespace FWmath {
	/**
	Extends DirectX::XMMATRIX with operations to make thing easier, and use as a real type.
	https://msdn.microsoft.com/en-us/library/windows/desktop/ee415594(v=vs.85).aspx

	`::matrix` is a shortcut to `DirectX::XMMATRIX`
	*/
	class Matrix
	{

	private:
		matrix mat;

	public:
		// --- constructors
		Matrix() {
			this->Identity();
		}


		// identity
		void Identity() {
			 (*this) = DirectX::XMMatrixIdentity();
		}

		///translation
		void Translate(float3 v)
		{
			this->mat = DirectX::XMMatrixTranslation(v.x, v.y, v.z);
			
		}

		///scale
		void Scale(float3 v)
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

		// --- getters

	};
}
