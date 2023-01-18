#pragma once

#include "common.h"

namespace Idogep {

	class RecievesOutlineTreechangeRole {

	};

	class Properties;

	// shall we have 
	// multiple animations maybe?
	class HasItemAnimationsRole {
	public:
		HasItemAnimationsRole();
		~HasItemAnimationsRole();

		void SetAnimation(Grafkit::AnimationRef animation);
		Grafkit::AnimationRef GetAnimation() { return m_animation; }

	protected:
		Grafkit::AnimationRef m_animation;
	};

	class HasItemPropertiesRole {
	public:
		HasItemPropertiesRole();
		~HasItemPropertiesRole();
	protected:
		Ref<Properties> m_properties;
	};

}
