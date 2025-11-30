#include "stdafx.h"
#include "resource.h"
#include "ItsTestRunner.h"
#include "testunit/test.h"
#include "testunit/ostream.h"
#include "testunit/failure.h"
#include "testunit/crash.h"
#include <string>
using namespace std;
using namespace testunit;

CItsTestRunner::CItsTestRunner()
:m_pTest(0), m_wnd(NULL)
{}

CItsTestRunner::~CItsTestRunner()
{
	if (m_pTest)
		stop();
}

void CItsTestRunner::setWindow(HWND wnd)
{
	m_wnd = wnd;
	m_listener.setWindow(wnd);
}

UINT CItsTestRunner::runTests
(
LPVOID p
)
{
	CItsTestRunner* _this = reinterpret_cast<CItsTestRunner*>(p);
	int stored_mode = tout.mode(testunit::ostream::eCustom|
					testunit::ostream::eDebugOutput, 0, &(_this->m_listener));

	tout << *(_this->m_pTest);

	terminator::reset();

	try
	{
		_this->m_pTest->run();
	}
	catch (testunit::exit&)
	{}
	
	_this->m_pTest = 0;

	tout.mode(stored_mode);

	SendMessage(_this->m_wnd, ID_PROGRESS_DISABLE, 0, 0);
	
	return 0;
}

void CItsTestRunner::run(test* t)
{
	if (m_pTest)
		return;

	m_pTest = t;
	AfxBeginThread(runTests, this);
}

void CItsTestRunner::stop()
{
	if (m_pTest)
		terminator::stop(true);
};

CItsTestRunner::CItsListener::CItsListener()
:m_wnd(NULL)
{}

void CItsTestRunner::CItsListener::setWindow(HWND wnd)
{
	m_wnd = wnd;
}

testunit::ostream::custom& CItsTestRunner::CItsListener::operator << (const string& s)
{
	SendMessage(m_wnd, ID_ADD_MESSAGE, 0, reinterpret_cast<long>(s.c_str()));
	return *this;
}

testunit::ostream::custom& CItsTestRunner::CItsListener::operator << (const failure& f)
{
	SendMessage(m_wnd, ID_ADD_FAILURE, 0, reinterpret_cast<long>(&f));
	return *this;
}

testunit::ostream::custom& CItsTestRunner::CItsListener::operator << (const crash& c)
{
	SendMessage(m_wnd, ID_ADD_CRASH, 0, reinterpret_cast<long>(&c));
	return *this;
}

testunit::ostream::custom& CItsTestRunner::CItsListener::operator << (const test& t)
{
	PostMessage(m_wnd, ID_PROGRESS_STEP, 0, 0);

	string s = "Starting test: " + t.name() + "\r\n";
	return *this << s;
}
