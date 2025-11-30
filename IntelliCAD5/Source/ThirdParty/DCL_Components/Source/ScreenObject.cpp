/** FILENAME:     ScreenObject.cpp- Contains implementation of class CScreenObject.
* Copyright  (C) Copyright 1998-99  Visio Corporation.  All Rights Reserved.
*
*
*
* Abstract 
*	this class is the base class for all the basic tiles
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
#include "sds.h"
#include "ScreenObject.h"
#include "Constants.h"
#include "TileAttributes.h"
#include "Button.h"
#include "dclprivate.h"
#include "ListOfAttributes.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/************************************************************************************
/* + ScreenObject::ScreenObject - 
*
*	default constructor
*
*
*
*/

ScreenObject::ScreenObject() :m_DisplayArea(0,0,0,0),m_FunctionPtr(NULL),m_ClientData(NULL),nWhichItem(-1),nChangeAddNew(-1),ListStarted(0),m_nIsFirstRadioBtn(0),m_nCheckThisRadioBtn(0),hRadioToRemoveCheck(NULL),m_nActionSetThruParent(0),
                              m_IsCancelBtn(FALSE)
	{
	try 
		{
		m_ComponentFont = new CFont; // create font object
		}
	catch (...)
	{
		TRACE("new font failed in ScreenObject");
	}
	

	}
/************************************************************************************
/* + ScreenObject::~ScreenObject - 
*
*	Default Destructor
*
*
*
*/

ScreenObject::~ScreenObject()
{
	if (m_ComponentFont != NULL)
	{
		delete m_ComponentFont; // delete the font object created in constructor
		m_ComponentFont = NULL;
	}

}

/************************************************************************************
/* + int ScreenObject::ActionTile(void *pfnControlFun - 
*
*	Stores the pointer to the call back  function
*	
*
*
*/

int ScreenObject::ActionTile(
							 CLIENTFUNC pfnControlFunc // i:Valid callback function pointer
							 )
	{
	ASSERT(pfnControlFunc);
	// set the callback function
	if(pfnControlFunc != NULL)
		m_FunctionPtr = pfnControlFunc; 

	m_nActionSetThruParent = 0;

	TreeNode *pParentNode = this->m_attriblist->m_pTile->m_pTreeNode;

	ASSERT(pParentNode);

	int index = pParentNode->GetChildCount(); 

	for ( int iLoop = 0; iLoop < index; iLoop++ )
			{
			TreeNode* pTreeNodeHolder = pParentNode->GetChild(iLoop);
			ASSERT(pTreeNodeHolder);
			ScreenObject* pScreenObjectHolder = pTreeNodeHolder->m_Tile.get_ScreenObject();
			ASSERT(pScreenObjectHolder);

			// Set the action for child tiles.
            if ( pScreenObjectHolder->m_FunctionPtr  == NULL )
				{
				pScreenObjectHolder->m_FunctionPtr = pfnControlFunc;
				pScreenObjectHolder->m_nActionSetThruParent = 1;
				}

			}
	
	return SDS_RTNORM;
	}

/************************************************************************************
/* + int ScreenObject::ClientDataTile(void *pApplicationDat - 
*
*	Stores a variable as the data specific to a tile 
*	i:Data to store
*
*
*/

int ScreenObject::set_ClientDataTile(
								 void *pApplicationData // i:Data to store
								 )
	{
	ASSERT(pApplicationData);
	// sets usere defined data to a tile
//MB 28-12-1999
	m_ClientData = pApplicationData;

//	char* strClientData = new char[strlen((char *)pApplicationData) + 1];
//	strcpy(strClientData,(char *)pApplicationData);
//	m_ClientData = (void*)strClientData;
	//m_ClientData = (void*)(LPCTSTR)CString((char*)pApplicationData);
	return SDS_RTNORM;
	}

/************************************************************************************
/* + int ScreenObject::AddList(CString szAddToLis - 
*
*	Adds a new string in the list
*	
*
*
*/

int ScreenObject::AddList(
						  CString szAddToList // i:String to add
						  )
	{
	// This function will be implemented in the list  and popup list class only
	return SDS_RTERROR;
	}



/************************************************************************************
/* + int ScreenObject::GetKeyString(CString szKeyString, long* lInfoMaxLengt - 
*
*	Returns the Key maped to The Tile
*	
*	
*
*/

int ScreenObject::GetKeyString(
							   CString & szKeyString, // o:String to hold the Key
							   long* lInfoMaxLength   // o:length of the string
							   )
	{
	szKeyString = m_controlID.GetKey();
	*lInfoMaxLength = szKeyString.GetLength();

	return SDS_RTNORM;
	}
/************************************************************************************
/* + int ScreenObject::GetListString(LPTSTR szListString, int nStringLength, int nWhichIte - 
*
*	Gets the String from the ListBox
*	
*	
*	
*/

int ScreenObject::GetListString(
								CString& szListString, // o:Parameter to hold the return string
								int* nStringLength,	   // o:Size of the String
								int nWhichItem		   // i:item index in the listbox whose string is returned
								)
	{
	// This function will be implemented in the list and popup list classes
	return SDS_RTERROR;
	}

/************************************************************************************
/* + int ScreenObject::VectorImage(int nXVectorStart,int nYVectorStart,int nXVectorEnd,int nYVectorEn - 
*
*	Draws a vector image on image or image_button 
*	
*	
*	
*	
*/

int ScreenObject::VectorImage(
							  int nXVectorStart,	// i:Starting x position of Vector
							  int nYVectorStart,	// i:Starting y position of Vector
							  int nXVectorEnd,		// i:Ending x position of Vector
							  int nYVectorEnd		// i:Ending x position of Vector
							  )
	{
	// This function will be implemented in the image and image button  base class
	return SDS_RTNORM;
	}
/************************************************************************************
/* + int ScreenObject::FillImage(int nULCornerX, int nULCornerY, int nWidthLess1, int nHeightLess1, int nColo - 
*
*	Fills the image or image_button with the specified color.
*	
*	
*	
*	
*	
*/

int ScreenObject::FillImage(
							int nULCornerX, // i:X position of the image on the control
							int nULCornerY, // i:Y position of the image on the control
							int nWidthLess1, // i:Width of the image
							int nHeightLess1, // i:Height of Image
							int nColor		 // i:Color to fill
							)
	{
	// This function will be implemented in the image and image button  base class
	return SDS_RTNORM;
	}
/************************************************************************************
/* + int ScreenObject::SlideImage(int nULCornerX, int nULCornerY, int nWidthLess1, int nHeightLess1, LPTSTR wszSnapshotNam - 
*
*	Opens the .SLD or .SLB specified file and Draws the image to image control or image_button
*	
*	
*	
*	
*	
*/

int ScreenObject::SlideImage(
							 int nULCornerX,	// i:X position of the image on the control
							 int nULCornerY,	// i:Y position of the image on the control
							 int nWidthLess1,	// i:Width of the image
							 int nHeightLess1,	// i:Height of Image
							 CString wszSnapshotName // i:Snap Shot name of image
							 )
	{
	// This function will be implemented in the image and image button  base class
	return SDS_RTNORM;
	}

/************************************************************************************
/* + CRect ScreenObject::GetDisplayRect - 
*
*	this method returns the member  m_DisplayArea
*
*
*
*/

CRect ScreenObject::GetDisplayRect()
	{
	return m_DisplayArea;
	}

/************************************************************************************
/* + CWnd* ScreenObject::GetWindow - 
*
*	returns the window of Screenobject
*
*
*
*/

CWnd* ScreenObject::GetWindow()
	{
return NULL;

	}
/************************************************************************************
/* + CRect ScreenObject::PositionControl - 
*
*	Positions the control in the display area
*
*
*
*/

CRect ScreenObject::PositionControl()
	{
	CRect rect(0,0,0,0);
	int PosX, PosY, height, width;
	// check for IsFixedHeight attribute is given true then take height from
	// list of attributes else make height equal to displayarea
	if (m_attriblist->IsFixedHeight())
		{
		rect.bottom = height = m_attriblist->get_Height();
		}

	else
		{
		rect.bottom = height = m_DisplayArea.Height();
		}
	// check for IsFixedWidth attribute is given true then take width from
	// list of attributes else make width equal to displayarea
	if (m_attriblist->IsFixedWidth())
		{
		rect.right = width = m_attriblist->get_Width();
		}

 	else
		{
		rect.right = width = m_DisplayArea.Width();
		}

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
				case ALIGN_DEFAULT:
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
				case ALIGN_DEFAULT:
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
				case ALIGN_DEFAULT:
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
/* + int ScreenObject::SetRectForDisplay(long lLeft, long lTop, long lWidth, long lHeigh - 
*	Sets the DisplayRect member.
*
*
*
*
*/

int ScreenObject::SetRectForDisplay(				// Set the DisplayRects width and height.
									long lLeft,		// i : Left.
									long lTop,		// i : Top.
									long lWidth,	// i : Width.
									long lHeight	// i : Height.
									)
	{
	m_DisplayArea.SetRect(lLeft,lTop,lWidth + lLeft,lHeight + lTop);
	return SDS_RTNORM;
	}

/************************************************************************************
/* + int ScreenObject::set_ColumnTile(int nColumnTil - 
*	Set the ColumnTile.It could be FIRSTTILE
*	FIRSTTILE - First tile in the column.
*	LASTTILE - Last tile in the column.
*	MIDDLETILE - Any middletile in the column.
*	This flag is checked in the Create method for the controls.
*/

int ScreenObject::set_ColumnTile(int nValue)
	{
nColumnTile = nValue;
return SDS_RTNORM;
	}

/************************************************************************************
/* + int ScreenObject::get_ColumnTile - 
*	Returns the ColumnTile value.
*
*
*
*
*/

int ScreenObject::get_ColumnTile()
	{
	return nColumnTile;
	}







/************************************************************************************
/* + int ScreenObject::set_WidthToSet(int nWidthToSet)
*	Sets the m_nDisplayWidthToSet member.
*	
*
*
*
*/

int ScreenObject::set_WidthToSet(					// Sets the m_nDisplayWidthToSet member to the specified width.
								 int nWidthToSet	// i : Width specified.
								 )
	{
	ASSERT(nWidthToSet);
	m_nDisplayWidthToSet = nWidthToSet;
	return SDS_RTNORM;
	}
/************************************************************************************
/* + int ScreenObject::set_HeightToSet(int nHeightToSet)
*	Sets the m_nDisplayHeightToSet member.
*
*
*
*
*/

int ScreenObject::set_HeightToSet(					// Sets the m_nDisplayHeightToSet member to the specified height.
								  int nHeightToSet	// i : Height specified.
								  )
	{
	ASSERT(nHeightToSet);
	m_nDisplayHeightToSet = nHeightToSet;
	return SDS_RTNORM;
	}
/************************************************************************************
/* + int ScreenObject::get_HeightToSet() 
*	Get the m_nDisplayHeightToSet to set the height of the displayRect.
*
*
*
*
*/

int ScreenObject::get_HeightToSet()					// Get the m_nDisplayHeightToSet to set the height of the displayRect.
	{
	ASSERT(m_nDisplayHeightToSet);
	return m_nDisplayHeightToSet;
	}

/************************************************************************************
/* + int ScreenObject::get_WidthToSet()
*	Get the m_nDisplayWidthToSet to set the width of the displayRect.
*
*
*
*
*/

int ScreenObject::get_WidthToSet()					// Get the m_nDisplayWidthToSet to set the width of the displayRect.
	{
	ASSERT(m_nDisplayWidthToSet);
	return m_nDisplayWidthToSet;
	}
/************************************************************************************
/* + CString ScreenObject::SetMnemonic - 
*
*	this function set a mnemonic with a control
*
*
*
*/

CString ScreenObject::SetMnemonic()
{
	CString Mnemonicstring = m_attriblist->get_Mnemonic();
	TCHAR Mnemonic= (TCHAR)254;
	Mnemonicstring.TrimLeft ();
	if(!Mnemonicstring.IsEmpty ())
		Mnemonic = Mnemonicstring[0];
	CString Label = m_attriblist->get_Label ();
	// search for the char and add an '&'  before the char
	if ( m_attriblist->IsTabstop() )
		{
		if(!Mnemonicstring.IsEmpty ())
			{
			if(!Label.IsEmpty ())
				{
				int index = Label.Find (Mnemonic);	
				if(index>-1)
				Label.Insert ((index),"&");
				}
			}
		}
		
	return Label ;

}


int ScreenObject::GetAttribute ( // Gets the Value of the attribute from the tile
		CString szAttribName,		 // i:Attribute name
		CString& szAttribInfo,		 // o:Value of the attribute corresponding to the name
		int lInfoMaxLength         // o:string length of the value
		)
	{
	 CObArray & pAttrib = m_attriblist->get_ListAttributes();
	 for ( int index = 0; index < pAttrib.GetSize(); index++ )
		{
		TileAttributes * pTileAttrib = (TileAttributes *)pAttrib[index];
		if ( pTileAttrib->get_AttributeName() == szAttribName)
			{
			szAttribInfo = pTileAttrib->get_AttributeValue();
			return SDS_RTNORM;
			}
		else if(szAttribName == CString("is_enabled"))
		{
		szAttribInfo = CString("true");
		}
		
		}
	 return SDS_RTERROR;
	}



/************************************************************************************
/* + int ScreenObject:: DimensionsTile ( int * nWidthLess1,int * nHeightLess1)
*
*	Returns the size of the contr - 
*
*
*/

int ScreenObject:: DimensionsTile ( // Returns the size of the control
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
/* + int ScreenObject::ModeTile ( int nTileState)
*
*
*	 Executes functions like setting focus to a control , enabling or disabling a control etc
*
*
*/

int ScreenObject::ModeTile ( // Executes functions like setting focus to a control , enabling or disabling a control etc.
		int nTileState	   // i:state of control
		)
	{
	return SDS_RTERROR;
	}
/************************************************************************************
/* + int ScreenObject ::SetTile (		
*
*	Sets the "value" attribute of the tile
*
*
*
*/

int ScreenObject ::SetTile (		// Sets the "value" attribute of the tile
		CString szControlKey,	// i:Key of the tile
		CString szControlInfo	// i:Data to store in the value attribute
		)
	{
	ValueString = szControlInfo;
	return SDS_RTNORM;
	}

/************************************************************************************
/* + sds_callback_packet * ScreenObject::GetCallBackPacket - 
*
*
*	creates the default callback[packet
*
*
*/

sds_callback_packet * ScreenObject::GetCallBackPacket()
{
	sds_callback_packet *cpkt = NULL;
	try
	{
	cpkt = new sds_callback_packet;
//MB 28-12-1999
//	if ( !m_ClientData )
//		cpkt->client_data = _T("");
//	else
		cpkt->client_data = m_ClientData ;
	// {{{ Gurudev
	/*		
	LISP_DCL_DATA *lspDataStruct = new LISP_DCL_DATA ;
	lspDataStruct->key = NULL;
	lspDataStruct->action = NULL;
	lspDataStruct->key = m_attriblist->get_Key();
	lspDataStruct->action = NULL;//m_attriblist->get_Action();
	CString action = m_attriblist->get_Action();
	action.TrimLeft();
	action.TrimRight();
	if (action.IsEmpty() == FALSE )
		lspDataStruct->action = (LPCTSTR)action;
	cpkt->client_data = lspDataStruct ;*/
            // }}}
	//cpkt->tile = static_cast<ScreenObject*>(this); 
	_sds_htile* htile = new _sds_htile();
	htile->p = static_cast<ScreenObject*>(this);
	cpkt->tile = htile;

	cpkt->reason = -1;
	cpkt->value = NULL;
	CRect rect = PositionControl();
	cpkt->x = rect.left ;
	cpkt->y = rect.top ;
	}
	catch(...)
	{
		DWORD dw = ::GetLastError();
		TRACE("%d",dw);
		TRACE("CALL BACK PACKET IN BUTTON FAILED");
	}
	
	return cpkt;


}
/************************************************************************************
/* + int ScreenObject::InitFocus - 
*
*	virtual function implemented in base classes , which sets the focus on the control
*
*
*
*/

int ScreenObject::InitFocus()
{
return  SDS_RTERROR;
}

/************************************************************************************
/* + int ScreenObject::ExecLispExpr(char *vpszLispExp, sds_callback_packet *pkt)
*	Executes the lisp expression which was
*	set in the dcl file for "action" attribute
*	for a particular tile.It also sets the values for the
*	following symbols & puts them on the stack.
*	$key,$value,$data,$reason,$x,$y.
*/

int ScreenObject::ExecLispExpr(								// Execute the lisp expression in the dcl file.
							   char *vpszLispExp,			// i : String representing the lisp expression.
							   sds_callback_packet *pkt		// i : Callback packet.
							   )
	{

	// This function gets executed  on;y when the project is a DLL

	#ifndef _TESTEXE

	try
		{

		// typedef all the function's used. 
		typedef struct sds_resbuf * (* funcptr_sds_newrb)(int nTypeOrDXF);
		typedef void  * (* funcptr_sds_malloc)(size_t sizeBytes);	
		typedef int	(* funcptr_sds_putsym)(const char *szSymbol, struct sds_resbuf *prbSymbolInfo);
		typedef int (* funcptr_sds_relrb ) (struct sds_resbuf *prbReleaseThis);
		typedef int (*funcptr_sds_command)(int nRType, ...);

		funcptr_sds_newrb sds_newrb;
		funcptr_sds_malloc sds_malloc; 
		funcptr_sds_putsym sds_putsym;
		funcptr_sds_relrb sds_relrb;
		funcptr_sds_command sds_command;


		// Get the module handle
		HMODULE   hmodule = GetModuleHandle(NULL);
		ASSERT(hmodule);
		if(hmodule)
			{
			sds_newrb = (funcptr_sds_newrb)GetProcAddress(hmodule,"sds_newrb");
			ASSERT(sds_newrb);
			sds_malloc = (funcptr_sds_malloc)GetProcAddress(hmodule,"sds_malloc");
			ASSERT(sds_malloc );
			sds_putsym = (funcptr_sds_putsym )GetProcAddress(hmodule,"sds_putsym");
			ASSERT(sds_putsym );
			sds_relrb = (funcptr_sds_relrb )GetProcAddress(hmodule,"sds_relrb");
			ASSERT(sds_relrb );
			sds_command = (funcptr_sds_command )GetProcAddress(hmodule,"sds_command");
			ASSERT(sds_command );
			}
		
		struct resbuf *buffer=sds_newrb(RTSTR);
		ASSERT(buffer);
		buffer->resval.rstring=(char *)sds_malloc(512 * sizeof(TCHAR)); 	
		ScreenObject * pScreen = (ScreenObject*)this;
		// $key
		_tcscpy(buffer->resval.rstring,pScreen->m_attriblist->get_Key());
		sds_putsym("$key",buffer); // Set the symbol.

		// $value
		if (pkt->value != NULL)
		{	
			_tcscpy(buffer->resval.rstring,pkt->value);
			sds_putsym("$value",buffer);
		}
		// $data
		if ((char *)pkt->client_data != NULL)
		{	
			_tcscpy(buffer->resval.rstring,(char *)pkt->client_data);
			sds_putsym("$data",buffer);	
		} 
		// Release  the buffer
		sds_relrb(buffer);
		
		buffer = sds_newrb(RTLONG);
		// $reason
		buffer->resval.rint = pkt->reason;
		sds_putsym("$reason",buffer);
		// $x
		buffer->resval.rlong = pkt->x;
		sds_putsym("$x",buffer);
		// $y
		buffer->resval.rlong = pkt->y;
		sds_putsym("$y",buffer);
		// Release the buffer.
		sds_relrb(buffer);
		// Wrap lisp expression with "(progn" & ")".
		char *FinalLispExpr = new char[_tcslen(vpszLispExp) + 11] ;
		
		//strcpy(FinalLispExpr,"(progn");
		_tcscpy(FinalLispExpr,_T("(progn"));
		_tcscat(FinalLispExpr,vpszLispExp);
		_tcscat(FinalLispExpr,_T(")"));
		
		// Execute the lisp func. / expression.
		int test = sds_command(RTSTR,FinalLispExpr,RTNONE);
		
		delete [] FinalLispExpr;
		if ( test )
			return test;
		else
			return SDS_RTERROR; 
		}
	catch(...)
		{
		TRACE("Exception:Error while executing lisp expression thru sds_command\n");
		return SDS_RTERROR;
		}

	#endif

	return SDS_RTNORM;
	}
/************************************************************************************
/* + int ScreenObject::get_CheckVar()
*	Get the value of m_nCheckThisRadioBtn.
*
*
*
*
*/	

int ScreenObject::get_CheckVar()
	{
	return m_nCheckThisRadioBtn;
	}
/************************************************************************************
/* + int ScreenObject::set_CheckVar(int nState)
*	Sets the m_nCheckThisRadioBtn variable.If this variable is set to 
*	1 then check this radio button after creation.If this value is 0
*	then do not set the check state even if it is specified in the 
*	list of attributes.
*
*/	
int ScreenObject::set_CheckVar(				// Set m_nCheckThisRadioBtn value to the specified value.
							   int nState	// i : Specified value.
							   )
	{
	m_nCheckThisRadioBtn = nState;
	return SDS_RTNORM;
	}


/************************************************************************************
/* + TreeNode * ScreenObject::GetParentNodeOfTile ()
*	Returns the parent's TreeNode.
*
*
*
*
*/	

TreeNode * ScreenObject::GetParentNodeOfTile()
	{
     
	 TreeNode *pParentNode = this->m_attriblist->m_pTile->m_pTreeNode->get_Parent();  
     ASSERT(pParentNode);
	 return pParentNode;

//	 pParentNode->m_Tile.get_ScreenObject()->hRadioToRemoveCheck = ( ;  

	}




/************************************************************************************
/* + int ScreenObject::set_Cancel(BOOL bCancel) - 
*
*   Sets the m_IsCancelBtn property.
*
*
*
*/
int ScreenObject::set_Cancel(             // Sets the m_IsCancelBtn property.
							 BOOL bCancel // i: Flag to set for cancel .
							 )
	{
     
	m_IsCancelBtn = bCancel;

	return SDS_RTNORM; 


	}
