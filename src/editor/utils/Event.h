// Taken form Jimmy/TGD
// zengine / zengarden

#pragma once 

#include <set>
#include <cassert>
#include "fastdelegate.h"

using namespace fastdelegate;

namespace Idogep {

	template <class X, class Y, class RetType, typename... Params>
	FastDelegate<RetType(Params...)> Delegate(Y* x, RetType(X::*func)(Params...)) {
		assert(x);
		return FastDelegate<RetType(Params...)>(x, func);
	}

	template <class X, class Y, class RetType, typename... Params>
	FastDelegate<RetType(Params...)> Delegate(Y* x, RetType(X::*func)(Params...) const) {
		assert(x);
		return FastDelegate<RetType(Params...)>(x, func);
	}

	template <typename... Types> class Event {
	public:
		void operator += (const FastDelegate<void(Types...)>& delegate) {
			mDelegates.insert(delegate);
		}

		void operator -= (const FastDelegate<void(Types...)>& delegate) {
			mDelegates.erase(delegate);
		}

		void operator () (Types... message) {
			for (auto& delegate : mDelegates) {
				delegate(message...);
			}
		}

		std::set<FastDelegate<void(Types...)>> mDelegates;
	};
}
