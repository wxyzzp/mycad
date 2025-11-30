#include "stdafx.h"
#include "collection.h"
#include "ostream.h"
#include "exception.h"
#include "failure.h"
#include "crash.h"
#include <exception>

using namespace testunit;

collection::~collection()
{
	for (test* t = first(); t; t = next())
		delete t;
}

test* collection::first()
{
	if (m_tests.empty())
		return 0;
	m_icurr = 0;
	return m_tests.front();
}

const test* collection::first() const
{
	if (m_tests.empty())
		return 0;
	m_icurr = 0;
	return m_tests.front();
}

test* collection::next()
{
	if (++m_icurr < m_tests.size())
		return m_tests[m_icurr];
	return 0;
}

const test* collection::next() const
{
	if (++m_icurr < m_tests.size())
		return m_tests[m_icurr];
	return 0;
}

void collection::run()
{
	try
	{
		pre_action();

		for (test* t = first(); t; t = next())
			tout << *t, t->run();

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
	catch (exit& e)
	{
		throw e;
	}
	catch (...)
	{
		tout << crash("Unknown exception", this);
	}
}
