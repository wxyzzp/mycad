/** FILENAME:     ControlID.cpp- Contains implementation of class CControlID.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
*	this class Maps the ID given to tile called Key to 
*	an ID given to windows control where Key is a CString Object
*   and ID is long
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
#include "ControlID.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/************************************************************************************
/* + ControlID::ControlID () 
*
*	default constructor
*
*
*
*/

ControlID::ControlID()
	{
	}
/************************************************************************************
/* + ControlID::~ControlID () 
*
*	default destructor
*
*
*
*/

ControlID::~ControlID()
	{
	}
/************************************************************************************
/* + ControlID::ControlId(LPTSTR key) 
*
*	overloaded Constructor which takes the Key.
*
*
*
*/

ControlID::ControlID(
					 CString key  // Key of the Control
					 ):m_Key(key)
	{
	
	}
/************************************************************************************
/* + long ControlID::GetID ()
*
*	this function returens The Control ID of the Tile
*
*
*
*/

long ControlID::GetID()
	{
	static UniqueId=999;
	return(++UniqueId);
	}
/************************************************************************************
/* + CString ControlID::GetKey() 
*
*	this function returens The key of the Tile.
*
*
*
*/

CString ControlID::GetKey()
	{
	return m_Key;
	}
/************************************************************************************
/* + long ControlID::GenCID()
*
*	function to generate an unique ID
*
*
*
*/

long ControlID::GenCID()
	{
	static UniqueId=999;
	return(++UniqueId);
	}
