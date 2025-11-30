/** FILENAME:     TileAttributes.cpp- Contains implementation of class CTileAttributes.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
*         This class holds the attibute name,attribute type and attribute value of a tile. 
*   Thess attributes are stored in the list stored in Tile class
*
*
* Audit Log 
*
*
* Audit Trail
*
*
*/


#include "stdafx.h"
#include "tileattributes.h"
#include "sds.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/************************************************************************************
/* + TileAttributes::TileAttributes() - Constructor of TileAttributes
*
*
*/
TileAttributes::TileAttributes()        // Constructor of TileAttributes
	{

	}


/************************************************************************************
/* + TileAttributes::~TileAttributes() - Destructor of TileAttributes
*
*
*/
TileAttributes::~TileAttributes()                   // Destructor of TileAttributes
	{

	}


/************************************************************************************
/* + TileAttributes::TileAttributes(CString name, CString type ,CString value)  
*
*       Constructor of TileAttributes
*
*/
TileAttributes::TileAttributes(CString name, CString type ,CString value)
:m_wszAttributeName(name),m_wszAttributeType(type) ,m_wszAttributeValue(value)
	{
     m_nRedundantInfo = 0;
	}


/*******************************************************************************************************
/* + CString TileAttributes::get_AttributeName() - Returns the attributes name as string.
*
*
*/
CString TileAttributes::get_AttributeName()    // Returns the attributes name as string.
	{
	return(m_wszAttributeName);
	}


/************************************************************************************
/* + CString TileAttributes::get_AttributeType() - Returns the attributes type  as string.
*
*
*/
CString TileAttributes::get_AttributeType()  // Returns the attributes type  as string.
	{
	return(m_wszAttributeType);
	}


/*************************************************************************************************
/* + CString TileAttributes::get_AttributeValue() - Returns the attributes value as string.
*
*
*/
CString TileAttributes::get_AttributeValue()  // Returns the attributes value as string.
	{
	return(m_wszAttributeValue);
	}


/************************************************************************************
/* + int TileAttributes::set_AttributeName(CString& name) - sets the attributes name
*
*
*/
int TileAttributes::set_AttributeName(                        // Sets the attributes name.
									  CString& name            // i: Attribute name.
									  )
	{
	m_wszAttributeName = name;
	return 0;
	}


/************************************************************************************
/* + int TileAttributes::set_AttributeType(CString& type) -  Sets the attributes type.
*
*
*
*
*
*/
int TileAttributes::set_AttributeType(                      //  Sets the attributes type.
									  CString& type          //  i: Attribute type.
									  )
	{
	m_wszAttributeType = type;
	return 0;
	}


/************************************************************************************
/* + int TileAttributes::set_AttributeValue(CString& value) - sets the attributes value
*                        
*
*/
int TileAttributes::set_AttributeValue(                         //  Sets the attributes value.
									   CString& value            //  i: Attribute value.
									   )
	{
	m_wszAttributeValue = value; 
	return 0;
	}



/************************************************************************************
/* + int TileAttributes::set_RedundantInfo(int nRedundantInfo) - 
*
*      Sets the value of m_nRedundantInfo.
*
*
*
*/
int TileAttributes::set_RedundantInfo(                                // Sets the value of redundant info.
									  int nRedundantInfo              // i: Index of the message in message list.
									  )
	{
	this->m_nRedundantInfo = nRedundantInfo;

	return SDS_RTNORM;

	}



/************************************************************************************
/* + int TileAttributes::get_RedundantInfo() - 
*
*    Returns the value of m_nRedundantInfo.
*   
*
*
*/
int TileAttributes::get_RedundantInfo()
	{

	return this->m_nRedundantInfo; 

	}
