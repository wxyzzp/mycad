#pragma once
#include "stdafx.h"
#include "../failure.h"
#include "../collection.h"
#include "../assert.h"
#include "../autoregistrar.h"
using namespace testunit;
#include <memory>
using namespace std;

typedef testunit::exception tex;

class _failure: public collection
{
	void _cdtor();
	void _default_ctor();
	void _copy_ctor();
	void _assignment();

	bool same(const failure&, const failure&);

public:
	_failure(const string& name);
};

void _failure::_cdtor()
{
	tex e("test", "file", 1);
	auto_ptr<failure> f(new failure(e, this));

	assert(f->failed_test() == this);
	assert(f->what() == e.what());
	assert(f->filename() == e.filename());
	assert(f->line_number() == e.line_number());
}

void _failure::_default_ctor()
{
	failure f;

	assert(f.failed_test() == 0);
	assert(f.what() == testunit::exception::unknown_exception());
	assert(f.line_number() == testunit::exception::unknown_line_number());
	assert(f.filename() == testunit::exception::unknown_filename());
}

void _failure::_copy_ctor()
{
	tex e("test", "file", 1);
	failure f(e, this);
	failure cf(f);

	assert(same(f, cf));
}

void _failure::_assignment()
{
	tex e("_assignment", "_failure.cpp", 47);
	failure f(e, this);
	failure cf;
	cf = f;

	assert(same(f, cf));
}

bool _failure::same(const failure& f1, const failure& f2)
{
	return f1.what() == f2.what() && f1.filename() == f2.filename()
		&& f1.line_number() == f2.line_number() 
		&& f1.failed_test() == f2.failed_test();
}

static
tauto_registrar<_failure> MAKE_NAME("test for class testunit::failure",
									base_collection());

IMPLEMENT_CTOR_BEGIN(_failure)
	ADD_TEST_METHOD(_cdtor)
	ADD_TEST_METHOD(_default_ctor)
	ADD_TEST_METHOD(_copy_ctor)
	ADD_TEST_METHOD(_assignment)
IMPLEMENT_CTOR_END
