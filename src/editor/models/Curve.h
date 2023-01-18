#pragma once

#include "animation/animation.h"

namespace Idogep
{
    class TreeModel;

    class AnimationWrapper;
    typedef Ref<AnimationWrapper> AnimationWrapperRef;

//    //========================================================================================================

    class AnimationWrapper : public Referencable
    {
    public:
        explicit AnimationWrapper(const Grafkit::AnimationRef& animation);
        explicit AnimationWrapper(const Grafkit::AnimationRef& animation, const std::string& parentName);

        ~AnimationWrapper();

        Grafkit::AnimationRef GetAnimation() const { return m_animation; }
        void SetAnimation(const Grafkit::AnimationRef& animation) { m_animation = animation; }

        std::string GetParentName() const { return m_parentName; }
        void SetParentName(const std::string& parentName) { m_parentName = parentName; }

        TreeModel* GetAnimationListModel() const { return m_animationListModel; }
        void SetAnimationListModel(TreeModel* const animationListModel) { m_animationListModel = animationListModel; }

    private:
        Grafkit::AnimationRef m_animation;
        std::string m_parentName;
        TreeModel* m_animationListModel;
    };

}
