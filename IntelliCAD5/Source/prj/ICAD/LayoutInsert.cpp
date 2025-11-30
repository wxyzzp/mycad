// LayoutInsert.cpp : implementation file
//

#include "stdafx.h"
#include "icad.h"
#include "LayoutInsert.h"
#include "cmdsLayout.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLayoutInsert dialog


CLayoutInsert::CLayoutInsert(CWnd* pParent /*=NULL*/)
	: CDialog(CLayoutInsert::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLayoutInsert)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CLayoutInsert::CLayoutInsert(CWnd* pParent, db_drawing* pDestDrawing, db_drawing* pSourceDrawing)
	: CDialog(CLayoutInsert::IDD, pParent),
	m_pDrawingDest(pDestDrawing), m_pDrawingSource(pSourceDrawing),
	m_initialWidth(0), m_initialHeight(0)
{
}

void CLayoutInsert::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLayoutInsert)
	DDX_Control(pDX, IDC_LAYOUT_LIST, m_layoutList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLayoutInsert, CDialog)
	//{{AFX_MSG_MAP(CLayoutInsert)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLayoutInsert message handlers

void CLayoutInsert::OnOK() 
{
	char layoutName[IC_ACADBUF];
	CLayoutManager lmSource(m_pDrawingSource);
	CLayoutManager lmDest(m_pDrawingDest);
	sds_name sdsSourceLayoutName;
	CDbLayout* pSourceLayout = NULL;
	for(int i = 0; i < m_layoutList.GetCount(); ++i)
	{
		if(m_layoutList.GetSel(i))
		{
			m_layoutList.GetText(i, layoutName);
			pSourceLayout = lmSource.findLayoutNamed(layoutName);
			if(pSourceLayout != NULL)
			{
				sdsSourceLayoutName[0] = (long)pSourceLayout;
				sdsSourceLayoutName[1] = (long)m_pDrawingSource;
				lmDest.importLayout(sdsSourceLayoutName);
			}
		}
	}
	
	CDialog::OnOK();
}

BOOL CLayoutInsert::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	const char* pName;
	CDbLayout* pLayout;
	db_blocktabrec* pBlock;

	CLayoutManager lm(m_pDrawingSource);
	lm.resetIterator();
	while(lm.next(pName, &pLayout, &pBlock, false))
		m_layoutList.AddString(pName);
	
	CRect r;
	GetClientRect(&r);
	placeControls(r.right, r.bottom);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLayoutInsert::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	if(cx != 0 && cy != 0)
		placeControls(cx, cy);
	return;
}

int CLayoutInsert::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_initialWidth = lpCreateStruct->cx;
	m_initialHeight = lpCreateStruct->cy;
	return 0;
}

void CLayoutInsert::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    if(m_initialWidth == 0 && m_initialHeight == 0)
	{
		CDialog::OnGetMinMaxInfo(lpMMI);
		return;
	}
    lpMMI->ptMinTrackSize.x = m_initialWidth;
    lpMMI->ptMinTrackSize.y = m_initialHeight;
}

#define CONTROLS_OFFSET 10
void CLayoutInsert::placeControls(int cx, int cy)
{
	CRect buttonRect;
	if(GetDlgItem(IDOK) != NULL)
	{
		GetDlgItem(IDOK)->GetClientRect(&buttonRect);
		GetDlgItem(IDOK)->MoveWindow(cx - CONTROLS_OFFSET - buttonRect.right, 
			CONTROLS_OFFSET, 
			buttonRect.right, 
			buttonRect.bottom); 
	}
	if(GetDlgItem(IDCANCEL) != NULL)
		GetDlgItem(IDCANCEL)->MoveWindow(cx - CONTROLS_OFFSET - buttonRect.right, 
		CONTROLS_OFFSET + buttonRect.bottom + CONTROLS_OFFSET, 
		buttonRect.right, 
		buttonRect.bottom); 
	if(m_layoutList.GetSafeHwnd())
		m_layoutList.MoveWindow(CONTROLS_OFFSET, 
		3 * CONTROLS_OFFSET, 
		cx - 3 * CONTROLS_OFFSET - buttonRect.right, 
		cy - 4 * CONTROLS_OFFSET);
}