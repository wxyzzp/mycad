#pragma once

class CItsDoc;
namespace testunit{class collection_base;}

class CItsTestTree: public CTreeView
{
protected:
	CItsTestTree();
	DECLARE_DYNCREATE(CItsTestTree)

public:
	CItsDoc* GetDocument();

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual ~CItsTestTree();

protected:

	virtual void OnInitialUpdate(); // called first time after construct

	bool fillTree
	(
	const testunit::collection_base&,
	HTREEITEM hParent = TVI_ROOT
	);
};

#ifndef _DEBUG  // debug version in LeftView.cpp
inline CItsDoc* CItsTestTree::GetDocument()
   { return (CItsDoc*)m_pDocument; }
#endif
