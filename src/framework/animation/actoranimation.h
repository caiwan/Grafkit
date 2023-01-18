#pragma once 

#include "animation.h"

namespace Grafkit {

    class ActorAnimation;
    typedef Ref<ActorAnimation> ActorAnimationRef;
    typedef Resource<ActorAnimation> ActorAnimationRes;
    typedef Ref<ActorAnimationRes> ActorAnimationResRef;

	class ActorAnimation : public Animation {

	public:
		ActorAnimation();
	    explicit ActorAnimation(ActorRef actor);

		ActorRef GetActor() const { return m_actor; };
		void SetActor(ActorRef actor) { m_actor = actor; };
        //void SetActor(Ref<Resource<Actor>> &actor) { m_actor = actor; };


		void Initialize() override;
	    void Update(double time) override;

	private:
	    Ref <Track> m_position;
		Ref <Track> m_rotation;
		Ref <Track> m_scale;

        //Ref<Resource<Actor>> m_actor;
		ActorRef m_actor;

        PERSISTENT_DECL(Grafkit::ActorAnimation, 1);
	protected:
	    void Serialize(Archive& ar) override;
	};

}
