#include "../stdafx.h"

#include "camera.h"

#define _USE_MATH_DEFINES
#include <math.h>

using namespace Grafkit;

Camera::Camera() : Entity3D()
{
	m_fov = M_PI / 4.0f;

	m_znear = 1.0;
	m_zfar = 1000.;

	m_aspect = 4. / 3.;
	m_screenHeight = 100;
	m_screenWidth = 100;

	m_position = float3(0, 0, 0);
	m_look = float3(0, 0, 0);
	m_up = float3(0, 1, 0);
	m_rotation = float3(0, 0, 0);
}


Camera::~Camera()
{
}


void Camera::SetPosition(float x, float y, float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
}


void Grafkit::Camera::SetLook(float x, float y, float z)
{
	m_look.x=x;
	m_look.y=y;
	m_look.z=z;
}

void Grafkit::Camera::SetUp(float x, float y, float z)
{
	m_up.x = x;
	m_up.y = y;
	m_up.z = z;
}


void Camera::SetRotation(float x, float y, float z)
{
	m_rotation.x = x;
	m_rotation.y = y;
	m_rotation.z = z;
}


// --- pinakobold
void Camera::Calculate(Renderer& renderer)
{

	// lofasz a seggedbe kis geci 
	// http://www.gamedev.net/page/resources/_/technical/directx-and-xna/directx-11-c-game-camera-r2978

#if 0
	float3 pos = float3(-5, 6, 15);
	float3 center = float3(0, 0, 0);
	float3 up = float3(0,1,0);
#else
	float3 pos = m_position;
	float3 center = m_look;
	float3 up = m_up;
#endif

	XMVECTOR v_pos = XMLoadFloat3(&pos);
	XMVECTOR v_look, v_center = XMLoadFloat3(&center);
	XMVECTOR v_up = XMLoadFloat3(&up);

	v_look = XMVectorSubtract(v_pos, v_center); 
	v_look = XMVector3Normalize(v_look);
	
	m_viewMatrix = XMMatrixLookToLH(v_pos, v_look, v_up);

#if 0
	XMFLOAT3 up, lookAt;
	XMVECTOR upVector, positionVector, lookVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix, cica;

	upVector = XMLoadFloat3(&m_up);

	positionVector = XMLoadFloat3(&m_position);
	positionVector = XMVectorSetW(positionVector, 1.f);

	lookVector = XMLoadFloat3(&m_look);
	lookVector = XMVectorSetW(lookVector, 1.f);

	pitch = m_rotation.x;
	yaw = m_rotation.y;
	roll = m_rotation.z;

	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// lookVector = XMVector3TransformCoord(lookVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);
	upVector = XMVectorSetW(upVector, 0.f);

	// lookVector = XMVectorAdd(positionVector, lookVector);
	// lookVector = XMVector3Normalize(lookVector);
	// lookVector = XMVectorSetW(lookVector, 1.f);

// http://stackoverflow.com/questions/19484601/3d-first-person-camera-strafing-at-angle

	
	//m_viewMatrix = XMMatrixLookToLH(positionVector, lookVector, upVector);
	m_viewMatrix = cica = XMMatrixLookAtLH(positionVector, lookVector, upVector);

#endif // 0

	renderer.GetScreenSizef(m_screenWidth, m_screenHeight);
	this->m_aspect = m_screenWidth / m_screenHeight;

	// --- projection & ortho --- 
	m_projectionMatrix = XMMatrixPerspectiveFovLH(m_fov, m_aspect, m_znear, m_zfar);
	m_orthoMatrix = XMMatrixOrthographicLH(m_screenWidth, m_screenHeight, m_znear, m_zfar);	


	}
