#include "stdafx.h"
#include "../autoregistrar.h"
#include "../test.h"
#include "../assert.h"
using namespace testunit;
#include <string>
#include <algorithm>
#include <functional>
using namespace std;

static
void foo(){}

class _t: public testcase
{
public:
	_t(const string& name)
		:testcase(name){}
	void action()
	{}
};

struct check_test_name
{
	typedef test* first_argument_type;
	typedef const string second_argument_type;
	typedef bool result_type;
	bool operator () (test* t, const string& name) const
	{return t->name() == name;}
};

void _autoregistrar()
{
	collection c("sample");

	check_test_name cmp;

	{
		string name("testing_auto_registrar");
		tauto_registrar<_t> auto1(name.c_str(), c);
		assert(find_if(c.begin(), c.end(), bind2nd(cmp, name)) != c.end());
	}
	{
		string name("testing_auto_func_registrar");
		auto_func_registrar auto2(name.c_str(), foo, c);
		assert(find_if(c.begin(), c.end(), bind2nd(cmp, name)) != c.end());
	}
}

static 
auto_func_registrar MAKE_NAME("test for auto-registering",
							  _autoregistrar, base_collection());
