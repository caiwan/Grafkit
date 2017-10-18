#include "stdafx.h"

#include <gtest/gtest.h>

#include "utils/struct_pack.h"
#include "utils/exceptions.h"

using namespace Grafkit;
using namespace FWdebugExceptions;

TEST(InputSchemantic, given_input_schemantic_when_pack_then_ok) 
{
	StructPack packer;

	int id_position = packer.addField(16);
	int id_normal   = packer.addField(16);
	int id_tangent  = packer.addField(16);
	int id_binormal = packer.addField(16);
	int id_tex      = packer.addField(8);
	int id_group0   = packer.addField(2);
	int id_group1   = packer.addField(2);
	int id_group2   = packer.addField(2);
	int id_group3   = packer.addField(2);
	int id_weight0  = packer.addField(4);
	int id_weight1  = packer.addField(4);
	int id_weight2  = packer.addField(4);
	int id_weight4  = packer.addField(4);

	packer.addPointer(id_position  , 0, 0, 16);
	packer.addPointer(id_normal	   , 0, 0, 16);
	packer.addPointer(id_tangent   , 0, 0, 16);
	packer.addPointer(id_binormal  , 0, 0, 16);
	packer.addPointer(id_tex	   , 0, 0, 8);
	packer.addPointer(id_group0	   , 0, 0, 2);
	packer.addPointer(id_group1	   , 0, 0, 2);
	packer.addPointer(id_group2	   , 0, 0, 2);
	packer.addPointer(id_group3	   , 0, 0, 2);
	packer.addPointer(id_weight0   , 0, 0, 4);
	packer.addPointer(id_weight1   , 0, 0, 4);
	packer.addPointer(id_weight2   , 0, 0, 4);
	packer.addPointer(id_weight4   , 0, 0, 4);

	void * result = packer(0);

	ASSERT_FALSE(result == NULL);

}