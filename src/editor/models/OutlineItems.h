#pragma once

#include "common.h"

namespace Idogep
{
    class AnimationWrapper;
    typedef Ref<AnimationWrapper> AnimationWrapperRef;

    class Properties;
    typedef Ref<Properties> PropertiesRef;

    // shall we have 
    // multiple animations maybe?
    class ItemHasAnimationsRole
    {
    public:
        ItemHasAnimationsRole();
        ~ItemHasAnimationsRole();

        void SetAnimation(AnimationWrapperRef animation);
        AnimationWrapperRef GetAnimation() const;

    protected:
        AnimationWrapperRef m_animation;
    };

    class HasItemPropertiesRole
    {
    public:
        HasItemPropertiesRole();
        ~HasItemPropertiesRole();
    protected:
        Ref<Properties> m_properties;
    };
}
