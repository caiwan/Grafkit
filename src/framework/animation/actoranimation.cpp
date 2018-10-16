#include "actoranimation.h"

#include "animation.h"

PERSISTENT_IMPL(Grafkit::ActorAnimation);

using namespace Grafkit;

ActorAnimation::ActorAnimation() : Animation() {
}

ActorAnimation::ActorAnimation(ActorRef actor) : Animation()
    , m_actor(actor) {
}

void ActorAnimation::Update(double time)
{
    const float3 t = m_position->GetFloat3(time);
    const float3 r = m_rotation->GetFloat3(time);
    const float3 s = m_scale->GetFloat3(time);

    assert(m_actor);

    m_actor->Transform().Identity();
    m_actor->Transform().RotateRPY(r);
    m_actor->Transform().Scale(s);
    m_actor->Transform().Translate(t);

}

void ActorAnimation::Initialize()
{
    m_position = new Track("Position", "xyz");
    AddTrack(m_position);

    m_rotation = new Track("Rotation", "rpy");
    AddTrack(m_rotation);

    m_scale = new Track("Scaling", "xyz");
    AddTrack(m_scale);
    m_scale->SetFloat3(0, float3(1, 1, 1));
}
