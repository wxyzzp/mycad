/** FILENAME:     AttributeInfo.cpp- Contains implementation of class AttributeInfo.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
*
*
*
*Audit Log 
*
*
*Audit Trail
*
*
*/

#include "stdafx.h"
#include "AttributeInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;

#define new DEBUG_NEW
#endif


/************************************************************************************
/* + AttributeInfo::AttributeInfo() - 
*
*
*
*
*
*/
AttributeInfo::AttributeInfo()
{

}

/************************************************************************************
/* + AttributeInfo::~AttributeInfo() - 
*
*        Remove all the attributes.
*
*
*
*/
AttributeInfo::~AttributeInfo()
{
    this->m_arrayAttributeValues.RemoveAll();  
}



/************************************************************************************
/* + AttributeInfo::AttributeInfo(int nDataType) - 
*
* 
*   Sets the datatype of attribute.This constructor is called whenever attribute added is 
*  has a datatype of either int,real or string.
*
*/
AttributeInfo::AttributeInfo(                              // Constructor.
							 int nDataType                 // Datatype, the values passed  are defined in Constants.h. 
							 )
	{
 
	/* nDataType can be
		INT_DATA_TYPE : for integer
		REAL_DATA_TYPE : for real
		STRING_DATA_TYPE : for string 
	*/
	m_nDataType = nDataType;

	}	


/************************************************************************************
/* + AttributeInfo::AttributeInfo(int nDataType, CStringArray arrayAttrList) - 
*
*     This create an object with a data type and the respective attribute values 
*   to be held.  
*
*
*/
AttributeInfo::AttributeInfo(                                       // Constructor.
							 int nDataType,                         // i:Datatype, the values passed  are defined in Constants.h. 
							 CString& wszDefaultValue,              // i: Default value. 	
							 CStringArray& arrayAttrList            // i:The list of values which can be held by them.
							 )
	{
	m_nDataType = nDataType;  // refer to previous contrsuctor for the values this variable can hold.
	m_wszDefaultValue = wszDefaultValue;    // Sets the default value of attribute if any.
	m_arrayAttributeValues.Append(arrayAttrList); // Append the attribute value list.
	}
