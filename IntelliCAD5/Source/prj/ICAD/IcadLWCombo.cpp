// IcadLWComboBox.cpp : implementation file
//

#include "icad.h"
#include "IcadLWCombo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIcadLWComboBox construction/destruction

/******************************************************************************
*+ CIcadLWComboBox::CIcadLWComboBox - constructor
*
*/

CIcadLWComboBox::CIcadLWComboBox()
: CComboBox()
	{
	}

/******************************************************************************
*+ CIcadLWComboBox::~CIcadLWComboBox - destructor
*
*/

CIcadLWComboBox::~CIcadLWComboBox()
	{
	}

/////////////////////////////////////////////////////////////////////////////
// CIcadLWComboBox operations

/******************************************************************************
*+ CIcadLWComboBox::FillLWeight
*
*/

void CIcadLWComboBox::FillLWeight(int lwunits, int celweight, int bitcode )
{
	int top, lweight, index;
	top = ( lwunits == 1 ? IDC_COMBO_M_LWDEFAULT : IDC_COMBO_I_LWDEFAULT );

	ResetContent();

	for(int i = 0 ; i < 27 ; i++ )
	{
		lweight = db_index2lweight( i - 3 );
		if( !db_is_validweight( lweight, bitcode ) )
			continue;
		index = AddString( ResourceString( top + i , ""));
		SetItemData( index, lweight );
		if( celweight == lweight )
			SetCurSel( index );
	}
}


BEGIN_MESSAGE_MAP(CIcadLWComboBox, CComboBox)
	//{{AFX_MSG_MAP(CIcadLWComboBox)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelChange)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIcadLWComboBox message handlers

void CIcadLWComboBox::OnSelChange()
{
	// update preview and other controls
	int lweight = GetItemData( GetCurSel());
	GetParent()->SendMessage( WM_LWEIGHT_CHANGED, lweight );
}

void CIcadLWComboBox::OnDestroy()
{
	CComboBox::OnDestroy();
}
