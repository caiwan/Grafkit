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
	m_position->CreateChannel(0, "x");
	m_position->CreateChannel(1, "y");
	m_position->CreateChannel(2, "z");

	m_rotation = new Animation::Track();
	m_rotation->CreateChannel(0, "r");
	m_rotation->CreateChannel(1, "p");
	m_rotation->CreateChannel(2, "y");

	m_scale = new Animation::Track();
	m_scale->CreateChannel(0, "x");
	m_scale->CreateChannel(1, "y");
	m_scale->CreateChannel(2, "z");

}


