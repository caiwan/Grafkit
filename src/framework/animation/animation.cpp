#include "stdafx.h"

#include "animation.h"

#include "camera.h"

using namespace Grafkit;

/* ============================================================================================== */

void Grafkit::Animation::_serialize(Archive & ar)
{
	PERSIST_STRING(ar, m_name);
}

// Serialize??? 
