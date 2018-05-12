#include "actoranimation.h"

using namespace Grafkit;

#include "animation.h"


Grafkit::ActorAnimation::ActorAnimation() : Animation()
{
	CreateTrack();
}

Grafkit::ActorAnimation::ActorAnimation(ActorRef actor) : Animation(), m_actor(actor)
{
	CreateTrack();
}

void Grafkit::ActorAnimation::Update(double time)
{
	const float3 t = m_position->GetFloat3(time);
	const float3 r = m_rotation->GetFloat3(time);
	const float3 s = m_scale->GetFloat3(time);
	
	m_actor->Transform().Identity();
	m_actor->Transform().RotateRPY(r);
	m_actor->Transform().Scale(s);
	m_actor->Transform().Translate(t);
}

void Grafkit::ActorAnimation::CreateTrack()
{
	m_position = new Animation::Track("Position", "xyz");
	AddTrack(m_position);

	m_rotation = new Animation::Track("Rotation", "rpy");
	AddTrack(m_rotation);

	m_scale = new Animation::Track("Scaling", "xyz");
	AddTrack(m_scale);
	m_scale->SetFloat3(0, float3(1, 1, 1));
}
