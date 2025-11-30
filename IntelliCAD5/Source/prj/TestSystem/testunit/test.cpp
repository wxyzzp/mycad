#include "stdafx.h"
#include "test.h"
#include "exctrans.h"
#include "ostream.h"
#include "failure.h"
#include "crash.h"

namespace testunit
{

void testcase::run()
{
	INSTALL_EXCTRANS;

	if (terminator::stop())
		throw exit();

	try
	{
		pre_action();

		try
		{
			action();
		}
		catch (testunit::exception& e)
		{
			tout << failure(e, this);
		}
		catch (std::exception& e)
		{
			tout << crash(e.what(), this);
		}
		catch (...)
		{
			tout << crash("Unknown exception", this);
		}

		post_action();

	}
	catch (testunit::exception& e)
	{
		tout << failure(e, this);
	}
	catch (std::exception& e)
	{
		tout << crash(e.what(), this);
	}
	catch (...)
	{
		tout << crash("Unknown exception", this);
	}
}

bool terminator::s_stop = false;
void terminator::reset()
{
	s_stop = false;
}

bool terminator::stop(bool s)
{
	if (s)
		s_stop = true;
	return s_stop;
}

}
