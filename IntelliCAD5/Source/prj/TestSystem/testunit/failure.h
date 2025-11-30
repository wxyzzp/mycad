#pragma once
#include "testunit.h"
#include "exception.h"

namespace testunit
{
class test;

//class descibes "excpected" testunit error, 
//with information: what test was failed
class testunit_api failure: public exception
{
public:
	//ctors
	failure()
		:exception(), m_test(0){}
	failure(const exception& e, test* _test)
		:exception(e), m_test(_test){}

	//get failed test
	test* failed_test() const
	{return m_test;}

private:
	//failed test
	test* m_test;
};

}
