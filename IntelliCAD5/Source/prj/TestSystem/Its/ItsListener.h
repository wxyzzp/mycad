#pragma once
#include "testunit/ostream.h"

class CItsListener: public testunit::ostream::custom
{
	HWND m_wnd;

public:
	CItsListener();

	testunit::ostream::custom& operator << (const std::string&);
	testunit::ostream::custom& operator << (const testunit::failure&);
	testunit::ostream::custom& operator << (const testunit::crash&);
	testunit::ostream::custom& operator << (const testunit::test&);
};
