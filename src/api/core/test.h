#pragma once

#include "../GKSDK_config.h"

#include "exceptions.h"
#include <intrin.h>

// exceptions
//#define DebugAssertionFailureException(x) ExceptionEX(500, "Assertion failure")

// macros
#if defined CORE_DEBUG

#define DEBUG_BREAK(x) __debugbreak()

//#define DEBUG_ASSERT(x) if((x) != 0) throw new AssertionFailException(x)
#define DEBUG_ASSERT(x) if((x) == 0) {DEBUG_BREAK(x); throw new AssertionFailException(#x);}

#define ASSERT_TRUE(x)  if((x) == 0) {throw new AssertionFailException(#x);}
#define ASSERT_FALSE(x) if((x) != 0) {throw new AssertionFailException(#x);}

#define ASSERT_EXPR(x) (x?x:(NULL, throw new AssertionFailException(#x))

#define DEBUG_DO(x) x

#ifdef CORE_DUMPING
#define DEBUG_DUMP(x) x
#define DEBUG_OUT(x) x
#else
#define DEBUG_DUMP(x)
#define DEBUG_OUT(x)
#endif /*CORE_DUMPING*/

#else /*CORE_DEBUG*/

///@todo if((x) == 0) {throw new AssertionFailException(#x);}
#define DEBUG_ASSERT(x) 
#define DEBUG_DO(x)
#define DEBUG_OUT(x)
#define DEBUG_DUMP(x) 
#define DEBUG_BREAK(x)

#endif /*CORE_DEBUG*/

namespace FWdebug{
	/// Unit teszt eset interface; a teszter fogja automatikusan implementalni.
	class UnitTestScenario{
		public:
			UnitTestScenario(){}
			~UnitTestScenario(){}

			virtual void init() = 0;
			virtual void test() = 0;

			virtual int isRenderTest(){return 0;}
			virtual const char* getTestName() = 0;
	};
}