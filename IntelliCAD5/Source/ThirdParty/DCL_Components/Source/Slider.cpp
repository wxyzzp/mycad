/** FILENAME:     Slider.cpp- Contains implementation of class CSlider.
* Copyright  (C) Copyright 1998-99 Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract
*	this class Maps the basic slider tile
*	to windows control
*
*
*Audit Log
*
*
*Audit Trail
*
*
*/


#include "stdafx.h"
#include "Slider.h"
#include "sds.h"
#include "dclprivate.h"
#include "constants.h"
#include "Dialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/************************************************************************************
/* + Slider::Slider()
*
*	Construction
*
*
*
*/

Slider::Slider():nCurrentPos(0)
	{
	}
/************************************************************************************
/* + Slider::Slider(int Parent_layout)
*	Constructor.
*
*
*
*
*/

Slider::Slider(int Parent_layout)
	{
	m_ParentAlignment = Parent_layout;
	}


/************************************************************************************
/* + Slider::~Slider ()
*
*	Destruction
*
*
*
*/

Slider::~Slider()
	{
	}


BEGIN_MESSAGE_MAP(Slider, CScrollBar)
	//{{AFX_MSG_MAP(Slider)
	ON_WM_VSCROLL_REFLECT()
	ON_WM_HSCROLL_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/************************************************************************************
/* + int Slider:: CreateOnScreenObj (CWnd* pParent)
*
*	creates the windows UI control
*
*
*
*
*/

int Slider:: CreateOnScreenObject (
								   CWnd* pParent	// i:pointer to the parent window
								   )
	{
	int error;
	DWORD dwStyle = WS_CHILD|WS_VISIBLE|WS_GROUP;

	if (!m_attriblist->IsEnabled())
		{
		// set style to enable
		dwStyle = dwStyle|WS_DISABLED;
		}
	if (m_attriblist->IsTabstop())
		{
		// set style to tabstop
		dwStyle = dwStyle|WS_TABSTOP;
		}
	if(m_attriblist->get_Layout() == LAYOUT_VERTICAL)
		{
		dwStyle = dwStyle | SBS_VERT;
		}

	CRect rect = PositionControl(); // function to calculate the actiual position of control in the dialog
	error = Create(dwStyle, rect, pParent, m_controlID.GetID()); // creates the UI control

	if (error)
		{
		// set attributes of slider
		m_MaxVal = m_attriblist->get_MaxValue ();
		m_MinVal = m_attriblist->get_MinValue ();
		m_bigIncrement = m_attriblist->get_BigIncrement ();
		m_smallIncrement = m_attriblist->get_SmallIncrement();
		if (m_MaxVal > m_MinVal)
			{
			SetScrollRange(m_MinVal,m_MaxVal);
			SetScrollPos(m_MinVal);
			}
		else
			{
			SetScrollRange(m_MaxVal,m_MinVal);
			SetScrollPos(m_MaxVal);
			}


		// if the layout is vertical the slider works opposite to how it works in MFC
		if(m_attriblist->get_Layout () == LAYOUT_VERTICAL && m_MaxVal > m_MinVal )
			{
			SetScrollPos(m_MaxVal);
			}

		// {{{ Gurudev - DT: 7/2/99 4:21:09 PM
		if( m_attriblist->get_Layout () != LAYOUT_VERTICAL &&  m_MaxVal < m_MinVal	)
			{
			SetScrollPos(m_MinVal);
			}

		// Gurudev - Dt:  7/2/99 4:21:23 PM }}}


		long value = m_attriblist->get_ValueInt ();

		if( (value < m_MaxVal && value >m_MinVal) && value != SDS_RTERROR )
		{
		switch (m_attriblist->get_Layout())
			{
			case LAYOUT_HORIZONTAL:
				break;

			case LAYOUT_VERTICAL:
				// if vertical then the value is reversed
				value = m_MaxVal -value;

				break;
			}
		SetScrollPos(value); // set the scrolling position
		}

		EnableScrollBar(); // mfc function called to enable scrolling
		return SDS_RTNORM;
		}
	else
		return SDS_RSERR;
	}
/************************************************************************************
/* + CRect Slider:: PositionControl()
*
*	this function positions the slider depending on layout
*
*
*
*/

CRect Slider:: PositionControl()
	{

	CRect rect(0,0,0,0);
	int PosX, PosY, height, width;
	// depending upon the layout the height and width is desided
	switch (m_attriblist->get_Layout())
		{
		default:
		case LAYOUT_HORIZONTAL:
			// if layout is horizontal then height is fixed it is 16 and width changes depending upon
			// the the attribute IsFixedWidth .
			rect.bottom = height = 16 ;
			if (m_attriblist->IsFixedWidth())
				// if fixed width == true dont change the width
				rect.right = width = m_attriblist->get_Width();
			else
				// else change the width
				rect.right = width = m_DisplayArea.Width();
			break;

		case LAYOUT_VERTICAL:
			// if layout is vertical the width is fixed ie 22 its height changes
			// depending upon fixedheight;
			rect.right = width = 22 ;
			if (m_attriblist->IsFixedHeight())
				// if fixed height  == true dont change the height
				rect.bottom = height = m_attriblist->get_Height();
			else
				// change the height.
				rect.bottom = height = m_DisplayArea.Height() ;
			break;
		}


// the below code positions the  control the control in the display are
	// check for parent layout if horizontal then the control can
	// be moved top ,bottom or centered
	if(m_ParentAlignment == LAYOUT_HORIZONTAL )
		{
		// if the control is first control in  a container the it is left aligned
		if(nColumnTile == FIRSTTILE)
			{
			switch (m_attriblist->get_Alignment())
				{
				case ALIGN_DEFAULT:
				case ALIGN_CENTERED:
					PosX = m_DisplayArea.left ;
					PosY = m_DisplayArea.CenterPoint().y - (height / 2 );
					break;
				case ALIGN_BOTTOM:
					PosX = m_DisplayArea.left ;
					PosY = m_DisplayArea.bottom - height;
					break;
				default:
				case ALIGN_TOP:
					PosX = m_DisplayArea.left;
					PosY =	m_DisplayArea.top  ;
					break;
				}
			}
		// if the control is middle control in  a container the it is centered aligned
		else if(nColumnTile == MIDDLETILE)
			{
			switch (m_attriblist->get_Alignment())
				{
				case ALIGN_DEFAULT:
				case ALIGN_CENTERED:
					PosX = m_DisplayArea.CenterPoint().x - (width / 2 );
					PosY = m_DisplayArea.CenterPoint().y - (height / 2 );
					break;
				case ALIGN_BOTTOM:
					PosX = m_DisplayArea.CenterPoint().x - (width / 2 );
					PosY = m_DisplayArea.bottom - height;
					break;
				default:
				case ALIGN_TOP:
					PosX = m_DisplayArea.CenterPoint().x - (width / 2 );
					PosY =	m_DisplayArea.top  ;
					break;
				}
			}
		// if the control is last control in  a container the it is right aligned
		else if (nColumnTile == LASTTILE)
			{
			switch (m_attriblist->get_Alignment())
				{
				case ALIGN_DEFAULT:
				case ALIGN_CENTERED:
					PosX = m_DisplayArea.right - width;
					PosY = m_DisplayArea.CenterPoint().y - (height / 2 );
					break;
				case ALIGN_BOTTOM:
					PosX = m_DisplayArea.right - width;
					PosY = m_DisplayArea.bottom - height;
					break;
				default:
				case ALIGN_TOP:
					PosX = m_DisplayArea.right - width;
					PosY =	m_DisplayArea.top  ;
					break;
				}
			}


		}
	// check for parent layout if vertical then the control can
	// be moved right  ,left or centered
	else
		{
		// if the control is first control in  a container the it is top aligned
		if(nColumnTile == FIRSTTILE)
			{
			switch (m_attriblist->get_Alignment())
				{

				case ALIGN_RIGHT:
					PosX = m_DisplayArea.right - width;
					PosY = m_DisplayArea.top ;
					break;
				case ALIGN_CENTERED:
					PosX = m_DisplayArea.CenterPoint().x - (width / 2 );
					PosY = m_DisplayArea.top ;
					break;
				default:
				case ALIGN_LEFT:
					PosX = m_DisplayArea.left;
					PosY =	m_DisplayArea.top  ;
					break;
				}
			}
		// if the control is middle control in  a container the it is centered aligned
		else if(nColumnTile == MIDDLETILE)
			{
			switch (m_attriblist->get_Alignment())
				{

				case ALIGN_RIGHT:
					PosX = m_DisplayArea.right - width;
					PosY = m_DisplayArea.CenterPoint().y - (height / 2 );
					break;
				case ALIGN_CENTERED:
					PosX = m_DisplayArea.CenterPoint().x - (width / 2 );
					PosY = m_DisplayArea.CenterPoint().y - (height / 2 );
					break;
				default:
				case ALIGN_LEFT:
					PosX = m_DisplayArea.left;
					PosY =	m_DisplayArea.CenterPoint().y - (height / 2 );
					break;
				}
			}
		// if the control is last control in  a container the it is bottom aligned
		else if (nColumnTile == LASTTILE)
			{
			switch (m_attriblist->get_Alignment())
				{

				case ALIGN_RIGHT:
					PosX = m_DisplayArea.right - width;
					PosY = m_DisplayArea.bottom - height;
					break;
				case ALIGN_CENTERED:
					PosX = m_DisplayArea.CenterPoint().x - (width / 2 );
					PosY = m_DisplayArea.bottom - height;
					break;
				default:
				case ALIGN_LEFT:
					PosX = m_DisplayArea.left;
					PosY =m_DisplayArea.bottom - height;
					break;
				}
			}
		}
	// the actual co-ordinates are calculated here.
	rect.OffsetRect(CPoint(PosX,PosY));

	return rect;

	}


/************************************************************************************
/* + void Slider::VScroll(UINT nSBCode, UINT nPos -
*
*
*	MFC function to handle vertical scroll
*
*
*/

void Slider::VScroll(UINT nSBCode, UINT nPos)
	{
	// TODO: Add your message handler code here
	long lCalcPos;

    // Initialize the call back packet.
	sds_callback_packet *cpkt = NULL;
	cpkt = GetCallBackPacket();
	// Then , set the reason within the switch case.

	switch(nSBCode)
		{
		case SB_PAGEDOWN:
			// in page down move the thumb by Big increment
			nCurrentPos = GetScrollPos();
			lCalcPos = nCurrentPos + m_bigIncrement;
			SetScrollPos(lCalcPos);
			cpkt->reason = SDS_CBR_SELECT;
			break;
		case SB_PAGEUP:
			// in page up move the thumb by Big increment
			nCurrentPos = GetScrollPos();
			lCalcPos = nCurrentPos - m_bigIncrement;
			SetScrollPos(lCalcPos);
			cpkt->reason = SDS_CBR_SELECT;
			break;
		case SB_LINEDOWN :
			// in line down move the thumb by small increment
			nCurrentPos = GetScrollPos();
			lCalcPos = nCurrentPos + m_smallIncrement;
			SetScrollPos((int)lCalcPos);
			cpkt->reason = SDS_CBR_SELECT;
			break;
		case SB_LINEUP  :
			// in line down up the thumb by small increment
			nCurrentPos = GetScrollPos();
			lCalcPos = nCurrentPos - m_smallIncrement;
			SetScrollPos(lCalcPos);
			cpkt->reason = SDS_CBR_DRAG;
			break;
		case SB_THUMBPOSITION :
			// move the thumb to new position
			SetScrollPos(nPos);
			cpkt->reason = SDS_CBR_DRAG;
			break;
		case SB_THUMBTRACK :
			SetScrollPos(nPos);
			cpkt->reason = SDS_CBR_DRAG;
			// move the thumb to new position
			break;
		}

	Dialog * pParent = static_cast<Dialog *>(GetParent());
	if(pParent->m_IsModal )
	{
	// the below code is used to fill the callback packet which is sent in the call back function

	cpkt->dialog = ((Dialog*)GetParent())->handle;
	nCurrentPos = GetScrollPos();

	TRACE("%%%%CurrentPos:%d\n",nCurrentPos);

	if ( m_MaxVal > m_MinVal)
		{
		nCurrentPos = m_MaxVal - nCurrentPos;
		}


	CString szValue;
	szValue.Format ("%d",nCurrentPos);
	cpkt->value =(char*)((LPCTSTR) szValue);

	TRACE("CurrentPosString:%s\n",szValue);
	ScreenObject *pScreen =(ScreenObject *)this;
	CString strAction = pScreen->m_attriblist->get_Action();

	// call the call back function if call back is set
	// If the callback is not set by action_tile() then see whether the
	// "action" attribute has been specified in the dcl file.If not,
	// call the default callback handler.
	if(m_FunctionPtr != NULL)
		m_FunctionPtr(cpkt);
	else if ( strAction.IsEmpty() == FALSE )
		{
		pParent->m_DefCallBack(cpkt);
		}
	else if(pParent->m_DefCallBack != NULL)
		pParent->m_DefCallBack(cpkt);
	// delete the callback packet
	delete cpkt->tile;
	delete cpkt;

	}

	}

/************************************************************************************
/* + void Slider::HScroll(UINT nSBCode, UINT nPos -
*
*
*	MFC function to handle horizontal scroll
*
*
*/

void Slider::HScroll(UINT nSBCode, UINT nPos)
	{
	// TODO: Add your message handler code here

	long lCalcPos;

	// Initialize the call back packet.
	sds_callback_packet *cpkt = NULL;
	cpkt = GetCallBackPacket();
	// Then , set the reason within the switch case.


	switch(nSBCode)
		{
		case SB_LINELEFT :
			// in LINELEFT move the thumb by small increment
			nCurrentPos = GetScrollPos();
			lCalcPos = nCurrentPos - m_smallIncrement;
			SetScrollPos(lCalcPos);
			cpkt->reason = SDS_CBR_SELECT;
			break;
		case SB_LINERIGHT :
			// in LINERIGHT move the thumb by small increment
			nCurrentPos = GetScrollPos();
			lCalcPos = nCurrentPos + m_smallIncrement;
			SetScrollPos((int)lCalcPos);
			cpkt->reason = SDS_CBR_SELECT;
			break;
		case SB_PAGELEFT :
			// in PAGELEFT move the thumb by Big increment
			nCurrentPos = GetScrollPos();
			lCalcPos = nCurrentPos - m_bigIncrement;
			SetScrollPos(lCalcPos);
			cpkt->reason = SDS_CBR_SELECT;
			break;
		case SB_PAGERIGHT :
			// in PAGERIGHT move the thumb by Big increment
			nCurrentPos = GetScrollPos();
			lCalcPos = nCurrentPos + m_bigIncrement;
			SetScrollPos(lCalcPos);
			cpkt->reason = SDS_CBR_SELECT;
			break;
		case SB_THUMBPOSITION :
			// move the thumb to new position
			SetScrollPos(nPos);
			cpkt->reason = SDS_CBR_DRAG;
			return;
			break;
		case SB_THUMBTRACK :
			// move the thumb to new position
			SetScrollPos(nPos);
			cpkt->reason = SDS_CBR_DRAG;
			break;
		default:
			return;
		}
	Dialog * pParent = static_cast<Dialog *>(GetParent());
	if(pParent->m_IsModal )
	{
	// the below code is used to fill the callback packet which is sent in the call back function

	cpkt->dialog = ((Dialog*)GetParent())->handle;

	nCurrentPos = GetScrollPos();

	if ( m_MaxVal < m_MinVal )
		{
		nCurrentPos =  m_MinVal - nCurrentPos + m_MaxVal;
		}

	CString szValue;
	szValue.Format ("%d",nCurrentPos);
	cpkt->value =(char*)((LPCTSTR) szValue);
	ScreenObject *pScreen =(ScreenObject *)this;
	CString strAction = pScreen->m_attriblist->get_Action();

	// call the call back function if call back is set
	// If the callback is not set by action_tile() then see whether the
	// "action" attribute has been specified in the dcl file.If not,
	// call the default callback handler.

	try
		{
		if(m_FunctionPtr != NULL)
			m_FunctionPtr(cpkt);
		else if ( strAction.IsEmpty() == FALSE )
			{
			pParent->m_DefCallBack(cpkt);
			}
		else if(pParent->m_DefCallBack != NULL)
			pParent->m_DefCallBack(cpkt);
		// delete the callback packet
		delete cpkt->tile;
		delete cpkt;
		}
	catch(...)
		{
		TRACE(_T("Slider callback failed..."));
		return;
		}
	}

	}

/************************************************************************************
/* + int Slider :: DimensionsTile (int * nWidthLess1,int * nHeightLess1)
*
*	Returns the size of the control
*
*
*
*/

int Slider :: DimensionsTile ( // Returns the size of the control
		int * nWidthLess1,		 // o:width of the Tile
		int * nHeightLess1		 // o:height of the Tile
		)
	{
	CRect rect;
	rect = PositionControl();
	*nWidthLess1= rect.Width ()-1;
	*nHeightLess1 = rect.Height()-1;
	return SDS_RTNORM;
	}

/************************************************************************************
/* + int Slider :: ModeTile (int nTileState)
*	Executes functions like setting focus to a control , enabling or disabling a control etc.
*	nTileState = 0  then enable window
*	nTileState = 1	disable window
*	nTileState = 2  set Focus
*
*
*
*/

int Slider :: ModeTile ( // Executes functions like setting focus to a control , enabling or disabling a control etc.
		int nTileState	   // i:state of control
		)
	{
	CWnd * pWind;
	pWind = (CWnd *)(this);

	switch(nTileState)
		{
		case SDS_MODE_ENABLE:
			pWind->EnableWindow();
			break;
		case SDS_MODE_DISABLE:
			pWind->EnableWindow(FALSE);
			break;
		case SDS_MODE_SETFOCUS:
			pWind->SetFocus();
			break;
		default:
			TRACE ("WRONG PARAMETER IN MODE TILE");
			break;
		}
	return SDS_RTNORM;
	}


/************************************************************************************
/* + int Slider::SetTile (	CString szControlKey, CString szControlInfo)
*
*	Sets the "value" attribute of the tile
*
*
*
*/

int Slider::SetTile (		// Sets the "value" attribute of the tile
		CString szControlKey,	// i:Key of the tile
		CString szControlInfo	// i:Data to store in the value attribute
		)
	{
	long value ;
	value = atoi((LPCTSTR)szControlInfo);
	// check for the value is in range if in range calculate the actual value
	// depending upon layout
//4M Item:12->
/*
	if(value < m_MaxVal && value >m_MinVal)
*/
	if(value <= m_MaxVal && value >=m_MinVal)
//<-4M Item:12
        {
		switch (m_attriblist->get_Layout())
			{
			case LAYOUT_HORIZONTAL:
				break;

			case LAYOUT_VERTICAL:
				value = m_MaxVal -value;

				break;
			}
		SetScrollPos(value); // set the thumb to new value
		}
	return SDS_RTNORM;
	}

/************************************************************************************
/* + int Slider:: GetTile (	CString & szControlInfo,int lInfoMaxLength)
*
*	// Returns the "value" attribute of the tile
*
*
*
*/

int Slider:: GetTile (		// Returns the "value" attribute of the tile
		CString & szControlInfo,// o:Date contaned in value attribute
		int lInfoMaxLength	// o: Size of data
		)
	{

	if(lInfoMaxLength>0)
		{
		// set the position of the thumb and convert to proper value
		// depending upon layout
		switch (m_attriblist->get_Layout())
			{
			case LAYOUT_HORIZONTAL:
				nCurrentPos = GetScrollPos();

				if ( m_MaxVal < m_MinVal )
					{
					nCurrentPos =  m_MinVal - nCurrentPos + m_MaxVal;
					}

				break;

			case LAYOUT_VERTICAL:
				nCurrentPos = GetScrollPos();

				if ( m_MaxVal > m_MinVal)
					{
					nCurrentPos = m_MaxVal -nCurrentPos;
					}


				break;
			}

		szControlInfo.Format("%d",nCurrentPos);
		return SDS_RTNORM ;
		}
	return SDS_RTERROR ;
	}
/************************************************************************************
/* + int Slider::InitFocus -
*
*	sets the focus to the control
*
*
*
*/

int Slider::InitFocus()
{
	SetFocus();
	return SDS_RTNORM;
}
