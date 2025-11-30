// File   : ContextIOToolBar.h
// Author : Vasily Tarasov

#pragma once
#ifndef _CONTEXTIOTOOLBAR_H_
#define _CONTEXTIOTOOLBAR_H_

#include "afxcoll.h"

// FORWARDS
class XMLNode;
class XMLString;
class CIcadToolBar;

class CIcadContextIOToolBar
{
public:
	CIcadContextIOToolBar() {}
	virtual ~CIcadContextIOToolBar();

	void SetExcludingToolBars(CPtrArray& ExcludingToolBars);
	void SetDefines(CMapStringToString& defines);
	bool NeedToLoad(XMLNode& node);
	bool NeedToSave(const CIcadToolBar * pToolBar);

protected:
	bool IsKeywordsInDefined(XMLString& keywordsList);

private:
	CPtrArray m_ExcludingToolBars;
	CMapStringToString m_Defines;
};

#endif
