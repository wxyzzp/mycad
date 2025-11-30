/* PRJ/ICADLIB/ContextIOToolBar.cpp
 *
 * Abstract
 *
 * Helping class for toolbar's import/export
 */

#include "icad.h"
#include "ContextIOToolBar.h"
#include "xml.h"
#include "IcadToolbar.h"

CIcadContextIOToolBar::~CIcadContextIOToolBar()
{
}

void CIcadContextIOToolBar::SetExcludingToolBars(CPtrArray& ExcludingToolBars)
{
	m_ExcludingToolBars.RemoveAll();
	m_ExcludingToolBars.Append(ExcludingToolBars);
}

void CIcadContextIOToolBar::SetDefines(CMapStringToString& defines)
{
	m_Defines.RemoveAll();

	POSITION pos;
	CString key, value;

	for(pos = defines.GetStartPosition(); pos != NULL; )
	{
		defines.GetNextAssoc(pos, key, value);
		m_Defines[key] = value;
	}
}

bool CIcadContextIOToolBar::NeedToLoad(XMLNode& node)
{
	XMLString* pValue;

	if((pValue = node.attribute("ifdef")) != NULL && !IsKeywordsInDefined(*pValue))
		return false;

	if((pValue = node.attribute("ifndef")) != NULL)
		return !IsKeywordsInDefined(*pValue);

	return true;
}

bool CIcadContextIOToolBar::NeedToSave(const CIcadToolBar * pToolBar)
{
	for(int i = m_ExcludingToolBars.GetSize(); --i >= 0; )
	{
		if(m_ExcludingToolBars[i] == (void*)pToolBar)
			return false;
	}

	return true;
}

bool CIcadContextIOToolBar::IsKeywordsInDefined(XMLString& keywordsList)
{
	CString checkedStr, fullStr, pseudoValue;

	fullStr = keywordsList.c_str();
	for( ; fullStr.GetLength(); )
	{
		checkedStr = fullStr.SpanExcluding(" ,;");
		if(m_Defines.Lookup(checkedStr, pseudoValue))
			return true;
		fullStr.Delete(0, checkedStr.GetLength() + 1);
	}
	
	return false;
}
