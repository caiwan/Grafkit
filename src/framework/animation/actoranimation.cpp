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
	// ... TBD
}

void Grafkit::ActorAnimation::CreateTrack()
{
	m_position = new Animation::Track();
	AddTrack(m_position);
	m_position->SetName("Position");
	m_position->CreateChannel("x");
	m_position->CreateChannel("y");
	m_position->CreateChannel("z");

	m_rotation = new Animation::Track();
	AddTrack(m_rotation);
	m_rotation->SetName("Rotation");
	m_rotation->CreateChannel("r");
	m_rotation->CreateChannel("p");
	m_rotation->CreateChannel("y");

	m_scale = new Animation::Track();
	AddTrack(m_scale);
	m_scale->SetName("Scale");
	m_scale->CreateChannel("x");
	m_scale->CreateChannel("y");
	m_scale->CreateChannel("z");

}
