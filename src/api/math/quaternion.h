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

		// --- setterek
		void Set(float x, float y, float z, float w) {
			DirectX::XMVectorSet(x, y, z, w);
		}

		// --- muveletek
		void Slerp(const Quaternion &q0, const Quaternion &q1, float t) {
			q = DirectX::XMQuaternionSlerp(q0.q, q1.q, t);
		}

		// --- konverziok

		operator matrix() const {
			return DirectX::XMMatrixRotationQuaternion(q);
		}

		operator float4() const {
			float4 res;
			DirectX::XMStoreFloat4(&res, q);
			return res;
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

		static Quaternion fromEuler(float r, float p, float y) {
			return Quaternion(DirectX::XMQuaternionRotationRollPitchYaw(r, p, y));
		}
	};
}
