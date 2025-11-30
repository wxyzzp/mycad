#pragma once
#include "testunit.h"
#include <exception>
#include <string>
#include "exception.h"
#include "object.h"
#include "ostream.h"

namespace testunit
{

using std::string;

class testunit_api test: public object
{
public:
	//dtor
	virtual ~test(){}

	//run the test
	virtual void run() = 0;

	//returns the name of the test
	virtual const string& name() const = 0;

	//query: has the test subtest
	//if so, test should inherit testunit::collection_base
	virtual bool has_subtests() const = 0;
};

class testunit_api testcase: public test
{
public:
	//ctor
	testcase(const string& name)
		:m_name(name){}

	//framework implementation of run()
	virtual void run();

	//it's called from run() before action()
	virtual void pre_action() {};
	
	//implement test here
	virtual void action() throw (std::exception, testunit::exception) = 0;
	
	//it's called from run() after action()
	virtual void post_action() {};

	//returns the name of the test
	const std::string& name() const
	{return m_name;}

	//query: has the test subtest
	virtual bool has_subtests() const
	{return false;}

private:
	//name of the test
	std::string m_name;
};

// Method wrapper
template <typename _Ty>
class tmethod: public testcase
{
public:
	//type of wrapped method
	typedef void (_Ty::*tmethod_t)();

	//ctor
	tmethod(const string& name, _Ty* Ob, tmethod_t Me)
		:testcase(name), _Ob(Ob), _Me(Me){}

	//test action
	virtual void action() throw (testunit::exception, std::exception)
	{
		(_Ob->*_Me)();
	}

protected:
	//the object of class with wrapped method
	_Ty* _Ob;
	//wrapped method
	tmethod_t _Me;
};

// Function wrapper
class func: public testcase
{
public:
	//type of wrapped function
	typedef void(*test_func_t)();

	//ctor
	func(const string& name, test_func_t _foo)
		:testcase(name), m_foo(_foo){}

	//test action
	virtual void action() throw (std::exception, testunit::exception)
	{
		if (m_foo)
			m_foo();
	}

protected:
	//wrapped function
	test_func_t m_foo;
};

class testunit_api terminator
{
	static bool s_stop;
public:
	static void reset();
	static bool stop(bool = false);
};

}
