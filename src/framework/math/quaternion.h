#pragma once 

#include "../render/dxtypes.h"
#include "matrix.h"

namespace Grafkit {
	class Quaternion {
	private:
		dxvector q;
	public:
		// ctor + copy ctor 
		Quaternion(){
			DirectX::XMVectorSet(0, 0, 0, 0);
		}

		Quaternion(float x, float y, float z, float w) {
			DirectX::XMVectorSet(x, y, z, w);
		}

		Quaternion(const dxvector &in) : q(in) {
		}

		Quaternion(const Quaternion &in) : q(in.q){
		}

		Quaternion(const float4 &in) {
			q = DirectX::XMLoadFloat4(&in);
		}

		// --- setters
		void Set(float x, float y, float z, float w) {
			DirectX::XMVectorSet(x, y, z, w);
		}

		// --- operations
		void Slerp(const Quaternion &q0, const Quaternion &q1, float t) {
			q = DirectX::XMQuaternionSlerp(q0.q, q1.q, t);
		}

		Quaternion& operator*(const Quaternion& in) {
			q = DirectX::XMQuaternionMultiply(q, in.q);
			return *this;
		}

		// --- operator = 
		Quaternion& operator=(const Quaternion& in) {
			q = in.q;
			return *this;
		}

		Quaternion& operator=(const dxvector& in) {
			q = in;
			return *this;
		}

		Quaternion& operator=(const float4& in) {
			q = DirectX::XMLoadFloat4(&in);
			return *this;
		}

		// --- conversion

		operator matrix() const {
			return DirectX::XMMatrixRotationQuaternion(q);
		}

		static Quaternion fromEuler(float r, float p, float y) {
			return Quaternion(DirectX::XMQuaternionRotationRollPitchYaw(r, p, y));
		}

		static Quaternion fromEuler(float3 rpy) {
			return Quaternion(DirectX::XMQuaternionRotationRollPitchYaw(rpy.x, rpy.y, rpy.z));
		}

		float4 ToAxisAngle() {
			dxvector axis;
			float angle = 0;
			DirectX::XMQuaternionToAxisAngle(&axis, &angle, q);
			float4 res;
			DirectX::XMStoreFloat4(&res, axis);
			res.w = angle;
			return res;
		}

		operator float4() const {
			float4 res;
			DirectX::XMStoreFloat4(&res, q);
			return res;
		}
	};
}
