#pragma once
#include "testunit/failure.h"
#include "testunit/crash.h"

class CItsResult
{
public:
	virtual ~CItsResult(){}

	virtual std::string toString() = 0;
};

class CItsFailure: public CItsResult, public testunit::failure
{
public:
	CItsFailure(const testunit::failure&);

	virtual std::string toString();
};

class CItsCrash: public CItsResult, public testunit::crash
{
public:
	CItsCrash(const testunit::crash&);
	
	virtual std::string toString();
};

class CItsMessage: public CItsResult
{
	std::string m_message;

public:
	CItsMessage(const std::string&);

	virtual std::string toString();
};
