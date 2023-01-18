#pragma once

#include "common.h"

namespace Idogep {

	class RecievesOutlineTreechangeRole {

	};

	class Properties;

	// shall we have 
	// multiple animations maybe?
	class ItemHasAnimationsRole {
	public:
		ItemHasAnimationsRole();
		~ItemHasAnimationsRole();

		void SetAnimation(Grafkit::AnimationRef animation);
		Grafkit::AnimationRef GetAnimation() const ;
	    
	    std::string GetParentName() const { return m_parentName; }
	    void SetParentName(const std::string& parentName) { m_parentName = parentName; }

	protected:
		std::string m_parentName;
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
