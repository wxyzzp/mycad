/* PRJ\ICAD\VALUEVALIDATOR.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.2 $ $Date: 2001/07/05 07:44:01 $ 
 * 
 * Abstract
 * 
 * Contains class implementations for validator objects that can
 * be used to validate various values.
 * 
 */ 


#include "icad.h"			/*DNT*/
#include "ValueValidator.h"	/*DNT*/

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CValueValidator construction/destruction

/******************************************************************************
*+ CValueValidator::CValueValidator - standard constructor
*
*/

CValueValidator::CValueValidator(
	)
	{
	m_value			= _T(""/*DNT*/);
	}


/////////////////////////////////////////////////////////////////////////////
// CValueValidator setters/getters

/******************************************************************************
*+ CValueValidator::SetValue - sets the value
*
*/

void CValueValidator::SetValue(
	const CString& value)						// i - the value to be validated
	{
	m_value = value;
	}



/////////////////////////////////////////////////////////////////////////////
// CTableNameValidator construction/destruction

/******************************************************************************
*+ CTableNameValidator::CTableNameValidator - standard constructor
*
*/

CTableNameValidator::CTableNameValidator(
	)
	: CValueValidator()
	{
	}



/////////////////////////////////////////////////////////////////////////////
// CTableNameValidator implementation

/******************************************************************************
*+ CTableNameValidator::CTableNameValidator - public constructor
*
*/

bool CTableNameValidator::ValidateValue(
	void)
	{

	// make sure the name meets the length/character criteria
	// and it hasn't been used already
	resbuf *tmpRb = NULL;
    if(!ic_isvalidname(m_value.GetBuffer(m_value.GetLength() + 1)))
		{
		CString	str;	/*D.G.*/
		str.FormatMessage(ResourceString(IDC_WRONG_NAME_MESSAGE, "The name can use up to %1!d! characters, including letters, numbers,\nhyphens, underscores, and dollar signs ($), but not spaces.\nPlease try again."), IC_ACADNMLEN - 1);
        AfxMessageBox((LPCTSTR)str);
        return false;
		}
	else if ((tmpRb = sds_tblsearch("STYLE"/*DNT*/, m_value.GetBuffer(IC_ACADNMLEN + 1), 0)) != NULL)
		{
        IC_RELRB(tmpRb);
		AfxMessageBox(ResourceString(IDC_TEXTSTYLE_DUPLICATE_NAME_1, "You have entered a style name that already exists in the drawing.  Please enter a unique name for this style."));
        return false;
	    }

	return true;
	}



