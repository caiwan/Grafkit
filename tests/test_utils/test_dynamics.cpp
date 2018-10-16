#include "stdafx.h"

#include <gtest/gtest.h>

#include "utils/persistence/dynamics.h"

using namespace Grafkit;

class DummyClass : public Serializable 
{
    //DYNAMICS_DECL (DummyClass, 1)
    SERIALIZE(DummyClass, 1, ar)
    {
        assert(0);
    }
};

PERSISTENT_IMPL(DummyClass);

// ======

TEST(Dynamics, ModuleLevelFactoryTest) {
    std::shared_ptr<Serializable> item(Peristence::Instance().Create("DummyClass"));
    ASSERT_TRUE(item);
    ASSERT_TRUE(dynamic_cast<DummyClass*>(item.get()));
}
