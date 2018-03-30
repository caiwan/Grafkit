#pragma once 

#include "animation.h"

namespace Grafkit {

	class ActorAnimation : public Animation {

	public:

		// setActor
		// getActor

		virtual void Update(double time) = 0;

	private:
		Ref <
			Animation::Track
		> m_position;
	

		Ref <
			Animation::Track
		> m_rotation;


		Ref <
			Animation::Track
		> m_scale;

		ActorRef m_actor;
	};

}