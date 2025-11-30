#include "stdafx.h"
#include "ItsResult.h"
#include "testunit/test.h"
using namespace testunit;
using namespace std;

CItsFailure::CItsFailure(const failure& f)
:failure(f)
{}

string CItsFailure::toString()
{
	string s;
	char buf[33];
	itoa(line_number(), buf, 10);
	s = string("Failure in test ") + failed_test()->name() + string(": ") + what() + "\r\n\t\t" +
		filename() + string("(") + string(buf) + string(")") + "\r\n";

	return s;
}

CItsCrash::CItsCrash(const crash& c)
:crash(c)
{}
	
string CItsCrash::toString()
{
	string s;
	s = string("Crash in test ") + crashed_test()->name() + string(": ") + what() + "\r\n"; 

	return s;
}

CItsMessage::CItsMessage(const string& s)
:m_message(s)
{}

string CItsMessage::toString()
{
	return m_message;
}
