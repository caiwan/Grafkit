#include "stdafx.h"

#include "animation.h"

#include "camera.h"

using namespace Grafkit;

PERSISTENT_IMPL(ActorAnimation, 1);

/* ============================================================================================== */

void Grafkit::Animation::_serialize(Archive & ar)
{
	PERSIST_STRING(ar, m_name);
}

/* ============================================================================================== */

/// quick and dirty hack 
#define LERP(v0, v1, a)\
	((v0)*(1-(a)) + (v1)*((a)))

#define LERPv3(v0, v1, a)\
	float3(\
	LERP(v0.x, v1.x, a),\
	LERP(v0.y, v1.y, a),\
	LERP(v0.z, v1.z, a))

void Grafkit::Animation::FindKey(FloatTrack track, double &time, float &value)
{
	float t = (float)time, f = 0.f;
	float v0 = 0., v1 = 0.;
	if (track.FindKey(t, v0, v1, f))
		value = LERP(v0, v1, f);
}

void Grafkit::Animation::FindKey(Vector3Track track, double &time, float3 &value)
{
	float t = (float)time, f = 0.f;
	float3 v0(0, 0, 0), v1(0, 0, 0);
	if (track.FindKey(t, v0, v1, f))
		value = LERPv3(v0, v1, f);
}

void Grafkit::Animation::FindKey(Vector4Track track, double &time, Quaternion &value)
{
	float t = (float)time, f = 0.f;
	float4 v0, v1;
	if (track.FindKey(t, v0, v1, f)) {
		Quaternion q0(v0), q1(v1);
		value.Slerp(q0, q1, f);
	}
}

/* ============================================================================================== */

void Grafkit::ActorAnimation::Update(double time)
{
	float3 pos(0, 0, 0), scale(1, 1, 1);
	Quaternion rotate;
	Matrix matrix;
	
	Animation::FindKey(m_positionTrack, time, pos);
	Animation::FindKey(m_scalingTrack, time, scale);
	Animation::FindKey(m_rotationTrack, time, rotate);

	matrix.Identity();
	matrix.Scale(scale);
	matrix.Rotate(rotate);
	matrix.Translate(pos);

	// note:
	// https://gamedev.stackexchange.com/questions/16719/what-is-the-correct-order-to-multiply-scale-rotation-and-translation-matrices-f
	
	m_actor->Matrix(matrix);
}

void Grafkit::ActorAnimation::serialize(Archive & ar)
{
	this->Animation::_serialize(ar);

	m_positionTrack.serialize(ar);
	m_scalingTrack .serialize(ar);
	m_rotationTrack.serialize(ar);
}

/* ============================================================================================== */

void Grafkit::CameraAnimation::Update(double t)
{
	Camera* cam = dynamic_cast<Camera*>(m_entity.Get());
	if (cam) {
		float fov = 0; FindKey(m_fov, t, fov); cam->SetFOV(fov);
		// focal shit??
	}
}
