#ifndef _XML_H_
#define _XML_H_

// Developer: VT

// FORWARDS
class XMLNode;
class XMLString;

#pragma warning( disable: 4786 )

#ifdef  _MBCS
#include "windows.h"
#include "mbctype.h"
#endif

#include "string"
#include "tchar.h"
#include "vector"
#include "map"
#include <cstdarg>
#include <assert.h>
#define long_double __int64
#define XML_ASSERT(A) assert(A)

typedef std::vector<XMLNode*> XMLNodes;
typedef std::map<XMLString, XMLString> XMLMapStrStr;

// Own string class
// Main goals
// - 1. Comfortable and easy operations for XML support (String <=> {double, int} transformations)
// - 2. Independence from another string realization
// Now based on std::string
class XMLString
{
public:
	XMLString(std::string& str): m_String(str), m_bValid(true) {}
	XMLString(const XMLString& str): m_bValid(true), m_String(str.m_String) {}
	XMLString(): m_bValid(true) {}
	XMLString(const TCHAR* srcStr): m_String(srcStr), m_bValid(true) {}
	XMLString(bool bValid): m_bValid(bValid) {}

	// return int value of string
	int toInt(int defaultValue = 0) const
	{ return (isValid() && length()) ? _ttoi(m_String.c_str()):defaultValue; }

	long_double tolong_double(long_double defaultValue = 0) const
	{ return (isValid() && length()) ? _ttoi64(m_String.c_str()):defaultValue; }

	// return double value of string
	double toDouble(double defaultValue = 0) const
	{
		TCHAR* pStr;
		return (!isValid() || !length()) ? defaultValue:_tcstod (c_str(), &pStr);
	}

	// length of string
	int length() const { return isValid() ? m_String.size():0; }

	// z-string of TCHAR
	const TCHAR* c_str() const { static TCHAR zero = 0; return isValid() ? m_String.c_str():&zero; }

	// symbol by index
	const TCHAR& operator [](int i) const { static TCHAR zero = 0; return (isValid()) ? m_String[i]:zero; }

	// assign string value
	XMLString& operator=(const XMLString& str)
	{
		m_String = str.m_String;
		return *this;
	}

	// compare strings
	bool operator==(const XMLString& str) const
	{
		return m_String == str.m_String;
	}

	// compare strings
	bool operator!=(const XMLString& str) const
	{
		return !(*this == str);
	}

	// compare strings
	bool operator <(const XMLString& str) const
	{
		return m_String < str.m_String;
	}

    // same as format but return reference to this
	// designed to use for function's parameter assigning: func(XMLString().assign("<%s>", name));
	XMLString& assign(const char* fmt, ...)
	{
		va_list varg;

		va_start(varg, fmt);
		vformat(fmt, varg);
		va_end(varg);

		return *this;
	}

	// Form string with fmt (as sprintf)
	int format(const char* fmt, ...)
	{
		va_list varg;
		int result;

		va_start(varg, fmt);
		result = vformat(fmt, varg);
		va_end(varg);

		return result;
	}

	// Form string with fmt (as vsprintf)
	int vformat(const char* fmt, va_list varg)
	{
		char buf[1024];
		int result;

		result = _vsntprintf(buf, sizeof(buf)/sizeof(TCHAR), fmt, varg);
		if(result < sizeof(buf)/sizeof(TCHAR))
		{
			m_String = buf;
			return result;
		}

		int bufSz = sizeof(buf)*2;
		char *pBuf;
		for(;; bufSz *= 2)
		{
			pBuf = new char[bufSz];
			result = _vsntprintf(pBuf, bufSz/sizeof(TCHAR), fmt, varg);
			if(result < bufSz/sizeof(TCHAR) || bufSz > 60000)
			{
				m_String = pBuf;
				delete[] pBuf;
				return result;
			}
			delete[] pBuf;
		}
	}

	// dump memory to string (reverse operation - pickup)
	XMLString& dump(const char* ptr, int bytes)
	{
		static const char hexSyms[16] = {
			'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
		};

		for(resize(0); bytes-- > 0; ++ptr)
		{
			(*this) += hexSyms[(((unsigned char)*ptr) >> 4)];
			(*this) += hexSyms[(((unsigned char)*ptr) & 15)];
		}

		return *this;
	}

	// pick memory from string (unpack). Reverse to dump
	int pickup(char* ptr, int bytes)
	{
		int iSym, cnt;

		if((iSym = bytes*2) > length())
			iSym = length() & (~1);

		cnt = iSym >> 1;
		for(; (iSym -= 2) >= 0;)
			ptr[iSym >> 1] = mapHexSym(m_String[iSym + 1]) + (mapHexSym(m_String[iSym]) << 4);

		return cnt;
	}

	// append string to string
	XMLString& operator+=(const XMLString& str) { m_String += str.m_String; return *this; }
	// append string to string
	XMLString& operator+=(const TCHAR* str) { m_String += str; return *this; }
	// append symbol to string
	XMLString& operator+=(TCHAR ch) { m_String += ch; return *this; }
	// set new size of string - buffer are reallocated if need
    void resize(int newSize) { m_String.resize(newSize); }

	// EBATECH(CNBR) 2004/4/10 Support non-ANSI languages.
#ifdef	_MBCS
	// Convert unicode charactor to MBCS byte(s) and append it.
	XMLString& SetU( int uChar )
	{
		wchar_t	inbuf[1];
		char	str[3];
		int		outsize;
		inbuf[0] = uChar;
        outsize = WideCharToMultiByte( CP_ACP, 0, inbuf, 1, str, 2, 0, 0 ); 
        if(!outsize)
        {
            return *this;
        }
        str[outsize] = '\0';
		m_String += str;
		return *this;
	}
	// Get MBCS byte(s) and convert to unicode charactor.
	int GetU(int &index )
	{
		int ch;
		int insize;
		char inbuf[2];
		wchar_t outbuf[1];
		if( index + 1 < m_String.length() &&
			(_MBC_LEAD ==_mbbtype((unsigned char)m_String[index],0)) &&
			(_MBC_TRAIL==_mbbtype((unsigned char)m_String[index+1],1)))
		{
			insize = 2;
			inbuf[0] = m_String[index];
			inbuf[1] = m_String[index+1];
			index += 2;
		}
		else
		{
			insize = 1;
			inbuf[0] = m_String[index];
			index++;
		}
		MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,inbuf,insize,outbuf,1);
		ch = 0x0000ffff & outbuf[0];
        return ch;
	}
#endif

	// return validity of this string
	bool isValid() const { return m_bValid; }

private:
	int mapHexSym(TCHAR ch) { return 15 & (ch - ((ch > '9') ? 'a' - 10:'0')); }

	std::basic_string<TCHAR> m_String;
	bool m_bValid;// validity of this string
};

#ifndef _XMLNODE_H_
#include "xmlnode.h"
#endif

#endif //_XML_H_
