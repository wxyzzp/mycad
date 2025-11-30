#include "stdafx.h"
#include "../tuostream.h"

class _custom: public tuostream::custom
{
public:
	virtual custom& operator << (const string&)
	{
		return *this;
	};
	virtual custom& operator << (const failure&)
	{
		return *this;
	};
	virtual custom& operator << (const crash&)
	{
		return *this;
	};
	virtual custom& operator << (const test&)
	{
		return *this;
	};
};

