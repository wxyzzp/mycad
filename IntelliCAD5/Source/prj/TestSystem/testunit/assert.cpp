#include "stdafx.h"
#include "assert.h"
#include "exception.h"

__declspec(dllexport) void testunit::_assert(const char* message, const char* fileName, 
			 unsigned stringNumber) throw (testunit::exception)
{
	throw testunit::exception(message, fileName, stringNumber);
}
