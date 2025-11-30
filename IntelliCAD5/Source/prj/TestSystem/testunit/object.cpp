#include "stdafx.h"
#include "object.h"
#include <new>
#include <cstddef>
using namespace testunit;

void* object::operator new (size_t s)
{
	return ::operator new(s);
}

void object::operator delete (void* p)
{
	::operator delete(p);
}

void object::operator delete[] (void* p)
{
	::operator delete(p);
}
