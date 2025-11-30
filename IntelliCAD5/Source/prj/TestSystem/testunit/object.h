#pragma once

#include "testunit.h"

namespace testunit
{

//class just redefine operators of dynamic creation of the object
//it need to be sure, all memory allocations/deallocations 
//(for testunit::object) use functions from standard library 
//linked with testunit.dll, 
//independantly of where testunit::object is created
class testunit_api object
{
public:
	static void *operator new (size_t);
	static void operator delete (void*);
	static void operator delete[] (void*);
};

}
