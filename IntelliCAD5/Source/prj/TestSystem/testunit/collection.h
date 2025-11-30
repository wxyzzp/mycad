#pragma once
#include "testunit.h"
#include "test.h"
#include <vector>
#include <string>

namespace testunit
{

class testunit_api collection_base: public test
{
public:
	//returns first test in the collection, 
	//if collection is empty returns 0
	virtual test* first() = 0;
	virtual const test* first() const = 0;
	//returns next test, 
	//if all tests passed returns 0
	virtual test* next() = 0;
	virtual const test* next() const = 0;

	//query: has the test subtest
	virtual bool has_subtests() const
	{return true;}
};

//forward declarations
template
class testunit_api std::vector<test*>;

class testunit_api collection: public collection_base
{
public://typedefs
	typedef std::vector<test*> tests_t;
	typedef tests_t::iterator iterator_t;
	typedef tests_t::const_iterator const_iterator_t;

public:
	//ctor
	collection(const string& name)
		:m_name(name), m_icurr(0){}
	//dtor
	virtual ~collection();

	//iterations
	test* first();
	const test* first() const;
	test* next();
	const test* next() const;
	
	const_iterator_t begin() const
	{return m_tests.begin();}
	const_iterator_t end() const
	{return m_tests.end();}

	//preparatory actions
	virtual void pre_action(){};
	//run tests
	virtual void run();
	//final actions
	virtual void post_action(){};

	//add test to the collection
	void add(test* te)
	{m_tests.push_back(te);}

	//get name of collection
	const std::string& name() const
	{return m_name;}

private://methods
	//prohibit copying
	collection(const collection&);
	const collection& operator = (const collection&);

private: //data
	//index of current test
	mutable unsigned int m_icurr;
	//name of collection
	std::string m_name;
	//tests
	tests_t m_tests;
};

}

// Helper macros
#define IMPLEMENT_CTOR_BEGIN(_class) _class::_class(const std::string& name)\
	:testunit::collection(name){\
	typedef _class _thisClassType;

#define ADD_TEST_METHOD(_meth) add(new testunit::tmethod<_thisClassType>(#_meth, this, _meth));

#define IMPLEMENT_CTOR_END }

// this function has not implemntation in testunit;
// you can implement it and use as base collection for tests in your library
testunit::collection& base_collection();
//possible implementation
#if 0
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
#endif
