#include "stdafx.h"
#include "exception.h"
#ifdef max
#undef max
#endif
#include <limits>
using namespace std;

const string& testunit::exception::unknown_exception()
{
	static string s_unknown_exception = "unknown exception";
	return s_unknown_exception;
}

unsigned int testunit::exception::unknown_line_number()
{
	return std::numeric_limits<unsigned>::max();
}

const string& testunit::exception::unknown_filename()
{
	static string s_unknown_filename = "unknown filename";
	return s_unknown_filename;
}
