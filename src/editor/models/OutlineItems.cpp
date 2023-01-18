#include "OutlineItems.h"
#include <QDebug>

#include "curve.h"
#include "property.h"

using namespace Grafkit;
using namespace Idogep;

// ------------------------------------------------------------------------------------------


ItemHasAnimationsRole::ItemHasAnimationsRole() {
}

ItemHasAnimationsRole::~ItemHasAnimationsRole() {
}

void ItemHasAnimationsRole::SetAnimation(AnimationWrapperRef animation) { m_animation = animation; }

AnimationWrapperRef ItemHasAnimationsRole::GetAnimation() const { return m_animation; }

// ------------------------------------------------------------------------------------------


HasItemPropertiesRole::HasItemPropertiesRole() {
}

HasItemPropertiesRole::~HasItemPropertiesRole() {
}
