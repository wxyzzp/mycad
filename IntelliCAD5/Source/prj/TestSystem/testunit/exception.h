#pragma once
#include "testunit.h"
#include "object.h"
#include <string>

namespace testunit
{

//class describes testunit exception;
//it stores information about what happens, 
//where it happens (source file name and line number)
class testunit_api exception: public object
{
public:
	//ctor
	exception
	(
	const std::string& what = unknown_exception(), 
	const std::string& filename = unknown_filename(), 
	unsigned int nline = unknown_line_number()
	)
	:m_what(what), m_nline(nline), m_filename(filename){}

	//get info about exception
	const std::string& what() const
	{return m_what;}

	//get number of line, from exception was thrown
	unsigned int line_number() const
	{return m_nline;}
	//get source file name, where exception wos thrown
	const std::string& filename() const
	{return m_filename;}

	static const std::string& unknown_exception();
	static unsigned int unknown_line_number();
	static const std::string& unknown_filename();

protected:
	//what happened
	std::string m_what;
	//source file name
	std::string m_filename;
	//number of line, from exception was thrown
	unsigned int m_nline;
};

class testunit_api exit: public object
{};

}
