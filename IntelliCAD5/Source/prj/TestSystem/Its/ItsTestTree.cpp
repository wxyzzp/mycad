#include "stdafx.h"
#include "ItsApp.h"
#include "ItsDoc.h"
#include "ItsTestTree.h"
#include "testunit/registry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CItsTestTree, CTreeView)

CItsTestTree::CItsTestTree()
{}

CItsTestTree::~CItsTestTree()
{}

BOOL CItsTestTree::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= TVS_HASBUTTONS | 
                    TVS_HASLINES | TVS_LINESATROOT | TVS_DISABLEDRAGDROP | 
                    TVS_NOTOOLTIPS | WS_BORDER | WS_TABSTOP;

	return CTreeView::PreCreateWindow(cs);
}

void CItsTestTree::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();

	fillTree(testunit::registry::get());
}

#ifdef _DEBUG
CItsDoc* CItsTestTree::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CItsDoc)));
	return (CItsDoc*)m_pDocument;
}
#endif //_DEBUG

bool CItsTestTree::fillTree
(
const testunit::collection_base& root,
HTREEITEM hParent //= TVI_ROOT
)
{
	CTreeCtrl& tree = GetTreeCtrl();
	using namespace testunit;

	if (hParent == TVI_ROOT)
	{
		TVINSERTSTRUCT ins;
		ins.hParent = TVI_ROOT;
		ins.hInsertAfter = TVI_LAST;
		ins.item.mask = TVIF_TEXT|TVIF_PARAM;
		ins.item.pszText = const_cast<char*>(root.name().c_str());
		ins.item.lParam = reinterpret_cast<long>(&root);

		hParent = tree.InsertItem(&ins);
	}

	for (const test* t = root.first(); t; t = root.next())
	{
		TVINSERTSTRUCT ins;
		ins.hParent = hParent;
		ins.hInsertAfter = TVI_LAST;
		ins.item.mask = TVIF_TEXT|TVIF_PARAM;
		ins.item.pszText = const_cast<char*>(t->name().c_str());
		ins.item.lParam = reinterpret_cast<long>(t);

		HTREEITEM hCurr = tree.InsertItem(&ins);
		if (NULL == hCurr)
			return false;

		if (t->has_subtests())
		{
			const collection_base* multi = static_cast<const collection_base*>(t);
			fillTree(*multi, hCurr);
		}
	}
	return true;
}
