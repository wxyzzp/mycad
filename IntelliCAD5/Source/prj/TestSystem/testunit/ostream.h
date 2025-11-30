#pragma once
#include "testunit.h"
#include <fstream>
#include <string>
#include <sstream>

namespace testunit
{
using std::string;
using std::ofstream;
using std::ostringstream;

//forward declarations
class failure;
class crash;
class test;

class testunit_api ostream
{
public: //some customization for output
	class custom
	{
	public:
		virtual custom& operator << (const string&) = 0;
		virtual custom& operator << (const failure&) = 0;
		virtual custom& operator << (const crash&) = 0;
		virtual custom& operator << (const test&) = 0;
	};

	// state flags
	enum
	{
		eDebugOutput = 1<<0,
		eCout		 = 1<<1,
		eFile		 = 1<<2,
		eCustom		 = 1<<3,
		eAll		 = eDebugOutput|eCout|eFile|eCustom
	};

private: //data
	int m_mode;

	//custom stream
	custom* m_custom;

	// file stream
	ofstream* m_file;

	// string stream
	ostringstream m_os;

private: //methods

	void print(const string&);

public:
	ostream();

	ostream& operator << (const string&); // automatically thinned
	ostream& operator << (const char*);
	ostream& operator << (char);
	ostream& operator << (unsigned char);
	ostream& operator << (short);
	ostream& operator << (unsigned short);
	ostream& operator << (int);
	ostream& operator << (unsigned int);
	ostream& operator << (long);
	ostream& operator << (unsigned long);
	ostream& operator << (bool);
	ostream& operator << (float);
	ostream& operator << (double);

	ostream& operator << (const failure&);
	ostream& operator << (const crash&);
	ostream& operator << (const test&);

	// state managing
	int mode(int _mode = 0, const char* filename = 0, ostream::custom* = 0);
};

//declare global testunit output stream
extern ostream testunit_api tout;
}
