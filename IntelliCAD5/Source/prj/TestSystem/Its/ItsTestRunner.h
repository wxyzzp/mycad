#pragma once
#include "testunit/ostream.h"

namespace testunit{class test;}

class CItsTestRunner
{
	testunit::test* m_pTest;
	HWND m_wnd;

	class CItsListener: public testunit::ostream::custom
	{
		HWND m_wnd;

	public:
		CItsListener();
		
		void setWindow(HWND);

		testunit::ostream::custom& operator << (const std::string&);
		testunit::ostream::custom& operator << (const testunit::failure&);
		testunit::ostream::custom& operator << (const testunit::crash&);
		testunit::ostream::custom& operator << (const testunit::test&);
	} 
	m_listener;

public:
	CItsTestRunner();
	~CItsTestRunner();

	void setWindow(HWND);

	void run(testunit::test*);
	void stop();

protected:
	static UINT runTests(LPVOID);
};
