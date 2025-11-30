#include "stdafx.h"
#include "../crash.h"
#include "../collection.h"
#include "../assert.h"
#include "../autoregistrar.h"
using namespace testunit;
#include <string>
#include <memory>
using namespace std;

class _crash: public collection
{
	void _cdtor();
	void _default_ctor();
	void _copy_ctor();
	void _assignment();

	bool same(const crash&, const crash&);

public:

	_crash(const string&);
};

void _crash::_cdtor()
{
	string what("test");
	auto_ptr<crash> c(new crash(what, this));

	assert(c->what() == what);
	assert(c->crashed_test() == this);
}

void _crash::_default_ctor()
{
	crash c;

	assert(c.what() == "unknown_crash");
	assert(c.crashed_test() == 0);
}

void _crash::_copy_ctor()
{
	crash c("test", this);
	crash cc(c);

	assert(same(c, cc));
}

void _crash::_assignment()
{
	crash c("test", this);
	crash cc;
	cc = c;

	assert(same(c, cc));
}

bool _crash::same(const crash& c1, const crash& c2)
{
	return c1.what() == c2.what() && c1.crashed_test() == c2.crashed_test();
}

static
tauto_registrar<_crash> MAKE_NAME("test for class testunit::crash",
							  base_collection());

IMPLEMENT_CTOR_BEGIN(_crash)
	ADD_TEST_METHOD(_cdtor)
	ADD_TEST_METHOD(_default_ctor)
	ADD_TEST_METHOD(_copy_ctor)
	ADD_TEST_METHOD(_assignment)
IMPLEMENT_CTOR_END
