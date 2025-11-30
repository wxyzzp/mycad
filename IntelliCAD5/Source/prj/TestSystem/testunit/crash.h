#pragma once
#include "testunit.h"
#include <string>

namespace testunit
{
class test;

//class describes the system crash (unexcpected exception)
//store the test, where crash was registered and info
class testunit_api crash
{
	//test, where crash was registered
	test* m_test;
	//info
	std::string m_what;

public:
	//ctor
	crash(const std::string& _what = "unknown_crash", test* _test = 0)
		:m_test(_test), m_what(_what){}

	//get crashed test
	test* crashed_test() const
	{return m_test;}

	//get info
	const std::string& what() const
	{return m_what;}
};

}
