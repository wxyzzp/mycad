#pragma once
#include "testunit.h"
#pragma message ("assert is redefined by testunit\n")

//Microsoft C++ (v6.0) does not support the function exception specification mechanism, 
//as described in section 15.4 of the ANSI C++ draft
#pragma warning (disable:4290)

namespace testunit
{
class exception;

testunit_api void _assert(const char* message, const char* fileName, unsigned stringNumber)
	throw (testunit::exception);
}

#ifdef assert
#undef assert
#endif
// we need macro to have possibility hidenly pass filename and line number into function
// we need redefine standard assert to prohibite its usage
#define assert(condition) (void)((condition) || (testunit::_assert(#condition,__FILE__,__LINE__),0) )

//macro with extended description of condition
#define assertex(condition, description) (void)((condition) ||\
	(testunit::_assert((description),__FILE__,__LINE__), 0))

//macro for unconditional exception throwing
#define throwex(description) testunit::_assert((description),__FILE__,__LINE__)
