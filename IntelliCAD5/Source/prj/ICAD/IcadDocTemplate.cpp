/* D:\DEV\PRJ\ICAD\ICADDOCTEMPLATE.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

//	IcadDocTemplate.cpp
//	Copyright (C) 1994,1995,1996,1997 by Visio Corporation.
//  Code copied from MFC source was copied from BARDOCK.CPP 10/30/96 12:41 PM
//

//** Includes
#include "Icad.h"/*DNT*/
#include "IcadDocTemplate.h"/*DNT*/

/////////////////////////////////////////////////////////////////////////////
// CIcadMultiDocTemplate

CDocument* CIcadMultiDocTemplate::OpenDocumentFile(
	LPCTSTR lpszPathName, BOOL bMakeVisible)
{
	if(!bMakeVisible && NULL!=SDS_CMainWindow)
	{
		SDS_CMainWindow->m_bUsingTempView=TRUE;
	}
	return CMultiDocTemplate::OpenDocumentFile(lpszPathName,bMakeVisible);
}


