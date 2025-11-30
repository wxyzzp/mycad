/** FILENAME:     ControlID.h- Contains declarations of class CControlID.
* Copyright  (C) Copyright 1998-99 Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
*	this class Maps the ID given to tile called Key to 
*	an ID given to windows control where Key is a CString Object
*   and ID is long
*
*Audit Log 
*
*
*Audit Trail
*
*
*/

#if !defined _DCLCONTROLID_H_
#define _DCLCONTROLID_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ControlID  
{
public:
	CString GetKey(); // this function returens The key of the Tile
	long GetID();	  // this function returens The Control ID of the Tile
	ControlID(		  // overloaded Constructor which takes the Key 
		CString key  // i:the Key of the Tile
		);
	ControlID() ;    // default constructor
	virtual ~ControlID(); // defAULT destructor

private:
	long m_ControlID; // variable which holds the tiles control ID
	CString m_Key;    // key of the control

protected:
	long GenCID ( void); // function to generate an unique ID
};

#endif 
