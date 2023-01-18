#include "../math/matrix.h"
#include "camera.h"

#define _USE_MATH_DEFINES
#include <math.h>

using namespace FWrender;

Camera::Camera()
{
	m_fov = M_PI / 4.0f;

	m_aspect = 1;
	m_screenHeight = 100;
	m_screenHeight = 100;

	m_position = float3( 0, 0, 0 );
	m_rotation = float3( 0, 0, 0 );
}


Camera::Camera(const Camera& other)
{
}


Camera::~Camera()
{
}


void* Camera::operator new(size_t memorySize)
{
	size_t alignment;
	void* memoryBlockPtr;


	// Set the memory alignment to 16 byte to support high speed XMMATRIX calculations and prevent crashes.
	alignment = 16;

	// Allocate the memory.
	memoryBlockPtr = _aligned_malloc(memorySize, alignment);

	return memoryBlockPtr;
}


void Camera::operator delete(void* memoryBlockPtr)
{
	// Free the class memory that had been allocated using the _aligned_malloc function.
	_aligned_free(memoryBlockPtr);

	return;
}


void Camera::SetPosition(float x, float y, float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
	
	return;
}


void Camera::SetRotation(float x, float y, float z)
{
	m_rotation.x = x;
	m_rotation.y = y;
	m_rotation.z = z;

	return;
}



// --- pinakobold
void Camera::Render()
{
	XMFLOAT3 up, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;


	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Load it into a XMVECTOR structure.
	upVector = XMLoadFloat3(&up);

	// Setup the position of the camera in the world.


	// Load it into a XMVECTOR structure.
	positionVector = XMLoadFloat3(&m_position);

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Load it into a XMVECTOR structure.
	lookAtVector = XMLoadFloat3(&lookAt);

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = m_rotation.x * 0.0174532925f;
	yaw   = m_rotation.y * 0.0174532925f;
	roll  = m_rotation.z * 0.0174532925f;

	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	// --- projection --- 

	m_projectionMatrix = XMMatrixPerspectiveFovLH(m_fov, m_aspect, m_znear, m_zfar);

	// Create an orthographic projection matrix for 2D rendering.
	m_orthoMatrix = XMMatrixOrthographicLH(m_screenWidth, m_screenHeight, m_znear, m_zfar);

	return;
}


void Camera::GetViewMatrix(FWmath::Matrix& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}

void FWrender::Camera::GetProjectionMatrix(FWmath::Matrix & matrix)
{
	matrix = m_projectionMatrix;
}

void FWrender::Camera::GetOrthoMatrix(FWmath::Matrix & matrix)
{
	matrix = m_orthoMatrix;
}
