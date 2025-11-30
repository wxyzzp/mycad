#include "stdafx.h"
#include "ostream.h"
#include "failure.h"
#include "crash.h"
#include "test.h"
#include <iostream>
#include <windows.h>
//using namespace testunit;

//define global testunit output stream
namespace testunit
{
ostream testunit_api tout;

ostream::ostream()
:m_custom(0),m_file(0),m_mode(0){}

void ostream::print(const std::string& s)
{
	if (m_mode & eDebugOutput)
	{
		OutputDebugString(s.c_str());
	}
	if (m_mode & eCout)
	{
		std::cout << s;
	}
	if (m_mode & eFile && m_file)
	{
		*m_file << s;
	}
	if (m_mode & eCustom && m_custom)
	{
		*m_custom << s;
	}
}

ostream& ostream::operator << (const failure& f)
{
	int _mode = m_mode;
	m_mode &= ~eCustom;
	
	*this << "fault:\t" << f.what() << "\n";
	if (f.failed_test())
		*this << "\t" << f.failed_test()->name();
	*this << f.filename() << ",	" << f.line_number() << "\n";

	m_mode = _mode;
	if (m_mode & eCustom && m_custom)
		*m_custom << f;

	return *this;
}

ostream& ostream::operator << (const crash& c)
{
	int _mode = m_mode;
	m_mode &= ~eCustom;
	
	*this << "crash:\t" << c.what() << "\n";
	if (c.crashed_test())
		*this << "\t" << c.crashed_test()->name();

	m_mode = _mode;
	if (m_mode & eCustom && m_custom)
		*m_custom << c;

	return *this;
}

ostream& ostream::operator << (const test& t)
{
	int _mode = m_mode;
	m_mode &= ~eCustom;

	*this << "starting test:\t" << t.name() << "\n";

	m_mode = _mode;
	if (m_mode & eCustom && m_custom)
		*m_custom << t;

	return *this;
}

ostream& ostream::operator << (const std::string& s)
{
	print(s);
	return *this;
}

ostream& ostream::operator << (const char* s)
{
	print(s);
	return *this;
}

#define IMPLEMENT_PRINT_FOR(_Ty) \
ostream& ostream::operator << (_Ty c)\
{\
	m_os << c;\
	print(m_os.str());\
	m_os.seekp(0, std::ios_base::beg);\
	return *this;\
}

IMPLEMENT_PRINT_FOR(char)
IMPLEMENT_PRINT_FOR(unsigned char)
IMPLEMENT_PRINT_FOR(short)
IMPLEMENT_PRINT_FOR(unsigned short)
IMPLEMENT_PRINT_FOR(int)
IMPLEMENT_PRINT_FOR(unsigned int)
IMPLEMENT_PRINT_FOR(long)
IMPLEMENT_PRINT_FOR(unsigned long)
IMPLEMENT_PRINT_FOR(bool)
IMPLEMENT_PRINT_FOR(float)
IMPLEMENT_PRINT_FOR(double)

// state managing
int ostream::mode(int _mode, const char* filename, ostream::custom* _custom)
{
	if (_mode == 0)
		return m_mode;

	_mode &= eAll;

	if (_mode & eFile)
	{
		delete m_file;
		m_file = new ofstream(filename);
	}
	if (_mode & eCustom)
	{
		m_custom = _custom;
	}

	int _m = m_mode;
	m_mode = _mode;
	return _m;
}

}
