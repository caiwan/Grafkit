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
	m_look = float3(0, 0, 1);
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
	XMFLOAT3 up, lookAt;
	XMVECTOR upVector, positionVector, lookVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	upVector = XMLoadFloat3(&m_up);

	positionVector = XMLoadFloat3(&m_position);

	lookVector = XMLoadFloat3(&m_look);

	pitch = m_rotation.x;
	yaw   = m_rotation.y;
	roll  = m_rotation.z;

	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	lookVector = XMVector3TransformCoord(lookVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	lookVector = XMVectorAdd(positionVector, lookVector);
	lookVector = XMVector3Normalize(lookVector);

	renderer.GetScreenSizef(m_screenWidth, m_screenHeight);
	this->m_aspect = m_screenWidth / m_screenHeight;

	m_viewMatrix = XMMatrixLookToLH(positionVector, lookVector, upVector);

	// --- projection & ortho --- 
	m_projectionMatrix = XMMatrixPerspectiveFovLH(m_fov, m_aspect, m_znear, m_zfar);
	m_orthoMatrix = XMMatrixOrthographicLH(m_screenWidth, m_screenHeight, m_znear, m_zfar);	


	}
