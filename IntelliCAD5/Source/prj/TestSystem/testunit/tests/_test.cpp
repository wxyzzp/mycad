#include "stdafx.h"
#include "../collection.h"
#include "../assert.h"
#include "../autoregistrar.h"
using namespace testunit;
#include <string>
#include <memory>
using namespace std;

collection& base_collection()
{
	class base_test_collection: public collection
	{
	public:
		base_test_collection()
			:collection("Collection of tests for testunit lib")
		{
			registry::get().add(this);
		}
	};

	static base_test_collection* s_base = new base_test_collection;

	return *s_base;
}

// Test class testunit::test
class _t: public test
{
	string m_name;
public:
	_t(const string& name)
		:m_name(name){}
	void run()
	{}
	const string& name() const
	{return m_name;}
	bool has_subtests() const
	{return false;}
};

void _test()
{
	// Create test object
	string name("_test");
	auto_ptr<_t> t(new _t(name));

	// Run test
	t->run();
	// Check method name()
	assert(t->name() == name);
}
static
auto_func_registrar MAKE_NAME("test for class testunit::test", 
							  _test, base_collection());

// Test class testunit::testcase
class _tc: public testcase
{
public:
	_tc(const string& name)
		:testcase(name){}
	
	void pre_action()
	{
		tout << "pre_action (it's before action)\r\n";
	}
	void action()
	{
		tout << "action: we've done it\r\n";
	}
	void post_action()
	{
		tout << "post_action (it's after action)\r\n";
	}
};

void _testcase()
{
	// Create test object
	string name("_test");
	auto_ptr<_tc> t(new _tc(name));

	// Run test
	t->run();
	// Check method name()
	assert(t->name() == name);
}
static
auto_func_registrar MAKE_NAME("test for class testunit::testcase",
							  _testcase, base_collection());

// Test class testunit::tmethod

class _bar
{
public:
	void method();
};
void _bar::method()
{}

void _tmethod()
{
	// Create tmethod test
	_bar b;
	string name("_tmethod");
	auto_ptr<tmethod<_bar> > t(new tmethod<_bar>(name, &b, _bar::method));

	// Check method name
	assert(t->name() == name);
#if 0
	// Check method size
	assert(t->size() == 1);
#endif
	// Run test
	t->run();
}
static
auto_func_registrar MAKE_NAME("test for class testunit::tmethod",
							  _tmethod, base_collection());

// Test class testunit::tfunc
void _foo()
{
	assert(true);
}

void _func()
{
	string name("_foo");
	auto_ptr<func> t(new func(name, _foo));

	// Check method name
	assert(t->name() == name);
#if 0
	// Check method size
	assert(t->size() == 1);
#endif
	// Run test
	t->run();
}
static
auto_func_registrar MAKE_NAME("test for class testunit::func",
							  _func, base_collection());

void _fail_func()
{
	bool thisMessageShouldBeDysplayedInFailureInfo = false;
	assert(thisMessageShouldBeDysplayedInFailureInfo);
}

static
auto_func_registrar MAKE_NAME("test failure", _fail_func, base_collection());

void _crash_func()
{
	*reinterpret_cast<int*>(0x0) = 1;
}

static
auto_func_registrar MAKE_NAME("test crash", _crash_func, base_collection());

