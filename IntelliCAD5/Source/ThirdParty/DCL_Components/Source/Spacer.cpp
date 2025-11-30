/** FILENAME:     Spacer.cpp- Contains implementation of class CSpacer.
* Copyright  (C) Copyright 1998-99 Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
*	this class Maps the basic (spacer , spacer0	)
*	to windows control
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
#include "Spacer.h"
#include "sds.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/************************************************************************************
/* + Spacer::Spacer()
*
*	Construction
*
*
*
*/

Spacer::Spacer()
	{
	}

/************************************************************************************
/* + Spacer::Spacer(int nType, int ParentLayout) 
*	Constructor.
*
*
*
*
*/

Spacer::Spacer(int nType, int ParentLayout)
	{
	m_nType = nType;
	m_ParentAlignment = ParentLayout;
	}
/************************************************************************************
/* + Spacer::~Spacer() 
*
*	Destruction
*
*
*
*/

Spacer::~Spacer()
	{
	}


BEGIN_MESSAGE_MAP(Spacer, CStatic)
	//{{AFX_MSG_MAP(Spacer)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/************************************************************************************
/* + int Spacer:: CreateOnScreenObj (CWnd* pParent)
*
*	creates the windows UI control
*	
*
*
*
*/

int Spacer:: CreateOnScreenObject (
								   CWnd* pParent	// i:pointer to the parent window
								   )
	{
	int error;
	DWORD dwStyle = WS_CHILD|WS_VISIBLE;
	ValueString = m_attriblist->get_ValueString();
	if(!m_attriblist->IsEnabled())
		{
		// set style to enable
		dwStyle = dwStyle|WS_DISABLED;
		}
	if(m_attriblist->IsTabstop())
		{
		// set style to tabstop
		//dwStyle = dwStyle|WS_TABSTOP;
		}

	CRect rect = PositionControl(); // gets the actual position on the dialog
	
	error = Create(NULL,dwStyle, rect, pParent, m_controlID.GetID()); // creates the ui control
	
	if (error)
		return SDS_RTNORM;	
	else
		return SDS_RSERR;
	}

/************************************************************************************
/* + int Spacer:: GetTile (	CString & szControlInfo,int lInfoMaxLength) 	
*
*	Returns the "value" attribute of the tile
*
*
*
*/

int Spacer:: GetTile (		// Returns the "value" attribute of the tile
		CString & szControlInfo,// o:Date contaned in value attribute
		int lInfoMaxLength	// o: Size of data
		)
	{
	//  if size of data requested is greater then 0 
	//  then get the text in the edit control and return
	if(lInfoMaxLength>0)
		{
		szControlInfo = ValueString;
		return SDS_RTNORM ; 
		}
	return SDS_RTERROR ; 
	}
