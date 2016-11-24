#include "../stdafx.h"
#include "resource.h"

using namespace Grafkit;

void IResource::_serialize(Archive& ar) {
	ar & PERSIST_STRING(&m_name);
}
