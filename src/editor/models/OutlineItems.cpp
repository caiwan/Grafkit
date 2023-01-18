#include "OutlineItems.h"
#include <QDebug>

#include "curve.h"
#include "property.h"

using namespace Grafkit;
using namespace Idogep;

// ------------------------------------------------------------------------------------------


ItemHasAnimationsRole::ItemHasAnimationsRole()
{
}

ItemHasAnimationsRole::~ItemHasAnimationsRole()
{
}

void ItemHasAnimationsRole::SetAnimation(AnimationWrapperRef animation)
{
    qDebug() << "Animation set OutlineItem " << QString::fromStdString(animation->GetAnimation()->GetName()) << " Ptr:" << animation->GetAnimation().Get();
    m_animation = animation;
}

AnimationWrapperRef ItemHasAnimationsRole::GetAnimation() const { return m_animation; }

// ------------------------------------------------------------------------------------------


HasItemPropertiesRole::HasItemPropertiesRole()
{
}

HasItemPropertiesRole::~HasItemPropertiesRole()
{
}