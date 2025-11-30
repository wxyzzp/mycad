#pragma once
#include "testunit.h"
#include "collection.h"

namespace testunit
{
//forward declarations
class test;

class testunit_api registry: public collection
{
	registry()
		:collection("All Tests"){}
public:
	static registry& get();
};

}
