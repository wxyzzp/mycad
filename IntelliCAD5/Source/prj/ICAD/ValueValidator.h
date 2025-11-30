/* PRJ\ICAD\VALUEVALIDATOR.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.1.1.1 $ $Date: 2000/01/19 13:41:24 $ 
 * 
 * Abstract
 * 
 * Contains class definitions for validator objects that can
 * be used to validate various values.
 * 
 */ 

#if !defined(_VALUEVALIDATOR)
#define _VALUEVALIDATOR

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// CValueValidator
//
// Virtual base class for a validation object that can be derived from to
// perform validation of various values.

class CValueValidator
	{
public:
	CValueValidator();
	virtual ~CValueValidator() {}

	void SetValue(const CString& value);

	virtual bool ValidateValue(void) = 0;

protected:
	CString	m_value;
	};


/////////////////////////////////////////////////////////////////////////////
// CTableNameValidator
//
// A validation object that ensures that a name is a valid table entity name
// (such as a block name or a style name).

class CTableNameValidator : public CValueValidator
	{
public:
	CTableNameValidator();

	bool ValidateValue(void);
	};



#endif // !defined(_VALUEVALIDATOR)

