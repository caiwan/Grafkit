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
	m_lookAt = float3(0, 0, 1);
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


void Grafkit::Camera::SetLookAt(float x, float y, float z)
{
	m_lookAt.x=x;
	m_lookAt.y=y;
	m_lookAt.z=z;
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
	XMFLOAT3 up, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	upVector = XMLoadFloat3(&m_up);

	positionVector = XMLoadFloat3(&m_position);

	lookAtVector = XMLoadFloat3(&m_lookAt);

	pitch = m_rotation.x * 0.0174532925f;
	yaw   = m_rotation.y * 0.0174532925f;
	roll  = m_rotation.z * 0.0174532925f;

	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	lookAtVector = XMVectorAdd(positionVector, lookAtVector);
	lookAtVector = XMVector3Normalize(lookAtVector);

	renderer.GetScreenSizef(m_screenWidth, m_screenHeight);
	this->m_aspect = m_screenWidth / m_screenHeight;

#if 1

	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	// --- projection & ortho --- 
	m_projectionMatrix = XMMatrixPerspectiveFovLH(m_fov, m_aspect, m_znear, m_zfar);
	m_orthoMatrix = XMMatrixOrthographicLH(m_screenWidth, m_screenHeight, m_znear, m_zfar);	

#else

	m_viewMatrix = XMMatrixLookAtRH(positionVector, lookAtVector, upVector);

	// --- projection & ortho --- 
	m_projectionMatrix = XMMatrixPerspectiveFovRH(m_fov, m_aspect, m_znear, m_zfar);
	m_orthoMatrix = XMMatrixOrthographicRH(m_screenWidth, m_screenHeight, m_znear, m_zfar);

#endif
	}


//void Camera::GetViewMatrix(matrix& viewMatrix)
//{
//	viewMatrix = m_viewMatrix;
//}
//
//
//void Grafkit::Camera::GetProjectionMatrix(matrix & matrix)
//{
//	matrix = m_projectionMatrix;
//}
//
//
//void Grafkit::Camera::GetOrthoMatrix(matrix & matrix)
//{
//	matrix = m_orthoMatrix;
//}

