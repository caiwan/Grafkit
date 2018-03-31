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
	m_position->CreateChannel("x");
	m_position->CreateChannel("y");
	m_position->CreateChannel("z");

	m_rotation = new Animation::Track();
	m_rotation->CreateChannel("r");
	m_rotation->CreateChannel("p");
	m_rotation->CreateChannel("y");

	m_scale = new Animation::Track();
	m_scale->CreateChannel("x");
	m_scale->CreateChannel("y");
	m_scale->CreateChannel("z");

}


