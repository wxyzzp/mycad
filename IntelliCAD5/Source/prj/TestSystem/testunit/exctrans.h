#pragma once
#include "testunit.h"
#include "object.h"
#include <eh.h>
#include <string>
#include <exception>

#define INSTALL_EXCTRANS  sexception_translator_installer _structured_exception_translator_installer;

namespace testunit
{

/*-------------------------------------------------------------------------*//**
Structired exception translator. You should not invoke this function directly.
This function invoked by run-time system when structured exception raised. It
throws C++ exception of class sexception.
@param code =>	code of Win32 exception
@param second	=> unknown
@author	Alexey Malov
*//*--------------------------------------------------------------------------*/
testunit_api void sexception_translator(unsigned int code, _EXCEPTION_POINTERS*);

class testunit_api sexception_translator_installer
{
public:
	sexception_translator_installer()
	{m_saved_exctrans = _set_se_translator(&sexception_translator);}

	~sexception_translator_installer()
	{_set_se_translator(m_saved_exctrans);}

private:
	_se_translator_function	m_saved_exctrans;
};

/*------------------------------------------------------------------------
Structured exceptions will be translated into this class by exctrans.
@author		Alexey Malov
------------------------------------------------------------------------*/
class testunit_api sexception: public std::exception, public object
{
	unsigned int m_code;

public:
	/*---------------------------------------------------------------------*//**
	@return code of Win32 exception
	*//*----------------------------------------------------------------------*/
	int	code() const
	{return m_code;}

private:

	friend void testunit::sexception_translator(unsigned int, _EXCEPTION_POINTERS*);

	/*---------------------------------------------------------------------*//**
    Constructor. Initialize internal variables : m_code and m_str
	@param code	=> code of Win32 exception
	*//*----------------------------------------------------------------------*/
    sexception(unsigned int code)
		:exception(make_str(code).c_str()){}

	/*---------------------------------------------------------------------*//**
	Function for generation string that describe Win32 exception
	@param code	=> code of Win32 exception
	*//*----------------------------------------------------------------------*/
    static std::string make_str(unsigned int code);
};

}
