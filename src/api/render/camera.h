#pragma once

#include "Actor.h"

#include "../stdafx.h"
#include "../math/matrix.h"

#include "renderer.h"

namespace Grafkit {

	class Camera;
	// class PerspectiveCamera;		///@todo implement this, see trello notes
	// class OrthoCamera;			///@todo implement this, see trello notes
	// 

	__declspec(align(16)) class Camera : public AlignedNew<Camera>, public Grafkit::Entity3D
	{
	public:
		Camera();
		~Camera();

	/*	void* operator new(size_t);
		void  operator delete(void*);
*/
		void SetPosition(float x, float y, float z);
		float3 &GetPosition() { return this->m_position; }
		
		void SetLookAt(float x, float y, float z);
		float3 &GetLookAt() { return this->m_position; }

		void SetUp(float x, float y, float z);
		float3 &GetUp() { return this->m_up; }

		void SetRotation(float x, float y, float z);
		float3 &GetRotation() { return this->m_rotation; }

		void SetAspect(float aspect) { this->m_aspect = aspect; }
		/// @todo GetAspect()
		
		void SetFOV(float fov) { this->m_fov = fov; }
		/// @todo GetFOV()
		
		void SetClippingPlanes(float znear, float zfar) { this->m_znear = znear, this->m_zfar = zfar; }
		/// @todo void GetClippingPlanes(float &znear, float &zfar);

		void SetScreenMetrics(float width, float height) { this->m_screenWidth = width, this->m_screenHeight = height; }
		/// @todo void GetScreenMetrics(float width, float height) { this->m_screenWidth = width, this->m_screenHeight = height; }

		/** Generate matrices */
		void Calculate(Renderer& renderer);

		//skip render, nothing to do with it. 
		virtual void Render(Grafkit::Renderer& deviceContext, Scene* scene) {}

		///@todo ezzel kell kezdeni valamit a jovoben:
		Matrix& GetViewMatrix() { return m_viewMatrix; }
		Matrix& GetProjectionMatrix() { return m_projectionMatrix; }
		Matrix& GetOrthoMatrix() { return m_orthoMatrix; }

		//virtual const char* GetBucketID() { return CAMERA_BUCKET; }

	private:
		float3 m_position;
		float3 m_lookAt;
		float3 m_up;
		float3 m_rotation;

		float m_znear, m_zfar;
		float m_aspect, m_screenWidth, m_screenHeight;
		float m_fov;

		Grafkit::Matrix m_viewMatrix;
		Grafkit::Matrix m_projectionMatrix;
		Grafkit::Matrix m_orthoMatrix;
	};
}
