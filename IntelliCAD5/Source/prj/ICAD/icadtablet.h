/* E:\INTELLICAD\PRJ\ICAD\ICADTABLET.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// ** Includes
#include "CWinTab.h"/*DNT*/


#define SYSTEMCTXNAME		ResourceString(IDC_ICADMAIN_VISIO_TEMPLATE__6, "IntelliCAD Template Area" )
#define TABLETCTXNAME		ResourceString(IDC_ICADMAIN_VISIO_POINTING__7, "IntelliCAD Pointing Area" )
#define RAWCTXNAME			ResourceString(IDC_TABLET_RAWCTXNAME, "IntelliCAD Raw Input Area")
#define CALIBCTXNAME		ResourceString(IDC_TABLET_CALIBCTXNAME, "IntelliCAD Calibrated Area")
#define TABLET_NROWS 26
#define TABLET_NCOLS 26

struct SDS_TabletDefaultSetup 
	{
	LPCTSTR cmd;
	};



class CIcadTablet
	{
	//structures for use within this class only
	struct SDS_TabletSetup 
		{
		CString cmd;
		};

	struct SDS_TabletButtons
		{
		CString cmd;
		};


	struct SDS_TabletCalibration
		{
		sds_matrix xform;
		xform_type type;
		};

	struct SDS_TabletScreen
		{
		int bottom;
		int left;
		int top;
		int right;
		sds_point bottomleft;
		sds_point topright;
		};

	struct SDS_TabletConfig
		{
		int nRows;
		int nCols;
		sds_point topleft;
		sds_point bottomleft;
		sds_point bottomright;
		struct SDS_TabletScreen screen;
		};


	public:
		CIcadTablet ();
		~CIcadTablet( void );

	private:
		struct SDS_TabletSetup		*m_TabletCmdList;
									//a run-time allocated array of size (nRows * nCols) which contain a CString 
									//indicating the command to be run.  Normally nRows = 26, nCols = 26, so this
									//array can hold 900 commands.
		struct SDS_TabletButtons	m_ButtonCmdList[64];
									//an array of 64 CStrings representing the commands
									//0-15  - commands for simple ButtonClick
									//16-31 - commands for Shift+ButtonClick
									//32-47 - commands for Ctrl+ButtonClick
									//48-63 - commands for Ctrl+Shift+ButtonClick
									//Cursors may have 4 or 16 buttons.  
									//For a 4 button cursor, only the first 4 commands in each of these sections will be valid.
									//Likewise, pens have only 2 buttons usually, So commands 0,1,16,17, 32,33, 48 and 49 are valid
		struct SDS_TabletCalibration m_Calibration;
									//contains the transformation information for the calibration
		struct SDS_TabletConfig		m_Configuration;
									//contains the CFG setup.  nRows, nCols, Digitized corners if any, Screen information

		//Tablet device info
		char m_DeviceName[80];
		SIZE m_TabletSize;

		BOOL m_bInitialized;
		BOOL m_bCalModified;
		BOOL m_bCfgModified;
		BOOL m_bButModified;

		BOOL m_bCalibrated;	//TRUE if the Tablet/Cal command has been run
		BOOL m_bConfigured; //TRUE if the Tablet/Cfg command has been run

	public:
		CWinTabCtx *m_pWinTabSys;	//helper object for system context
		CWinTabCtx *m_pWinTabTab;	//helper object for tablet context
		CWinTabCtx *m_pWinTabCal;	//helper object for Calibrated context
		CWinTabCtx *m_pWinTabRaw;	//helper object for raw input context

		//The following variables are used for the sds_gettabletinput() function (cmds\tablet.cpp) to work
		//Since this function needs to wait for input, it waits for this event to be set by CMainWindow.
		CEvent m_RawInputEvent;
		BOOL m_bCancelled;	//Flag set before SetEvent to indicate if the user Cancelled the command
		BOOL m_bExit;		//Flag set before SetEvent to indicate if the user hit Enter (for cmd_calibrate)

	public:
		//Functions to set/get information in the Tablet structures
		BOOL GetScreenSize(RECT *rect);
		BOOL GetTabletSize(sds_point topleft, sds_point bottomleft, sds_point bottom_right);
		BOOL GetNumberOfRows(int *nRows);
		BOOL GetNumberOfCols(int *nCols);
		BOOL GetCalibration (sds_matrix xform);
		BOOL GetGridItem (int nCol, int nRow, LPTSTR cmd);
		BOOL GetButton (int nButton, LPTSTR cmd);
		BOOL SetScreenSize(int left, int bottom, int top, int right, sds_point lowerleft, sds_point upperright);
		BOOL SetGridSize(sds_point upperleft, sds_point lowerleft, sds_point lowerright);
		BOOL SetGridDimensions (int nRows, int nCols);
		BOOL SetGridItem (int nCol, int nRow, CString cmd);
		BOOL SetButton (int nButton, CString cmd);
		BOOL SetCalibration (sds_matrix xform, xform_type type);
		BOOL SetDefaults();
		void SetCalibrated() { m_bCalibrated = TRUE; };
		void SetConfigured() { m_bConfigured = TRUE; };
		BOOL IsCalibrated() { return m_bCalibrated; };
		BOOL IsConfigured() { return m_bConfigured; };

		//Functions to Save/Restore tablet info
		BOOL SaveToRegistry();
		BOOL ReadFromRegistry();
		BOOL SaveToICDFile(CString Filename);
		BOOL ReadFromICDFile (CString Filename);

		//Functions to interact with the Tablet
		BOOL Initialize(HWND hWnd, char* pError, UINT nSize);
		BOOL EnableCal(BOOL enable);
		BOOL EnableCfg(BOOL enable);
		BOOL EnableRaw(BOOL enable);

		void ProcessDigitizerInput (WPARAM wParam, LPARAM lParam);
		void ProcessDigitizerButton (int but_num);
		void ProcessDigitizerTemplate ();
		void ProcessDigitizerTemplateHelpString ();
	};




