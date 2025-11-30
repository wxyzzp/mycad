#include "stdafx.h"
#include "../object.h"
#include "../assert.h"
#include "../collection.h"
#include "../autoregistrar.h"
using namespace testunit;
#include <crtdbg.h>

class _crt_mem_difference
{
	_CrtMemState m_start;
	_CrtMemState m_end;

	int m_modes[_CRT_ERRCNT];

public:
	_crt_mem_difference();
	~_crt_mem_difference();
};

_crt_mem_difference::_crt_mem_difference()
{
	_CrtMemCheckpoint(&m_start);
}

_crt_mem_difference::~_crt_mem_difference()
{
	_CrtMemCheckpoint(&m_end);

	{
		m_modes[_CRT_ASSERT] = _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_REPORT_MODE);
		m_modes[_CRT_WARN] = _CrtSetReportMode(_CRT_WARN, _CRTDBG_REPORT_MODE);
		m_modes[_CRT_ERROR] = _CrtSetReportMode(_CRT_ERROR, _CRTDBG_REPORT_MODE);

		_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
		_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);

		_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
		_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);

		_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
		_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
	}

	_CrtMemState diff;
	int exist_memory_difference = _CrtMemDifference(&diff, &m_start, &m_end);

	{
		_CrtSetReportMode(_CRT_ASSERT, m_modes[_CRT_ASSERT]);
		_CrtSetReportMode(_CRT_WARN, m_modes[_CRT_WARN]);
		_CrtSetReportMode(_CRT_ERROR, m_modes[_CRT_ERROR]);
	}

	assert(!exist_memory_difference);
}

class _object: public collection
{
	class _t: public object
	{
		char* m_buf;

	public:
		_t::_t()
		{m_buf = new char [11];}

		_t::~_t()
		{delete m_buf;}
	};

	void _new_delete();
	void _new_delete_array();

public:
	_object(const string&);
};

void _object::_new_delete()
{
	_crt_mem_difference md;

	_object::_t* o = new _object::_t;
	delete o;
}

void _object::_new_delete_array()
{
	_crt_mem_difference md;

	_object::_t* o = new _object::_t[10];
	delete [] o;
}

static
tauto_registrar<_object> MAKE_NAME("test for class testunit::object",
									base_collection());

IMPLEMENT_CTOR_BEGIN(_object)
	ADD_TEST_METHOD(_new_delete)
	ADD_TEST_METHOD(_new_delete_array)
IMPLEMENT_CTOR_END
	
