#include "stdafx.h"

#include <gtest/gtest.h>

#include "utils/struct_pack.h"
#include "utils/exceptions.h"

using namespace Grafkit;
using namespace FWdebugExceptions;

TEST(InputSchemantic, InputSchemanticTest)
{
    char p[] = {
        0,1,2,3,
        4,5,6,7,
        8,9,10,11,
        12,13,14,15
    };

    StructPack packer;

    int id_position = packer.addField(16);
    int id_normal = packer.addField(16);
    int id_tangent = packer.addField(16);
    int id_binormal = packer.addField(16);
    int id_tex = packer.addField(8);
    int id_group0 = packer.addField(2);
    int id_group1 = packer.addField(2);
    int id_group2 = packer.addField(2);
    int id_group3 = packer.addField(2);
    int id_weight0 = packer.addField(4);
    int id_weight1 = packer.addField(4);
    int id_weight2 = packer.addField(4);
    int id_weight4 = packer.addField(4);

    packer.addPointer(id_position, p, 0, 16);
    packer.addPointer(id_normal, p, 0, 16);
    packer.addPointer(id_tangent, p, 0, 16);
    packer.addPointer(id_binormal, p, 0, 16);
    packer.addPointer(id_tex, p, 0, 8);
    packer.addPointer(id_group0, p, 0, 2);
    packer.addPointer(id_group1, p, 0, 2);
    packer.addPointer(id_group2, p, 0, 2);
    packer.addPointer(id_group3, p, 0, 2);
    packer.addPointer(id_weight0, p, 0, 4);
    packer.addPointer(id_weight1, p, 0, 4);
    packer.addPointer(id_weight2, p, 0, 4);
    packer.addPointer(id_weight4, p, 0, 4);

    int* result = reinterpret_cast<int*> (packer(1));

    ASSERT_TRUE(result != nullptr);
}
