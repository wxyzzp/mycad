#include "stdafx.h"
#include "../assert.h"
#include "../exception.h"
#include "../autoregistrar.h"
using namespace testunit;

void _assert()
{
	do
	{
		try
		{
			assert(false);
		}
		catch (const testunit::exception&)
		{
			break;
		}
		catch (...)
		{
			throw testunit::exception("Excpected exception of type testunit::exception",
				__FILE__, __LINE__);
		}
		throw testunit::exception("Excpected exception of type testunit::exception",
			__FILE__, __LINE__);
	}
	while (false);

	try
	{
		assert(true);
	}
	catch (...)
	{
		throw testunit::exception("Unexcpected exception",
			__FILE__, __LINE__);
	}
}

static auto_func_registrar MAKE_NAME("test for assert macro redifinition",
							  _assert, base_collection());
