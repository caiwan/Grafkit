
#ifndef _Camera_H_
#define _Camera_H_

#include "dxtypes.h"
#include "../core/reference.h"
#include "Actor.h"

using namespace DirectX;

namespace FWrender {

	class PerspectiveCamera;	///@todo implement this, see trello notes
	class OrthoCamera;			///@todo implement this, see trello notes

	class Camera : public Entity3D, virtual public Referencable
	{
	public:
		Camera();
		Camera(const Camera&);
		~Camera();

		void* operator new(size_t);
		void  operator delete(void*);

		void SetPosition(float x, float y, float z);
		float3 GetPosition() { return this->m_position; }
		
		void SetRotation(float x, float y, float z);
		float3 GetRotation() { return this->m_rotation; }

		void SetAspect(float aspect) { this->m_aspect = aspect; }
		/// @todo GetAspect()
		
		void SetFOV(float fov) { this->m_fov = fov; }
		/// @todo GetFOV()
		
		void SetClippingPlanes(float znear, float zfar) { this->m_znear = znear, this->m_zfar = zfar; }
		/// @todo void GetClippingPlanes(float &znear, float &zfar);

		void SetScreenMetrics(float width, float height) { this->m_screenWidth = width, this->m_screenHeight = height; }
		/// @todo void GetScreenMetrics(float width, float height) { this->m_screenWidth = width, this->m_screenHeight = height; }

		/** Generate matrices */
		void Render();

		void GetViewMatrix(matrix & matrix);
		void GetProjectionMatrix(matrix& matrix);
		void GetOrthoMatrix(matrix& matrix);

	private:
		float3 m_position;
		float3 m_rotation;

		float m_znear, m_zfar;
		float m_aspect, m_screenWidth, m_screenHeight;
		float m_fov;

		matrix m_viewMatrix;
		matrix m_projectionMatrix;
		matrix m_orthoMatrix;
	};
}

#endif