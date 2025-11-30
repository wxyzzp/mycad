// LayoutRename.cpp : implementation file
//

#include "icad.h"
#include "LayoutRename.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLayoutRename dialog


CLayoutRename::CLayoutRename(CWnd* pParent /*=NULL*/, LPCTSTR pLayoutName /*= _T("")*/)
	: CDialog(CLayoutRename::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLayoutRename)
	m_layoutName = pLayoutName;
	//}}AFX_DATA_INIT
}


void CLayoutRename::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLayoutRename)
	DDX_Text(pDX, IDC_LAYOUT_NEWNAME, m_layoutName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLayoutRename, CDialog)
	//{{AFX_MSG_MAP(CLayoutRename)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLayoutRename message handlers
