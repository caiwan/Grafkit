#include "../stdafx.h"

#include "camera.h"

#define _USE_MATH_DEFINES
#include <math.h>

using namespace Grafkit;

Camera::Camera() : Entity3D()
{
	m_mode = LOOK_TO;
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


void Grafkit::Camera::SetLookAt(float x, float y, float z)
{
	m_mode = LOOK_AT;
	m_look.x=x;
	m_look.y=y;
	m_look.z=z;
}

void Grafkit::Camera::SetLookTo(float x, float y, float z)
{
	m_mode = LOOK_TO;
	m_look.x = x;
	m_look.y = y;
	m_look.z = z;
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

	// itt kilukadta akamera, fixelni kell egy kicsit
	
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

	XMVECTOR v0, v1, v2;
	XMVECTOR s, u, f;
	XMVECTOR E = XMLoadFloat3(&pos);
	XMVECTOR C = XMLoadFloat3(&center);
	XMVECTOR U = XMLoadFloat3(&up);

	// look at a reference point 
	if (m_mode == LOOK_AT) {
		//f = XMVectorSubtract(C, E);
		f = XMVectorSubtract(E, C);
		f = XMVector3Normalize(f);	// lookat
	}
	// look towards a drirection
	else if(m_mode == LOOK_TO) {
		f = C;	// lookat = center 
	}

#if 0
	m_viewMatrix = XMMatrixLookToLH(E, f, U);
#else 

	// kezzel fel kell irni a kamera matrixot, mert a D3D-s feliras valamiert nem okes. 
	// https://github.com/g-truc/glm/blob/316460408a5ef0338029a7c8f387ac49a2f1561c/glm/gtc/matrix_transform.inl#L638

	s = XMVector3Cross(U, f);	// right

	u = XMVector3Cross(f, s);	// up

	v0 = XMVector3Dot(s, E);
	v1 = XMVector3Dot(u, E);
	v2 = XMVector3Dot(f, E);

	float3 sv, uv, fv;
	float vv0, vv1, vv2;

	XMStoreFloat3(&sv, s);
	XMStoreFloat3(&uv, u);
	XMStoreFloat3(&fv, f);
#if 0
	XMStoreFloat(&vv0, v0);
	XMStoreFloat(&vv1, v1);
	XMStoreFloat(&vv2, v2);
#else
	XMVectorGetXPtr(&vv0, E);
	XMVectorGetYPtr(&vv1, E);
	XMVectorGetZPtr(&vv2, E);
#endif

	m_viewMatrix = Matrix(
		sv.x, sv.y, sv.z, 0,
		uv.x, uv.y, uv.z, 0,
		fv.x, fv.y, fv.z, 0,
		-vv0, -vv1, -vv2, 1
	);

	m_viewMatrix.RotateRPY(m_rotation.x, m_rotation.y, m_rotation.z);

#endif // 0

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
	m_perspectiveMatrix = XMMatrixPerspectiveFovLH(m_fov, m_aspect, m_znear, m_zfar);
	m_orthoMatrix = XMMatrixOrthographicLH(m_screenWidth, m_screenHeight, m_znear, m_zfar);	


	}
