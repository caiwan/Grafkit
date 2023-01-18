#pragma once

#include "Actor.h"

#include "../stdafx.h"
#include "../math/matrix.h"

#include "renderer.h"

namespace Grafkit {

	class Camera;
	// 

	/** Basic camera class */
	__declspec(align(16)) 
		class Camera : public Entity3D, public AlignedNew<Camera>
	{
	public:
		enum camera_mode { CAMERA_LH, CAMERA_RH };

		Camera(camera_mode mode = CAMERA_LH);
		~Camera();

		void SetAspect(float aspect) { this->m_aspect = aspect; }
		/// @todo GetAspect()
		
		/// Horizontal FOV in radian
		void SetFOV(float fov) { this->m_hFov = fov; }
		float GetFOV() { return this->m_hFov; }
		
		void SetClippingPlanes(float znear, float zfar) { this->m_znear = znear, this->m_zfar = zfar; }
		/// @todo void GetClippingPlanes(float &znear, float &zfar);

		void SetScreenMetrics(float width, float height) { this->m_screenWidth = width, this->m_screenHeight = height; }
		/// @todo void GetScreenMetrics(float width, float height) { this->m_screenWidth = width, this->m_screenHeight = height; }

		/** Generate matrices */
		void Calculate(Renderer& renderer);

		//skip render, nothing to do with it. 
		virtual void Render(Grafkit::Renderer& deviceContext, Scene* const & scene) {}
		virtual void Build(Grafkit::Renderer& deviceContext, Scene* const & scene){}

		Matrix& GetViewMatrix() { return m_viewMatrix; }
		Matrix& GetPerspectiveMatrix() { return m_perspectiveMatrix; }
		Matrix& GetOrthoMatrix() { return m_orthoMatrix; }

		virtual Matrix& ProjectionMatrix() {
			switch(m_type){
			case ORTHOGRAPHIC:
				return m_orthoMatrix;
			//case PERSPECTIVE:
			default:
				return m_perspectiveMatrix;
			}
		}

		

		enum camera_mode getMode() {
			return m_mode;
		}
		
		enum camera_type { PERSPECTIVE, ORTHOGRAPHIC };
		
		virtual void setType(enum camera_type t) {
			m_type = t;
		}

		virtual enum camera_type getType() {
			return m_type;
		}

	private:
		float m_znear, m_zfar;
		float m_aspect, m_screenWidth, m_screenHeight;
		float m_hFov;

		enum camera_type m_type; // hack: van, ahol kezzel kell a kamerat atvaltnai perpective/ortho kozott
		enum camera_mode m_mode;
	
	protected:
		Grafkit::Matrix m_viewMatrix;
		Grafkit::Matrix m_perspectiveMatrix;
		Grafkit::Matrix m_orthoMatrix;

		PERSISTENT_DECL(Grafkit::Camera, 1);
	protected:
		virtual void serialize(Archive& ar);
	};
}
