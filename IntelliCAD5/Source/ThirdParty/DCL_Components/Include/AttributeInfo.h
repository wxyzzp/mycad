/** FILENAME:     AttributeInfo.h- Contains declarations of class CButton.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
*	This class holds the datatype and key attribute values if any. 
*	to windows control
*
* Audit Log 
*
*
* Audit Trail
*
*
*/

#if !defined _ATTRIBUTEINFO_H_
#define _ATTRIBUTEINFO_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Parser;

class AttributeInfo : public CObject  
{
public:
	
	AttributeInfo(                               // Called for DCL reseverved attributes.
		int nDataType,                           // i: Datatype to be set.
		CString& wszDefaultValue,                 // i: DefaultValue.
		CStringArray& arrayAttrList              // i: The attribute keyword list.
		);
	
	AttributeInfo(                               // Called for standard data types(int,real,string)
		int nDataType                            // i: Sets only the data type.
		);                        

	AttributeInfo();                               // Constructor.
	virtual ~AttributeInfo();                      // Destructor.

private:
	CStringArray m_arrayAttributeValues;           // Holds the list of possible values, the attribute can hold
	int m_nDataType;                               // Datatype of the attribute.
	CString m_wszDefaultValue;                     // The default value of the attribute.

    friend class AuditLevelProcessor;              // This class is just an extension of parser class, and does the audit level parsing.
	friend class Parser;                           // This class takes care of parsing. 
};

#endif 
