#include "stdafx.h"

#include "camera.h"

#define _USE_MATH_DEFINES
#include <math.h>

using namespace Grafkit;

//#define CAMERA_RH

PERSISTENT_IMPL(Grafkit::Camera)


Grafkit::Camera::Camera(camera_mode mode) :Entity3D()
{
	m_mode = mode;
	m_hFov = M_PI / 4.0f;

	m_znear = 1.0;
	m_zfar = 1000.;

	m_aspect = 4. / 3.;
	m_screenHeight = 100;
	m_screenWidth = 100;
}

Camera::~Camera()
{
}

void Camera::Calculate(Renderer& renderer)
{

	// http://www.gamedev.net/page/resources/_/technical/directx-and-xna/directx-11-c-game-camera-r2978

	float3 position = float3(0, 0, 0);
	float3 look = float3(0, 0, 1);			// LH coord
	float3 up = float3(0, 1, 0);

	dxvector E = XMLoadFloat3(&position);	// eye coord
	dxvector C = XMLoadFloat3(&look);		// look reference or vector
	dxvector U = XMLoadFloat3(&up);			// up vector

	if (m_mode == CAMERA_RH)
		m_viewMatrix = XMMatrixLookToRH(E, C, U);
	else 
		m_viewMatrix = XMMatrixLookToLH(E, C, U);

	renderer.GetViewportSizef(m_screenWidth, m_screenHeight);
	this->m_aspect = m_screenWidth / m_screenHeight;

	// --- projection & ortho --- 

	float fov = M_PI / 4;
	fov = 2 * atanf(tanf(m_hFov / 2) / m_aspect);

	m_perspectiveMatrix = XMMatrixPerspectiveFovLH(fov, m_aspect, m_znear, m_zfar);
	m_orthoMatrix = XMMatrixOrthographicLH(m_screenWidth, m_screenHeight, m_znear, m_zfar);	

}


void Grafkit::Camera::serialize(Archive & ar)
{
	this->Entity3D::_serialize(ar);

	PERSIST_FIELD(ar, m_type);
	PERSIST_FIELD(ar, m_mode);

	PERSIST_FIELD(ar, m_znear);
	PERSIST_FIELD(ar, m_zfar);
	PERSIST_FIELD(ar, m_hFov);
}