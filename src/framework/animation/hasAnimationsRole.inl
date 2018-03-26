#pragma once 
#include "scene.h"
#include "animation.h"

inline void Grafkit::HasAnimationsRole::UpdateAnimations(float t)
{
	for (size_t i = 0; i < m_animations.size(); i++) {
		AnimationRef &animation = m_animations[i];
		animation->Update(t);
	}
}
