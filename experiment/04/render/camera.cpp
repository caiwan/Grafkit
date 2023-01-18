////////////////////////////////////////////////////////////////////////////////
// Filename: Camera.cpp
////////////////////////////////////////////////////////////////////////////////
#include "camera.h"

using namespace FWrender;

Camera::Camera()
{
	/*
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
	*/
	m_position = float3( 0, 0, 0 );
	m_rotation = float3(0, 0, 0);
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

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// Finally create the view matrix from the three updated vectors.
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	return;
}


void Camera::GetViewMatrix(matrix& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}