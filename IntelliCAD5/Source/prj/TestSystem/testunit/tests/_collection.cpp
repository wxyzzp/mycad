#include "stdafx.h"
#include "../collection.h"
#include "../assert.h"
#include "../autoregistrar.h"
using namespace testunit;
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
using namespace std;

class _outcome;

// Test class testunit::collection
class _test: public testcase
{
public:
	_test(const string& name)
		:testcase(name){}
	virtual void action(){}
};

void _collection()
{
	// Construct tests for collections
	auto_ptr<_test> t1(new _test("test1"));
	auto_ptr<_test> t2(new _test("test2"));
	auto_ptr<_test> t3(new _test("test3"));
	// Construct subcllection
	string sname("subcollection");
	auto_ptr<collection> sc(new collection(sname));
	sc->add(t1.release());
	// Construct collection
	string name("collection");
	auto_ptr<collection> c(new collection(name));
	c->add(t2.release());
	c->add(t3.release());
	c->add(sc.release());

	// Check method name()
	assert(sc->name() == sname);
	assert(c->name() == name);

	// Check method fisrt()
	{
		assert(c->first() != 0);

		collection oc("other collection");
		assert(oc.next() == 0);
		assert(oc.first() == 0);
	}
	
	//Check method next()
	{
		int i = 0;
		for (test* t = c->first(); t = c->next();++i);
		assert(t == 0);
		assert(i == 2);
	}

	//Check existance of subtests
	{
		assert(sc->end() != find(sc->begin(), sc->end(), t1.get()));

		assert(c->end() != find(c->begin(), c->end(), t2.get()));
		assert(c->end() != find(c->begin(), c->end(), t3.get()));
		assert(c->end() != find(c->begin(), c->end(), sc.get()));
	}

	// Run tests
	sc->run();
	c->run();
}

static 
auto_func_registrar MAKE_NAME("test for class testunit::collection",
							  _collection, base_collection());


