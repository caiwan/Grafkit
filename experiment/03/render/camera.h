////////////////////////////////////////////////////////////////////////////////
// Filename: Camera.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _Camera_H_
#define _Camera_H_

#include "dxtypes.h"
using namespace DirectX;

namespace FWrender {
	class Camera
	{
	public:
		Camera();
		Camera(const Camera&);
		~Camera();

		void* operator new(unsigned int);
		void  operator delete(void*);

		void SetPosition(float x, float y, float z);
		void SetRotation(float x, float y, float z);

		float3 GetPosition() { return this->m_position; }
		float3 GetRotation() { return this->m_rotation; }

		void Render();
		void GetViewMatrix(matrix& matrix);

	private:
		float3 m_position;
		float3 m_rotation;
		matrix m_viewMatrix;
	};
}

#endif