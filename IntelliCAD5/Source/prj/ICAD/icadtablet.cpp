/* ICAD\ICADTABLET.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Digitizer information for ICAD.  Implementation of CIcadTablet class.
 * 
 */ 

// ** Includes
#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "IcadTablet.h"/*DNT*/
#include "TabletDef.h"/*DNT*/

// the following include statements are needed to support GEO
#define THREE_D
#include "Geometry.h"/*DNT*/
#include "Point.h"/*DNT*/
#include "Trnsform.h"/*DNT*/
#include "Preferences.h"

void sds_point_to_C3Point(const sds_point & A, C3Point *B);
void CAffine_to_sds_matrix(const CAffine & A, sds_matrix B);

LONG IcadRegDeleteKey(HKEY hKey, LPCTSTR lpSubKey);  //defined in icadmenu.cpp
BOOL ExtractPointFromCString (CString String, sds_point pt);  //define in icadexplorer.cpp




//CIcadTablet Class Implementation
CIcadTablet::CIcadTablet()
	{
	m_TabletCmdList = NULL;
	m_pWinTabSys = NULL;
	m_pWinTabTab = NULL;
	m_pWinTabRaw = NULL;
	m_pWinTabCal = NULL;
	m_TabletCmdList = NULL;

	m_bInitialized = FALSE;
	m_bCalModified = FALSE;
	m_bCfgModified = FALSE;
	m_bButModified = FALSE;

	m_bCalibrated = FALSE;
	m_bConfigured = FALSE;
	}

CIcadTablet::~CIcadTablet()
	{
	if (m_pWinTabSys) 
		{
		m_pWinTabSys->Close();
		delete m_pWinTabSys;
		m_pWinTabSys = NULL;
		}
	if (m_pWinTabTab) 
		{
		m_pWinTabTab->Close();
		delete m_pWinTabTab;
		m_pWinTabTab = NULL;
		}
	if (m_pWinTabRaw) 
		{
		m_pWinTabRaw->Close();
		delete m_pWinTabRaw;
		m_pWinTabRaw = NULL;
		}
	if (m_pWinTabCal) 
		{
		m_pWinTabCal->Close();
		delete m_pWinTabCal;
		m_pWinTabCal = NULL;
		}
	if (m_TabletCmdList)
		{
		delete []m_TabletCmdList;
		m_TabletCmdList = NULL;
		}
	}


BOOL CIcadTablet::Initialize (HWND hWnd, char* pError, UINT nSize)
	{
	// Activate tablet data message loop - create a new thread to handle tablet input
	//We have to create a separate thread for the tablet.  Trying to hookup the tablet to the
	//main UI thread was not very successful.  Certain events like button down on the title bar
	//and rightclick on toolbar dock area, are not processed by the main UI thread.  They seem
	//to be handled by the system/MFC somehow.  So we need a Tablet thread that will simply handle
	//all input.  This thread creates a window that is not an MFC window, and then looks at its
	//message loop.  

//	DWORD dwThreadId;
//	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TabletMsgLoop, NULL, 0, &dwThreadId);

	//return FALSE right away if a new CWinTab failed i.e. Wintab dlls were not found.

	if (m_bInitialized)
		return FALSE;

	m_bInitialized = TRUE;

	//return FALSE right away if a new CWinTab failed i.e. Wintab dlls were not found.
    // Open WinTab API. Exit on failure

    CWinTab* pWinTab = new CWinTab();
    if ( !pWinTab || pWinTab->GetLastError(pError, nSize) )
		{
		delete pWinTab;
        return FALSE;
		}

    // Get general WinTab info

    pWinTab->GetDeviceName(m_DeviceName, sizeof(m_DeviceName));
    pWinTab->GetTabletSize(&m_TabletSize);

	//if the info was not in the registry, or reading it failed, restore defaults.
	if (!ReadFromRegistry())
		SetDefaults();

    // Although we no longer need pWinTab at this time, we'll
    // wait deleting it until the context class instances are
    // created, to avoid premature unloading of the WinTab DLL.

    m_pWinTabTab = new CWinTabCtx(FALSE);
    m_pWinTabSys = new CWinTabCtx(TRUE);
    m_pWinTabRaw = new CWinTabCtx(FALSE);
    m_pWinTabCal = new CWinTabCtx(FALSE);
    delete pWinTab;

    // Validate contexts we've created so far

    if ( !m_pWinTabTab || m_pWinTabTab->GetLastError(pError, nSize) ||
         !m_pWinTabSys || m_pWinTabSys->GetLastError(pError, nSize) ||
         !m_pWinTabRaw || m_pWinTabRaw->GetLastError(pError, nSize) ||
         !m_pWinTabCal || m_pWinTabCal->GetLastError(pError, nSize) )
        return FALSE;

	// ***********************************
	//Setup the Tablet Context
	// Set tablet context name
	m_pWinTabTab->SetCtxName(TABLETCTXNAME);

	// Bugzilla :78297 Submission by Bruce Scott
	// We want to receive WT_PACKET messages (off by default)
	m_pWinTabTab->SetOptions(m_pWinTabTab->GetOptions() | CXO_MESSAGES);

	// We want this context to return x, y and button info
	m_pWinTabTab->SetPktData(PK_X | PK_Y | PK_BUTTONS | PK_CURSOR);

	// Send us movement and all button events. Eventhough we're
	// probably only interested in button downs, we still want 
	// to get the movement and button up events. Otherwise, they
	// may end up in the next context in the overlap order and
	// who knows what that's going to do with it.

	m_pWinTabTab->SetMoveMask(PK_X | PK_Y);
	m_pWinTabTab->SetBtnDnMask(0xFFFFFFF);
	m_pWinTabTab->SetBtnUpMask(0xFFFFFFF);

	// ***********************************
	//Setup the System Context
	// Set system context name
	m_pWinTabSys->SetCtxName(SYSTEMCTXNAME);

	// We want to receive WT_PACKET messages (off by default for system contexts)
	m_pWinTabSys->SetOptions(m_pWinTabSys->GetOptions() | CXO_MESSAGES);

	// We want this context to return x, y and button info
	m_pWinTabSys->SetPktData(PK_X | PK_Y | PK_BUTTONS | PK_CURSOR);

	// X,Y should return absolute screen coordinates
	m_pWinTabSys->SetPktMode(0);

	// Send us events for any buttons that are not used as system buttons. 
	m_pWinTabSys->SetBtnDnMask(0xFFFFFFF);
	m_pWinTabSys->SetBtnUpMask(0xFFFFFFF);

	// Set the input area for the screen
	//RECT rctSystem;
	//GetScreenSize (&rctSystem);
	//m_pWinTabSys->SetInputArea(&rctSystem);


	// ***********************************
	//Setup the Raw Context
	// Set tablet context name
	m_pWinTabRaw->SetCtxName(RAWCTXNAME);

	// Bugzilla :78297 Submission by Bruce Scott
	// We want to receive WT_PACKET messages (off by default)
	m_pWinTabRaw->SetOptions(m_pWinTabRaw->GetOptions() | CXO_MESSAGES);

	// We want this context to return x, y and button info
	m_pWinTabRaw->SetPktData(PK_X | PK_Y | PK_BUTTONS | PK_CURSOR);

	// Send us movement and all button events.
	m_pWinTabRaw->SetMoveMask(PK_X | PK_Y);
	m_pWinTabRaw->SetBtnDnMask(0xFFFFFFF);
	m_pWinTabRaw->SetBtnUpMask(0xFFFFFFF);


	// ***********************************
	//Setup the Calibrated Context
	// Set tablet context name
	m_pWinTabCal->SetCtxName(CALIBCTXNAME);

	// Bugzilla :78297 Submission by Bruce Scott
	// We want to receive WT_PACKET messages (off by default)	
	m_pWinTabCal->SetOptions(m_pWinTabCal->GetOptions() | CXO_MESSAGES);

	// We want this context to return x, y and button info
	m_pWinTabCal->SetPktData(PK_X | PK_Y | PK_BUTTONS | PK_CURSOR);

	// Send us movement and all button events.
	m_pWinTabCal->SetMoveMask(PK_X | PK_Y);
	m_pWinTabCal->SetBtnDnMask(0xFFFFFFF);
	m_pWinTabCal->SetBtnUpMask(0xFFFFFFF);

	// Open the contexts. Tablet context on the bottom.
	m_pWinTabTab->Open(hWnd, FALSE);
	m_pWinTabSys->Open(hWnd, FALSE);
	m_pWinTabCal->Open(hWnd, FALSE);
	m_pWinTabRaw->Open(hWnd, FALSE);

	if (IsConfigured())
		{
		//set the screen pointing area to the saved values
		RECT rctScreen, rctSystem;
		GetScreenSize (&rctScreen);
		rctSystem.left = rctScreen.left;
		rctSystem.right = rctScreen.right;
		rctSystem.top = rctScreen.bottom;
		rctSystem.bottom = rctScreen.top;
		m_pWinTabSys->SetInputArea(&rctSystem);

		//recalcuate the transformation and set it.
		int nRows, nCols;
		sds_point ptDig[3], ptWorld[3];
		GetTabletSize (ptDig[0], ptDig[1], ptDig[2]);
		GetNumberOfRows (&nRows);
		GetNumberOfCols (&nCols);
		(ptWorld[0])[0] = 0.0;
		(ptWorld[0])[1] = 0.0;
		(ptWorld[0])[2] = 0.0;
		(ptWorld[1])[0] = 0.0;
		(ptWorld[1])[1] = nRows;
		(ptWorld[1])[2] = 0.0;
		(ptWorld[2])[0] = nCols;
		(ptWorld[2])[1] = nRows;
		(ptWorld[2])[2] = 0.0;
		ptDig[0][2] = 0;
		ptDig[1][2] = 0;
		ptDig[2][2] = 0;

		C3Point * P = new C3Point[3];
		C3Point * Q = new C3Point[3];
		CAffine T;
		sds_matrix ATrans;
		for (int i = 0; i < 3; i++)
		{
			sds_point_to_C3Point(ptDig[i], &P[i]);
			sds_point_to_C3Point(ptWorld[i], &Q[i]);
		}
		Affine3Pt(P, Q, T);
		CAffine_to_sds_matrix(T, ATrans);
		// clean up temporary memory
		delete [] P;
		delete [] Q;

		m_pWinTabTab->SetTransformation(ATrans, XFORM_AFFINE);

		//Enable the correct contexts
		EnableCfg (TRUE);
		EnableCal (FALSE);
		}
	else if (IsCalibrated())
		{
		//set the calibration matrix
		sds_matrix cal;
		GetCalibration (cal);
		SetCalibration (cal, XFORM_PROJECTIVE);

		//Enable the correct contexts
		EnableCfg (FALSE);
		EnableCal (TRUE);
		}


	return TRUE;
	}

BOOL CIcadTablet::GetTabletSize(sds_point topleft, sds_point bottomleft, sds_point bottomright)
	{
	topleft[0] = m_Configuration.topleft[0];
	topleft[1] = m_Configuration.topleft[1];
	topleft[2] = m_Configuration.topleft[2];
	bottomleft[0] = m_Configuration.bottomleft[0] ;
	bottomleft[1] = m_Configuration.bottomleft[1] ;
	bottomleft[2] = m_Configuration.bottomleft[2] ;
	bottomright[0] = m_Configuration.bottomright[0];
	bottomright[1] = m_Configuration.bottomright[1];
	bottomright[2] = m_Configuration.bottomright[2];
	return TRUE;
	}

BOOL CIcadTablet::GetScreenSize (RECT *rect)
	{

    RECT rctSystem;
	//if the screen points are set use them
	rctSystem.left		= (long)m_Configuration.screen.bottomleft[0];
	rctSystem.bottom	= (long)m_Configuration.screen.bottomleft[1];
	rctSystem.right		= (long)m_Configuration.screen.topright[0];
	rctSystem.top		= (long)m_Configuration.screen.topright[1];

	if ((rctSystem.left == 0) && (rctSystem.bottom==0) && (rctSystem.right==0) && (rctSystem.top==0))
		{
		//otherwise use the screen size by grid
		int StartX, StartY, EndX, EndY;
		StartX	= m_Configuration.screen.left;
		StartY	= m_Configuration.screen.bottom;
		EndX	= m_Configuration.screen.right;
		EndY	= m_Configuration.screen.top;
		int TabWidth, TabHeight;

		TabWidth  = (int)(m_Configuration.bottomright[0] - m_Configuration.bottomleft[0]);
		TabHeight = (int)(m_Configuration.topleft[1] - m_Configuration.bottomleft[1]);
		rctSystem.left   = (TabWidth * StartX) / m_Configuration.nCols; 
		rctSystem.right  = (TabWidth * EndX) / m_Configuration.nCols;
		rctSystem.bottom = (TabHeight * EndY) / m_Configuration.nRows; 
		rctSystem.top	 = (TabHeight * StartY) / m_Configuration.nRows;
		}

	*rect = rctSystem;
	return TRUE;
	}

BOOL CIcadTablet::GetNumberOfRows (int *nRows)
	{
	*nRows = m_Configuration.nRows; 
	return TRUE;
	}

BOOL CIcadTablet::GetNumberOfCols (int *nCols)
	{
	*nCols = m_Configuration.nCols; 
	return TRUE;
	}

BOOL CIcadTablet::GetCalibration (sds_matrix xform)
	{
	int i,j;
	for (i=0; i<4; i++)
		{
		for (j=0; j<4; j++)
			{
			xform[i][j] = m_Calibration.xform[i][j];
			}
		}
	return TRUE;
	}

BOOL CIcadTablet::GetGridItem (int nCol, int nRow, LPTSTR cmd)
	{
	int index = nRow*m_Configuration.nRows + nCol;
	strcpy (cmd, (m_TabletCmdList[index]).cmd);
	return TRUE;
	}

BOOL CIcadTablet::GetButton (int nButton, LPTSTR cmd)
	{
	strcpy (cmd, m_ButtonCmdList[nButton].cmd);
	return TRUE;
	}

BOOL CIcadTablet::SetScreenSize(int left, int bottom, int top, int right, sds_point lowerleft, sds_point upperright)
	{
	m_Configuration.screen.bottom = bottom;
	m_Configuration.screen.left = left;
	m_Configuration.screen.top = top;
	m_Configuration.screen.right = right;
	m_Configuration.screen.bottomleft[0] = lowerleft[0];
	m_Configuration.screen.bottomleft[1] = lowerleft[1];
	m_Configuration.screen.bottomleft[2] = lowerleft[2];
	m_Configuration.screen.topright[0] = upperright[0];
	m_Configuration.screen.topright[1] = upperright[1];
	m_Configuration.screen.topright[2] = upperright[2];
	m_bCfgModified = TRUE;
	return TRUE;
	}

BOOL CIcadTablet::SetGridSize(sds_point upperleft, sds_point lowerleft, sds_point lowerright)
	{
	m_Configuration.topleft[0] = upperleft[0];
	m_Configuration.topleft[1] = upperleft[1];
	m_Configuration.topleft[2] = upperleft[2];
	m_Configuration.bottomleft[0] = lowerleft[0];
	m_Configuration.bottomleft[1] = lowerleft[1];
	m_Configuration.bottomleft[2] = lowerleft[2];
	m_Configuration.bottomright[0] = lowerright[0];
	m_Configuration.bottomright[1] = lowerright[1];
	m_Configuration.bottomright[2] = lowerright[2];
	m_bCfgModified = TRUE;
	return TRUE;
	}

BOOL CIcadTablet::SetGridDimensions (int nRows, int nCols)
	{
	m_Configuration.nRows = nRows;
	m_Configuration.nCols = nCols;
	m_bCfgModified = TRUE;
	return TRUE;
	}

BOOL CIcadTablet::SetGridItem (int nCol, int nRow, CString cmd)
	{
	int index = nRow*m_Configuration.nRows + nCol;
	(m_TabletCmdList[index]).cmd = cmd.Left(255);
	m_bCfgModified = TRUE;
	return TRUE;
	}

BOOL CIcadTablet::SetButton (int nButton, CString cmd)
	{
	m_ButtonCmdList[nButton].cmd = cmd.Left(255);
	m_bButModified = TRUE;
	return TRUE;
	}

BOOL CIcadTablet::SetCalibration (sds_matrix xform, xform_type type)
	{
	int i,j;
	for (i=0; i<4; i++)
		{
		for (j=0; j<4; j++)
			{
			m_Calibration.xform[i][j] = xform[i][j];
			}
		}
	m_Calibration.type = type;
	m_bCalModified = TRUE;

	m_pWinTabCal->SetTransformation(xform, type);
	return TRUE;
	}

BOOL CIcadTablet::SetDefaults()
	{
	m_bCalModified = TRUE;
	m_bCfgModified = TRUE;
	m_bButModified = TRUE;

	//Initialize the Tablet Configuration
	int nRows = TABLET_NROWS;
	int nCols = TABLET_NCOLS;

	//assume we are using a 12x12 tablet as defaults
	double OneInchX = m_TabletSize.cx/12;
	double OneInchY = m_TabletSize.cy/12;
	double LeftMargin	= OneInchX * 3/4; //3/4"margin on left 
	double RightEdge	= OneInchX * 11.18;  //11 1/4" from left
	double BottomMargin = OneInchY * 11/16; //11/16"margin on bottom
	double TopEdge		= OneInchY * 11.1; //11" from bottom
	m_Configuration.nRows = nRows;
	m_Configuration.nCols = nCols;
	m_Configuration.topleft[0] = LeftMargin;
	m_Configuration.topleft[1] = TopEdge;
	m_Configuration.topleft[2] = 0;
	m_Configuration.bottomleft[0] = LeftMargin;
	m_Configuration.bottomleft[1] = BottomMargin;
	m_Configuration.bottomleft[2] = 0;
	m_Configuration.bottomright[0] = RightEdge;
	m_Configuration.bottomright[1] = BottomMargin;
	m_Configuration.bottomright[2] = 0;

	m_Configuration.screen.bottom = 19; //Row R
	m_Configuration.screen.left = 14;   //Col 12
	m_Configuration.screen.top = 10; //Row I
	m_Configuration.screen.right = 23;  //Row 21
	m_Configuration.screen.bottomleft[0] = 0;
	m_Configuration.screen.bottomleft[1] = 0;
	m_Configuration.screen.bottomleft[2] = 0;
	m_Configuration.screen.topright[0] = 0;
	m_Configuration.screen.topright[1] = 0;
	m_Configuration.screen.topright[2] = 0;


	//Initialize the Grid commands
	int nEntries = nRows*nCols;
	if (m_TabletCmdList)
		delete [] m_TabletCmdList;
	m_TabletCmdList = new SDS_TabletSetup[nEntries];

	int i, j;
	for (i=0; i<nEntries; i++)
		{
		(m_TabletCmdList[i]).cmd = (SDS_pTabletDefaultCmdList[i]).cmd;
		}


	//Initialize the Button commands
	for (i=0; i<64; i++)
		{
		(m_ButtonCmdList[i]).cmd = ""/*DNT*/;
		}
	m_ButtonCmdList[0].cmd = ""/*DNT*/; 
	m_ButtonCmdList[1].cmd = "'_REDRAW"/*DNT*/; 
	m_ButtonCmdList[2].cmd = "'_REGEN"/*DNT*/; 
	m_ButtonCmdList[3].cmd = ""/*DNT*/; 
	m_ButtonCmdList[4].cmd = "'_SNAP"/*DNT*/; 
	m_ButtonCmdList[5].cmd = "'_ORTHO"/*DNT*/; 
	m_ButtonCmdList[6].cmd = "'_GRID'"/*DNT*/; 
	m_ButtonCmdList[7].cmd = "'_EXPLORER"/*DNT*/; 
	m_ButtonCmdList[8].cmd = "'_COPY"/*DNT*/; 
	m_ButtonCmdList[9].cmd = "'_PASTE"/*DNT*/; 
	m_ButtonCmdList[10].cmd = "'_ENTPROP"/*DNT*/; 
	m_ButtonCmdList[11].cmd = ""/*DNT*/; 
	m_ButtonCmdList[12].cmd = "'_ZOOM;2x"/*DNT*/; 
	m_ButtonCmdList[13].cmd = "'_PAN"/*DNT*/; 
	m_ButtonCmdList[14].cmd = "'_ZOOM;.5x"/*DNT*/; 
	m_ButtonCmdList[15].cmd = ""/*DNT*/; 

	//Initialize the Calibration
	//TODO m_Calibration.xform
	for (i=0; i<4; i++)
		{
		for (j=0; j<4; j++)
			{
			if (i==j)
				m_Calibration.xform[i][j] = 1.0;
			else
				m_Calibration.xform[i][j] = 0.0;
			}
		}

        
    return(TRUE);
	}

BOOL CIcadTablet::SaveToRegistry()
	{
	CString Str, Val;
	char *pstr;
	PreferenceKey		tabletKey(HKEY_CURRENT_USER, "Tablet", FALSE);
	HKEY hKeyMenuRoot = tabletKey;
	DWORD dwRetVal;
	int i, j, strlen;

	//*** Return if the registry exists but the Modified flag has not been set;
	if(hKeyMenuRoot != NULL)
		{
//		RegCloseKey(hKeyMenuRoot);
		if ((!m_bCalModified) && (!m_bCfgModified) && (!m_bButModified))
			{
			//save the IsCalibrated and IsConfigured flags always, since setting these 2 flags
			//does not require deleting and resaving the entire tablet info.
			HKEY hKeyCalRoot = tabletKey;
			if(RegOpenKeyEx(hKeyCalRoot,"Calibration",0,KEY_READ,&hKeyCalRoot)==ERROR_SUCCESS)
				{
				Str = "IsCalibrated"/*DNT*/;
				RegSetValueEx(hKeyCalRoot,Str,0,REG_DWORD,(CONST BYTE*)&m_bCalibrated,sizeof(int));
				RegCloseKey(hKeyCalRoot);
				}

			HKEY hKeyCfgRoot = tabletKey;
			if(RegOpenKeyEx(hKeyCfgRoot,"Configuration",0,KEY_READ,&hKeyCfgRoot)==ERROR_SUCCESS)
				{
				Str = "IsConfigured"/*DNT*/;
				RegSetValueEx(hKeyCfgRoot,Str,0,REG_DWORD,(CONST BYTE*)&m_bConfigured,sizeof(int));
				RegCloseKey(hKeyCfgRoot);
				}

			return FALSE;
			}
		}
	PreferenceKey TabKey(HKEY_CURRENT_USER, "Tablet");

	if (m_bCalModified)
		{
		HKEY hKeyCalRoot = tabletKey;

		IcadRegDeleteKey(hKeyCalRoot, "Calibration");
		hKeyCalRoot = TabKey;
		if(RegCreateKeyEx(hKeyCalRoot,"Calibration",0,""/*DNT*/,REG_OPTION_NON_VOLATILE,
						 KEY_ALL_ACCESS,NULL,&hKeyCalRoot,&dwRetVal)!=ERROR_SUCCESS) return FALSE;

		Str = "IsCalibrated"/*DNT*/;
		RegSetValueEx(hKeyCalRoot,Str,0,REG_DWORD,(CONST BYTE*)&m_bCalibrated,sizeof(int));

		for (i=0; i<4; i++)
			{
			for (j=0; j<4; j++)
				{
				Str.Format ("Matrix%d%d"/*DNT*/, i, j);
				Val.Format ("%f"/*DNT*/, m_Calibration.xform[i][j]);
				RegSetValueEx(hKeyCalRoot,Str,0,REG_SZ,(CONST BYTE*)Val.GetBuffer(0),Val.GetLength());
				Val.Empty();
				}
			}
		RegCloseKey(hKeyCalRoot);
		}

	if (m_bCfgModified)
		{
		HKEY hKeyCfgRoot = TabKey;
		
		IcadRegDeleteKey(hKeyCfgRoot,"Configuration");
		hKeyCfgRoot = TabKey;

		if(RegCreateKeyEx(hKeyCfgRoot,"Configuration",0,""/*DNT*/,REG_OPTION_NON_VOLATILE,
						 KEY_ALL_ACCESS,NULL,&hKeyCfgRoot,&dwRetVal)!=ERROR_SUCCESS) return FALSE;

		Str = "IsConfigured"/*DNT*/;
		RegSetValueEx(hKeyCfgRoot,Str,0,REG_DWORD,(CONST BYTE*)&m_bConfigured,sizeof(int));

		Str = "Grid Rows"/*DNT*/;
		RegSetValueEx(hKeyCfgRoot,Str,0,REG_DWORD,(CONST BYTE*)&m_Configuration.nRows,sizeof(int));

		Str = "Grid Columns"/*DNT*/;
		RegSetValueEx(hKeyCfgRoot,Str,0,REG_DWORD,(CONST BYTE*)&m_Configuration.nCols,sizeof(int));

		Str = "Grid TopLeft"/*DNT*/;
		Val.Format ("%f,%f,%f"/*DNT*/, m_Configuration.topleft[0],m_Configuration.topleft[1],m_Configuration.topleft[2]);
		RegSetValueEx(hKeyCfgRoot,Str,0,REG_SZ,(CONST BYTE*)Val.GetBuffer(0),Val.GetLength());

		Str = "Grid BottomLeft"/*DNT*/;
		Val.Format ("%f,%f,%f"/*DNT*/, m_Configuration.bottomleft[0],m_Configuration.bottomleft[1],m_Configuration.bottomleft[2]);
		RegSetValueEx(hKeyCfgRoot,Str,0,REG_SZ,(CONST BYTE*)Val.GetBuffer(0),Val.GetLength());

		Str = "Grid BottomRight"/*DNT*/;
		Val.Format ("%f,%f,%f"/*DNT*/, m_Configuration.bottomright[0],m_Configuration.bottomright[1],m_Configuration.bottomright[2]);
		RegSetValueEx(hKeyCfgRoot,Str,0,REG_SZ,(CONST BYTE*)Val.GetBuffer(0),Val.GetLength());

		Str = "Screen Grid Position Bottom"/*DNT*/;
		RegSetValueEx(hKeyCfgRoot,Str,0,REG_DWORD,(CONST BYTE*)&m_Configuration.screen.bottom,sizeof(int));
		Str = "Screen Grid Position Left"/*DNT*/;
		RegSetValueEx(hKeyCfgRoot,Str,0,REG_DWORD,(CONST BYTE*)&m_Configuration.screen.left,sizeof(int));
		Str = "Screen Grid Position Top"/*DNT*/;
		RegSetValueEx(hKeyCfgRoot,Str,0,REG_DWORD,(CONST BYTE*)&m_Configuration.screen.top,sizeof(int));
		Str = "Screen Grid Position Right"/*DNT*/;
		RegSetValueEx(hKeyCfgRoot,Str,0,REG_DWORD,(CONST BYTE*)&m_Configuration.screen.right,sizeof(int));

		Str = "Screen BottomLeft"/*DNT*/;
		Val.Format ("%f,%f,%f"/*DNT*/, m_Configuration.screen.bottomleft[0],m_Configuration.screen.bottomleft[1],m_Configuration.screen.bottomleft[2]);
		RegSetValueEx(hKeyCfgRoot,Str,0,REG_SZ,(CONST BYTE*)Val.GetBuffer(0),Val.GetLength());

		Str = "Screen TopRight"/*DNT*/;
		Val.Format ("%f,%f,%f"/*DNT*/, m_Configuration.screen.topright[0],m_Configuration.screen.topright[1],m_Configuration.screen.topright[2]);
		RegSetValueEx(hKeyCfgRoot,Str,0,REG_SZ,(CONST BYTE*)Val.GetBuffer(0),Val.GetLength());

		int nEntries = m_Configuration.nRows * m_Configuration.nCols;
		for (i=0; i<nEntries; i++)
			{
			Str.Format ("Grid Command %d"/*DNT*/, i);
			strlen = (m_TabletCmdList[i].cmd).GetLength();
			if (strlen)
				{
				pstr = m_TabletCmdList[i].cmd.GetBuffer(0);
				RegSetValueEx(hKeyCfgRoot,Str,0,REG_SZ,(CONST BYTE*)pstr,strlen);
				}
			}
		RegCloseKey(hKeyCfgRoot);
		}

	if (m_bButModified)
		{
		HKEY hKeyButRoot = TabKey;

		IcadRegDeleteKey(hKeyButRoot,"Buttons");
		hKeyButRoot = TabKey;

		if(RegCreateKeyEx(hKeyButRoot,"Buttons",0,""/*DNT*/,REG_OPTION_NON_VOLATILE,
						 KEY_ALL_ACCESS,NULL,&hKeyButRoot,&dwRetVal)!=ERROR_SUCCESS) return FALSE;

		for (i=0; i<64; i++)
			{
			Str.Format ("Button Command %d"/*DNT*/, i);
			strlen = (m_ButtonCmdList[i].cmd).GetLength();
			if (strlen)
				{
				pstr = (m_ButtonCmdList[i].cmd).GetBuffer(0);
				RegSetValueEx(hKeyButRoot,Str,0,REG_SZ,(CONST BYTE*)pstr,strlen);
				}
			}
		RegCloseKey(hKeyButRoot);
		}

	return TRUE;
	}


BOOL CIcadTablet::ReadFromRegistry()
	{
	PreferenceKey		tabletKey(HKEY_CURRENT_USER, "Tablet", FALSE);
	CString Str, Val;
	HKEY hKeyMenuRoot = tabletKey;
	DWORD dwRetVal, dwSize;
	int i, j;
	char strvalue[256];

	//*** Return if the registry does not exist
	if(hKeyMenuRoot == NULL)
		{
		return FALSE;
		}
//	RegCloseKey(hKeyMenuRoot);

	//Read the Calibration Infor
	HKEY hKeyCalRoot = tabletKey;

	if(RegOpenKeyEx(hKeyCalRoot,"Calibration",0,KEY_READ,&hKeyCalRoot)==ERROR_SUCCESS) 
		{
		Str = "IsCalibrated"/*DNT*/;
		dwSize=sizeof(int);
		RegQueryValueEx(hKeyCalRoot,Str,NULL,&dwRetVal,(LPBYTE)&m_bCalibrated,(LPDWORD)&dwSize);
		for (i=0; i<4; i++)
			{
			for (j=0; j<4; j++)
				{
				dwSize=sizeof(strvalue);
				Str.Format ("Matrix%d%d"/*DNT*/, i, j);
				RegQueryValueEx(hKeyCalRoot,Str,NULL,&dwRetVal,(LPBYTE)&strvalue,(LPDWORD)&dwSize);
				Val = strvalue;
				m_Calibration.xform[i][j] = atof (Val);
				}
			}
//		RegCloseKey(hKeyCalRoot);
		}
	else return FALSE;

	HKEY hKeyCfgRoot = tabletKey;

	if(RegOpenKeyEx(hKeyCfgRoot,"Configuration",0,KEY_READ,&hKeyCfgRoot)==ERROR_SUCCESS) 
		{
		Str = "IsConfigured"/*DNT*/;
		dwSize=sizeof(int);
		RegQueryValueEx(hKeyCfgRoot,Str,NULL,&dwRetVal,(LPBYTE)&m_bConfigured,(LPDWORD)&dwSize);

		Str = "Grid Rows"/*DNT*/;
		dwSize=sizeof(int);
		RegQueryValueEx(hKeyCfgRoot,Str,NULL,&dwRetVal,(LPBYTE)&m_Configuration.nRows,(LPDWORD)&dwSize);

		Str = "Grid Columns"/*DNT*/;
		dwSize=sizeof(int);
		RegQueryValueEx(hKeyCfgRoot,Str,NULL,&dwRetVal,(LPBYTE)&m_Configuration.nCols,(LPDWORD)&dwSize);

		Str = "Grid TopLeft"/*DNT*/;
		dwSize=sizeof(strvalue);
		RegQueryValueEx(hKeyCfgRoot,Str,NULL,&dwRetVal,NULL,(LPDWORD)&dwSize);
		RegQueryValueEx(hKeyCfgRoot,Str,NULL,&dwRetVal,(LPBYTE)&strvalue,(LPDWORD)&dwSize);
		Val = strvalue;
		ExtractPointFromCString (Val, m_Configuration.topleft);

		Str = "Grid BottomLeft"/*DNT*/;
		dwSize=sizeof(strvalue);
		RegQueryValueEx(hKeyCfgRoot,Str,NULL,&dwRetVal,(LPBYTE)&strvalue,(LPDWORD)&dwSize);
		Val = strvalue;
		ExtractPointFromCString (Val, m_Configuration.bottomleft);

		Str = "Grid BottomRight"/*DNT*/;
		dwSize=sizeof(strvalue);
		RegQueryValueEx(hKeyCfgRoot,Str,NULL,&dwRetVal,(LPBYTE)&strvalue,(LPDWORD)&dwSize);
		Val = strvalue;
		ExtractPointFromCString (Val, m_Configuration.bottomright);

		Str = "Screen Grid Position Bottom"/*DNT*/;
		dwSize=sizeof(int);
		RegQueryValueEx(hKeyCfgRoot,Str,NULL,&dwRetVal,(LPBYTE)&m_Configuration.screen.bottom,(LPDWORD)&dwSize);
		Str = "Screen Grid Position Left"/*DNT*/;
		dwSize=sizeof(int);
		RegQueryValueEx(hKeyCfgRoot,Str,NULL,&dwRetVal,(LPBYTE)&m_Configuration.screen.left,(LPDWORD)&dwSize);
		Str = "Screen Grid Position Top"/*DNT*/;
		dwSize=sizeof(int);
		RegQueryValueEx(hKeyCfgRoot,Str,NULL,&dwRetVal,(LPBYTE)&m_Configuration.screen.top,(LPDWORD)&dwSize);
		Str = "Screen Grid Position Right"/*DNT*/;
		dwSize=sizeof(int);
		RegQueryValueEx(hKeyCfgRoot,Str,NULL,&dwRetVal,(LPBYTE)&m_Configuration.screen.right,(LPDWORD)&dwSize);

		Str = "Screen BottomLeft"/*DNT*/;
		dwSize=sizeof(strvalue);
		RegQueryValueEx(hKeyCfgRoot,Str,NULL,&dwRetVal,(LPBYTE)&strvalue,(LPDWORD)&dwSize);
		Val = strvalue;
		ExtractPointFromCString (Val, m_Configuration.screen.bottomleft);

		Str = "Screen TopRight"/*DNT*/;
		dwSize=sizeof(strvalue);
		RegQueryValueEx(hKeyCfgRoot,Str,NULL,&dwRetVal,(LPBYTE)&strvalue,(LPDWORD)&dwSize);
		Val = strvalue;
		ExtractPointFromCString (Val, m_Configuration.screen.topright);

		int nEntries = m_Configuration.nRows * m_Configuration.nCols;

		if (m_TabletCmdList)
			delete [] m_TabletCmdList;
		m_TabletCmdList = new SDS_TabletSetup[nEntries];

		for (i=0; i<nEntries; i++)
			{
			Str.Format ("Grid Command %d"/*DNT*/, i);
			dwSize=sizeof(strvalue);
			if (RegQueryValueEx(hKeyCfgRoot,Str,NULL,&dwRetVal,(LPBYTE)&strvalue,(LPDWORD)&dwSize) == ERROR_SUCCESS)
				m_TabletCmdList[i].cmd = strvalue;
			else
				m_TabletCmdList[i].cmd = "";

			strvalue[0] = '\0';
			}
//		RegCloseKey(hKeyCfgRoot);
		}
	else return FALSE;

	HKEY hKeyButRoot = tabletKey;

	if(RegOpenKeyEx(hKeyButRoot,"Buttons",0,KEY_READ,&hKeyButRoot)==ERROR_SUCCESS) 
		{
		for (i=0; i<64; i++)
			{
			Str.Format ("Button Command %d"/*DNT*/, i);
			dwSize=sizeof(strvalue);
			if (RegQueryValueEx(hKeyButRoot,Str,NULL,&dwRetVal,(LPBYTE)&strvalue,(LPDWORD)&dwSize)== ERROR_SUCCESS)
				m_ButtonCmdList[i].cmd = strvalue;
			else
				m_ButtonCmdList[i].cmd = "";

			strvalue[0] = '\0';
			}
//		RegCloseKey(hKeyButRoot);
		}
	else return FALSE;

	return TRUE;
	}

BOOL CIcadTablet::SaveToICDFile(CString Filename)
	{
	return FALSE;
	}

BOOL CIcadTablet::ReadFromICDFile (CString Filename)
	{
	return FALSE;
	}


BOOL CIcadTablet::EnableCfg(BOOL bEnable)
	{
	m_pWinTabSys->Enable(bEnable);
	m_pWinTabTab->Enable(bEnable);
	return TRUE;
	}

BOOL CIcadTablet::EnableCal(BOOL bEnable)
	{
	m_pWinTabCal->Enable(bEnable);
	return TRUE;
	}

BOOL CIcadTablet::EnableRaw(BOOL bEnable)
	{
	m_pWinTabRaw->Enable(bEnable);
	return TRUE;
	}



void CIcadTablet::ProcessDigitizerTemplate ()
	{
	CString command;
	sds_point Pos;
	m_pWinTabTab->GetTabletPosition(Pos);

	if ((Pos[1] >= m_Configuration.nRows) || (Pos[0] >= m_Configuration.nCols) || (Pos[1] < 0) || (Pos[0] < 0))
		return;

	int index = (int)Pos[1] * m_Configuration.nRows + (int)Pos[0];
	command = m_TabletCmdList[index].cmd;
	if (!(command.IsEmpty() || command == " "))
		SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)command.GetBuffer(0));
	}

void CIcadTablet::ProcessDigitizerTemplateHelpString ()
	{
	CString command;
	sds_point Pos;
	m_pWinTabTab->GetTabletPosition(Pos);

	if ((Pos[1] >= m_Configuration.nRows) || (Pos[0] >= m_Configuration.nCols) || (Pos[1] < 0) || (Pos[0] < 0))
		return;

	int index = (int)Pos[1] * m_Configuration.nRows + (int)Pos[0];
	command = m_TabletCmdList[index].cmd;
#ifdef _DEBUG
	if (command.IsEmpty() || command == " ")
		command.Format ("Row %d Col %d", (int)Pos[1], (int)Pos[0]);
#endif 

	if (!(command.IsEmpty() || command == " "))
		SDS_CMainWindow->m_StatusBar.TextOut(command.GetBuffer(0));
	}

void CIcadTablet::ProcessDigitizerButton (int but_num)
	{
	CString command;
	ASSERT (but_num < 64);
	BOOL bShiftDown = (GetKeyState(VK_SHIFT) < 0);
	BOOL bCtrlDown  = (GetKeyState(VK_CONTROL) < 0);
	if (bShiftDown) but_num += 16;
	if (bCtrlDown) but_num += 32;
	command = m_ButtonCmdList[but_num].cmd;
	if (!(command.IsEmpty() || command == " "))
		SDS_SendMessage(WM_MENUSELECT,0,(LPARAM)command.GetBuffer(0));
	}


void CIcadTablet::ProcessDigitizerInput (WPARAM wParam, LPARAM lParam)
	{

	// Determine owner context of packet

	CWinTabCtx* pCtx = NULL;

	if (m_pWinTabTab && (HCTX)lParam == m_pWinTabTab->GetHandle() )
		pCtx = m_pWinTabTab;
	else if (m_pWinTabSys && (HCTX)lParam == m_pWinTabSys->GetHandle() )
		pCtx = m_pWinTabSys;
	else if (m_pWinTabCal && (HCTX)lParam == m_pWinTabCal->GetHandle() )
		pCtx = m_pWinTabCal;
	else if (m_pWinTabRaw && (HCTX)lParam == m_pWinTabRaw->GetHandle() )
		pCtx = m_pWinTabRaw;

	if ( !pCtx )
		return;

	pCtx->ReadPacket (wParam);

	// Gather button event information

	BOOL bClicked   = FALSE;
	BOOL bProcessed = FALSE;

	static DWORD oldButtonMask = 0;

	DWORD button  = pCtx->GetButtons ();
	DWORD newbtns = (oldButtonMask ^ button) & button;
	oldButtonMask = button;

	if ( newbtns )
		{
		// Get button map for current cursor type

		BYTE SysBtnMap[32];
		UINT cursor = pCtx->GetCursor ();
		CWinTabCsr* pCursor = new CWinTabCsr(cursor);
		pCursor->GetButtonMap(NULL, SysBtnMap);
		delete pCursor;
	
		// Process all newly pressed buttons

		DWORD bit = 0x00000001L;

		for ( int i = 0 ; i < 32 ; i++ )
			{
			if ( newbtns & bit )
				{
					UCHAR ucBtn = SysBtnMap[i] & 0x0F; 

					if ( ucBtn == 0 && pCtx != m_pWinTabRaw )
						{
						ProcessDigitizerButton(i);
						bProcessed = TRUE;
						}
					else
						{
						bClicked |= ucBtn == SBN_LCLICK || ucBtn == SBN_LDBLCLICK;
						}
				}
			bit <<= 1;
			}
		}

	if ( bProcessed )
		return;

	if ( pCtx == m_pWinTabRaw && bClicked )
		{
		SDS_CMainWindow->m_pTablet->m_bCancelled = FALSE;
		SDS_CMainWindow->m_pTablet->m_bExit = FALSE;
		m_RawInputEvent.SetEvent();
		}
	else if ( pCtx == m_pWinTabTab && bClicked )
		ProcessDigitizerTemplate();
	else if ( pCtx == m_pWinTabTab )
		ProcessDigitizerTemplateHelpString();
	else if ( pCtx == m_pWinTabCal )
		pCtx->MouseEvent();
	}


