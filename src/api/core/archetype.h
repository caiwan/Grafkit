/**
	Osztaly archetipus
	Debugging miatt kell 
*/
#pragma once

#include "../stdafx.h"

namespace Grafkit{
	class Object {

#ifndef LIVE_RELEASE
	public:

		Object() : _m_init(0)
		{}
		
		virtual ~Object() {
			ASSERT_DETAILS(_m_init != 0, "Objektum nem volt torolve megfeleloen");
		}

		virtual void Shutdown() = 0;

	private:
		int _m_init;

#endif //LIVE_RELEASE

	};

}
