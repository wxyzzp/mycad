#include "stdafx.h"
#include "resource.h"
#include "ItsListener.h"
#include "testunit/test.h"
#include "testunit/failure.h"
#include "testunit/crash.h"
#include <string>
using namespace std;
using namespace testunit;

CItsListener::CItsListener(HWND hwnd)
:m_wnd(hwnd)
{}

testunit::ostream::custom& CItsListener::operator << (const string& s)
{
	SendMessage(m_wnd, ID_ADD_MESSAGE, 0, reinterpret_cast<long>(s.c_str()));
	return *this;
}

testunit::ostream::custom& CItsListener::operator << (const failure& f)
{
	SendMessage(m_wnd, ID_ADD_FAILURE, 0, reinterpret_cast<long>(&f));
	return *this;
}

testunit::ostream::custom& CItsListener::operator << (const crash& c)
{
	SendMessage(m_wnd, ID_ADD_CRASH, 0, reinterpret_cast<long>(&c));
	return *this;
}

testunit::ostream::custom& CItsListener::operator << (const test& t)
{
	PostMessage(m_wnd, ID_PROGRESS_STEP, 0, 0);

	string s = "Starting test: " + t.name() + "\r\n";
	return *this << s;
}


