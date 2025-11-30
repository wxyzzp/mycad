#include "stdafx.h"
#include "../exception.h"
#include "../exctrans.h"
#include "../collection.h"
#include "../autoregistrar.h"
#include "../assert.h"
using namespace testunit;
#include <windows.h>
#include <memory>
using namespace std;

typedef testunit::exception tex;

class _exception: public collection
{
	void _cdtor();
	void _default_ctor();
	void _copy_ctor();
	void _assignment();

	bool same(const tex&, const tex&);
	tex* create_some_exception();

public:
	_exception(const string& name);
};

static
tauto_registrar<_exception> MAKE_NAME("test for class testunit::exception",
							  base_collection());

IMPLEMENT_CTOR_BEGIN(_exception)
	ADD_TEST_METHOD(_cdtor)
	ADD_TEST_METHOD(_default_ctor)
	ADD_TEST_METHOD(_copy_ctor)
	ADD_TEST_METHOD(_assignment)
IMPLEMENT_CTOR_END

void _exception::_cdtor()
{
	string what = "tex";
	string filename = "some file";
	unsigned int nline = 1;
	auto_ptr<tex> e(new tex(what, filename, nline));

	assert(e->what() == what);
	assert(e->filename() == filename);
	assert(e->line_number() == nline);
}

void _exception::_default_ctor()
{
	auto_ptr<tex> e(new tex);

	assert(e->what() == testunit::exception::unknown_exception());
	assert(e->line_number() == testunit::exception::unknown_line_number());
	assert(e->filename() == testunit::exception::unknown_filename());
}

void _exception::_copy_ctor()
{
	auto_ptr<tex> e(create_some_exception());
	auto_ptr<tex> ce(new tex(*e.get()));

	assert(same(*e.get(), *ce.get()));
}

void _exception::_assignment()
{
	auto_ptr<tex> e(create_some_exception());
	tex ce;
	ce = *(e.get());

	assert(same(ce, *e.get()));
}

tex* _exception::create_some_exception()
{
	string what = "tex";
	string filename = "some file";
	unsigned int nline = 1;
	return new tex(what, filename, nline);
}

bool _exception::same(const tex& e1, const tex& e2)
{
	return (e1.what() == e2.what()) && (e1.filename() == e2.filename()) &&
		(e1.line_number() == e2.line_number());
}

typedef testunit::sexception tsex;

class _sexception: public collection
{
	void _cdtor();
	void _copy_ctor();
	void _assignment();

	bool same(const tsex&, const tsex&);

public:
	_sexception(const string& name);
};

static
tauto_registrar<_sexception> MAKE_NAME("test for class testunit::sexception",
							  base_collection());

IMPLEMENT_CTOR_BEGIN(_sexception)
	ADD_TEST_METHOD(_cdtor)
	ADD_TEST_METHOD(_copy_ctor)
	ADD_TEST_METHOD(_assignment)
IMPLEMENT_CTOR_END

void _sexception::_cdtor()
{
	try
	{
		sexception_translator(EXCEPTION_ACCESS_VIOLATION, 0);
	}
	catch (const sexception&)
	{}
	catch (...)
	{
		assertex(false,"Function sexception_translator should generate testunit::sexception");
	}
}

void _sexception::_copy_ctor()
{
	try
	{
		sexception_translator(EXCEPTION_ACCESS_VIOLATION, 0);
	}
	catch (const sexception& e)
	{
		auto_ptr<sexception> ce(new sexception(e));
		
		assert(same(e, *ce.get()));
	}
}

void _sexception::_assignment()
{
	try
	{
		sexception_translator(EXCEPTION_ACCESS_VIOLATION, 0);
	}
	catch (const sexception& e)
	{
		try
		{
			sexception_translator(EXCEPTION_BREAKPOINT, 0);
		}
		catch (sexception& ce)
		{
			ce = e;
		
			assert(same(e, ce));
		}
	}
}

bool _sexception::same(const tsex& e1, const tsex& e2)
{
	return (string(e1.what()) == string(e2.what()) && e1.code() == e2.code());
}
