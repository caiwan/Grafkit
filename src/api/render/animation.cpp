#include "animation.h"


void Grafkit::NodeAnimation::Initialize(ActorRef actor)
{
	m_actor = actor;
}

void Grafkit::NodeAnimation::Update(double time)
{
	float3 pos(0, 0, 0), scale(1, 1, 1);
	Quaternion rotate;
	Matrix matrix;
	
	findKey(m_positionTrack, time, pos);
	findKey(m_scalingTrack, time, scale);
	findKey(m_rotationTrack, time, rotate);
	
	matrix.Identity();
	matrix.Scale(scale);
	matrix.Rotate(rotate);
	matrix.Translate(pos);
	
	m_actor->Matrix(matrix);
}

/// quick and dirty hack 
#define LERP(v0, v1, a)\
	((v0)*(1-(a)) + (v1)*((a)))

#define LERPv3(v0, v1, a)\
	float3(\
	LERP(v0.x, v1.x, a),\
	LERP(v0.y, v1.y, a),\
	LERP(v0.z, v1.z, a))

void Grafkit::NodeAnimation::findKey(Vector3Track track, double &time, float3 &value)
{
	float t = (float)time, f = 0.f;
	float3 v0(0, 0, 0), v1(0, 0, 0);
	if (track.findKey(t, v0, v1, f))
		value = LERPv3(v0, v1, f);
}

void Grafkit::NodeAnimation::findKey(QuaternionTrack track, double &time, Quaternion &value)
{
	float t = (float)time, f = 0.f;
	Quaternion q0, q1;
	if (track.findKey(t, q0, q1, f))
		value.Slerp(q0, q1, f);
}
