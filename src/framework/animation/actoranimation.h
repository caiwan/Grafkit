#pragma once 

#include "animation.h"

namespace Grafkit {

	class ActorAnimation : public Animation {

	public:
		ActorAnimation();
		ActorAnimation(ActorRef actor);

		ActorRef GetActor() { return m_actor; };
		void SetActor(ActorRef actor) { m_actor = actor; };


		virtual void Update(double time);

	private:
		void CreateTrack();

	private:
		Ref <Animation::Track> m_position;
		Ref <Animation::Track> m_rotation;
		Ref <Animation::Track > m_scale;

		ActorRef m_actor;
	};

}