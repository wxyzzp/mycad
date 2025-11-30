//	IcadDocTemplate.h : header file
//	Copyright (C) 1994,1995,1996,1997 by Visio Corporation.
//

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CIcadMultiDocTemplate

class CIcadMultiDocTemplate : public CMultiDocTemplate
{

//*** Member variables
public:

protected:

//*** Member functions
public:

	CIcadMultiDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
		CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass) :
		CMultiDocTemplate(nIDResource,pDocClass,pFrameClass,pViewClass) {}
	~CIcadMultiDocTemplate() {}

	virtual CDocument* OpenDocumentFile(LPCTSTR lpszPathName, BOOL bMakeVisible = TRUE);

protected:

};
