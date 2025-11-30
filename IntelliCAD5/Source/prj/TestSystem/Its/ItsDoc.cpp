#include "stdafx.h"
#include "ItsApp.h"
#include "ItsDoc.h"
#include "ItsMainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CItsDoc, CDocument)

CItsDoc::CItsDoc()
{}

CItsDoc::~CItsDoc()
{
	clear();
}

void CItsDoc::addCrash(CItsCrash* c)
{
	m_results.push_back(c);
	UpdateAllViews(0);
}

void CItsDoc::addFailure(CItsFailure* f)
{
	m_results.push_back(f);
	UpdateAllViews(0);
}

void CItsDoc::addMessage(CItsMessage* m)
{
	m_results.push_back(m);
	UpdateAllViews(0);
}

const std::vector<CItsResult*>& CItsDoc::getResults() const
{
	return m_results;
}

void CItsDoc::clear()
{
	std::vector<CItsResult*>::iterator i = m_results.begin();
	for (; i != m_results.end(); ++i)
		delete *i;
	m_results.clear();
}
