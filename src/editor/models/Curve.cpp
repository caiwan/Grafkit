#include "Curve.h"

#include "utils/tree/treemodel.h"

using namespace Idogep;
using namespace Grafkit;

AnimationWrapper::AnimationWrapper(const AnimationRef& animation): m_animation(animation)
    , m_animationListModel(nullptr) {
}

AnimationWrapper::AnimationWrapper(const AnimationRef& animation, const std::string& parentName): m_animation(animation)
    , m_parentName(parentName)
    , m_animationListModel(nullptr) {
}

AnimationWrapper::~AnimationWrapper() { delete m_animationListModel; }
