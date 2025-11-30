/* D:\ICADDEV\PRJ\ICAD\BHATCHDIALOG.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Bhatch/boundary main dialog
 *
 */

#include "icad.h"
#include "icadapi.h"
#include "BhatchDialog.h"
#include "resource.hm"
#include "cmds.h"
#include "cmdsHatch.h"
#include "paths.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// VT. 28-3-02. Explicit data initialization.
BHatchDialog::BHatchDialog(BoundaryEntity enttype, db_hatch *hatch, CWnd* pParent /*=NULL*/)
	: IcadDialog(BHatchDialog::IDD, pParent), m_hatch(hatch),  m_boundType(enttype), m_prevTab(BOUNDARY),
		m_boundaryTab((enttype==HATCH),  pParent), m_patPropTab(pParent), m_patternTab(pParent),
		m_origStyle(0), m_origCrossHatch(0), m_origScale(0), m_origAngle(0), m_origSpace(0)
{
	m_bCopied = false;

	//{{AFX_DATA_INIT(BHatchDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	resbuf rb;

	if(SDS_getvar(NULL,DB_QHPSTYLE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) == RTNORM)
		m_origStyle = rb.resval.rint;

	if (SDS_getvar(NULL,DB_QHPSCALE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) == RTNORM)
		m_origScale = rb.resval.rreal;

	if (SDS_getvar(NULL,DB_QHPANG,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) == RTNORM)
		m_origAngle = 180.0 / IC_PI * rb.resval.rreal;	// convert to degrees

	if (SDS_getvar(NULL,DB_QHPSPACE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) == RTNORM)
		m_origSpace = rb.resval.rreal;

	if (SDS_getvar(NULL,DB_QHPDOUBLE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) == RTNORM)
		m_origCrossHatch = rb.resval.rint;

	if(SDS_getvar(NULL,DB_QHPNAME,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) == RTNORM)
	{
		m_origName = rb.resval.rstring;
		IC_FREE(rb.resval.rstring);
	}
}


void BHatchDialog::DoDataExchange(CDataExchange* pDX)
{
	IcadDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(BHatchDialog)
	DDX_Control(pDX, IDC_TAB2, m_TabCtrl);
	//}}AFX_DATA_MAP
}

void BHatchDialog::UndrawHatch()
{
	// only draw if user has selected boundary path
	if (m_boundaryTab.m_pickPointsCalled)
	{
		cmd_redrawEntity(m_hatch, IC_REDRAW_UNDRAW);
	}
}

void BHatchDialog::RedrawHatch(
//4M Item:17->
//BHatch Edit
									sds_name * hatches
//<-4M Item:17
				)
{
	// only draw if user has selected boundary path
	if (m_boundaryTab.m_pickPointsCalled)
	{
		cmd_redrawEntity(m_hatch, IC_REDRAW_HILITE);
//4M Item:17->
//BHatch Edit
			if (hatches){
				 long NoHatches;
				 sds_sslength(*hatches,&NoHatches);
				 for (int i=0;i<NoHatches;i++){
						sds_name ename;
						sds_ssname(*hatches,i,ename);
						db_hatch * nextEntity=(db_hatch *)ename[0];
					cmd_redrawEntity(nextEntity, IC_REDRAW_HILITE);
				}
			}
//<-4M Item:17
		}
}

void BHatchDialog::InitializeHatch()
{
	resbuf rb;

	// Get hatch pattern info from system variables
	// don't need to handle error since set the proper default will already be initialized.
	if(SDS_getvar(NULL,DB_QHPSTYLE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) == RTNORM)
		m_boundaryTab.m_islandRadio = (BHatchBoundaryTab::island) rb.resval.rint;

	m_boundaryTab.UpdateData(FALSE);

	if (m_boundType == HATCH)
	{
		if (SDS_getvar(NULL,DB_QHPSCALE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) == RTNORM)
			m_patPropTab.m_scale = rb.resval.rreal;

		if (SDS_getvar(NULL,DB_QHPANG,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) == RTNORM)
			m_patPropTab.m_angle = rb.resval.rreal * 180.0 / IC_PI; //convert to degrees

		if (SDS_getvar(NULL,DB_QHPSPACE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) == RTNORM)
			m_patPropTab.m_spacing = rb.resval.rreal;

		if (SDS_getvar(NULL,DB_QHPDOUBLE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) == RTNORM)
			m_patPropTab.m_crossHatch = rb.resval.rint;

		if(SDS_getvar(NULL,DB_QHPNAME,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) == RTNORM)
		{
			m_patternTab.m_patternName = rb.resval.rstring;
			IC_FREE(rb.resval.rstring);
		}

		// if no pattern - default to ANSI31
		// EBATECH(CNBR) -[ 05/10/2003 default to _USER for metric drawing.
		//if (m_patternTab.m_patternName.IsEmpty())
		//	m_patternTab.m_patternName = "ANSI31";
		if (m_patternTab.m_patternName.IsEmpty())
		{
			m_patternTab.m_patternName = USER;
			if(SDS_getvar(NULL,DB_QMEASUREMENT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES) == RTNORM)
				if( rb.resval.rint == DB_ENGLISH_MEASUREINIT )
					m_patternTab.m_patternName = "ANSI31";
		}
		// EBATECH(CNBR) ]-

		if (m_patternTab.m_patternName == USER)
			m_patPropTab.m_patType = BHatchPatPropTab::USER_DEFINED;

		else
		{
			m_patPropTab.m_patType = BHatchPatPropTab::PREDEFINED;

			// If predefined, call InitPatternData to initialize the file from the pattern name and the
			// pattern def.
			// if error set pattern to user defined so that some pattern is displayed.
			if (!m_bCopied && m_patternTab.InitPatternData() == RTERROR)
				m_patPropTab.m_patType = BHatchPatPropTab::USER_DEFINED;
		}

		m_patternTab.UpdateData(FALSE);
		m_patPropTab.UpdateData(FALSE);
		m_patPropTab.EnableControls();

		//set hatch properties
		m_hatch->set_75((int) m_boundaryTab.m_islandRadio);

		m_hatch->set_52(m_patPropTab.m_angle * IC_PI/180.0); 	//convert angle to radians

		m_hatch->set_71(m_patPropTab.m_associative);

		//Set values needed for a specific pattern type
		//Note: need to map pattern type values from their combo box order to their dxf value
		switch (m_patPropTab.m_patType)
		{
		case BHatchPatPropTab::PREDEFINED:
			m_hatch->set_2((char*)(LPCTSTR)m_patternTab.m_patternName);
			m_hatch->set_76(1);	//predefined
			m_hatch->set_41(m_patPropTab.m_scale);
			m_hatch->set_77(0); //double is always off with predefined and custom

			break;

		case BHatchPatPropTab::USER_DEFINED:
			m_hatch->set_2((char*)(LPCTSTR)USER);
			m_hatch->set_76(0); //user defined
			m_hatch->set_41(m_patPropTab.m_spacing);
			m_hatch->set_77(m_patPropTab.m_crossHatch);

			break;

		default:
			ASSERT(FALSE);
		}
	}
}

void BHatchDialog::UpdateHatch()
{
	m_boundaryTab.UpdateData(TRUE);
	m_patPropTab.UpdateData(TRUE);
	m_patternTab.UpdateData(TRUE);

	InitializeHatch();
}

void BHatchDialog::RecalcPattern(
//4M Item:17->
// BHatch edit
		sds_name * hatches
//<-4M Item:17
				)
{
	cmd_hpdefll *curHpDef = NULL,
				*pSave = NULL;
	int solidflag = 0;		// BugZilla:78692 Clear GC=70 when _USER is selected.
	int numlines = 0;

	// load proper pattern def
	switch(m_patPropTab.m_patType)
	{
		case BHatchPatPropTab::PREDEFINED:
			// set specific hatch settings
			m_hatch->set_2((char*)(LPCTSTR)m_patternTab.m_patternName);
			if(strsame(m_patternTab.m_patternName, "SOLID"/*DNT*/))
				solidflag = 1;
			m_hatch->set_70(solidflag);
			numlines = m_patternTab.m_numPatLines;

			// get pattern def
			curHpDef = m_patternTab.m_patternDef;
			break;

		case BHatchPatPropTab::USER_DEFINED:
			{
				// set hatch specific settings
				m_hatch->set_2((char*)(LPCTSTR)USER);
				m_hatch->set_70(solidflag);
				numlines = m_patPropTab.m_crossHatch ? 2 : 1;

				if(m_bCopied)
				{
					curHpDef = m_patternTab.m_patternDef;

					pSave = curHpDef->next;			// We always copy as from crossed.
					if(!m_patPropTab.m_crossHatch)
						curHpDef->next = NULL;

					break;
				}

				// Note: don't pass in space here because it is adjusted for space below
				if (!cmd_loadUserDefPattern(&curHpDef, m_patPropTab.m_crossHatch))
					return;
			}
			break;

		default:
			ASSERT(FALSE);	// should never get here
	}

	cmd_SetHatchFromDef(numlines, curHpDef, m_hatch);

	if(pSave)					// For user-defined
		curHpDef->next = pSave;	// (see above).

//4M Item:17->
// BHatch Edit
	 if (hatches){
			char * HatchName;
			double HatchScale,HatchAngle;
		int CrossHatch;
			m_hatch->get_2(&HatchName);
			m_hatch->get_41(&HatchScale);
		m_hatch->get_77(&CrossHatch);
		 m_hatch->get_52(&HatchAngle);

			long NoHatches;
			sds_sslength(*hatches,&NoHatches);
			for (int i=0;i<NoHatches;i++){
				 sds_name ename;
				 sds_ssname(*hatches,i,ename);
				 db_hatch * nextEntity=(db_hatch *)ename[0];
				 nextEntity->set_2(HatchName);
				 nextEntity->set_70(solidflag);
				 nextEntity->set_41(HatchScale);
				 nextEntity->set_77(CrossHatch);
				 nextEntity->set_52(HatchAngle);
				cmd_SetHatchFromDef(numlines, curHpDef, nextEntity);
			}
	 }
//<-4M Item:17

		// need to free hatch def if user defined.
	if (!m_bCopied && m_patPropTab.m_patType == BHatchPatPropTab::USER_DEFINED)
		cmd_hatch_freehpdefll(&curHpDef);

}

BEGIN_MESSAGE_MAP(BHatchDialog, CDialog)
	//{{AFX_MSG_MAP(BHatchDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB2, OnSelchangeTab2)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL BHatchDialog::OnInitDialog()
{
	IcadDialog::OnInitDialog();

	m_prevTab = BOUNDARY;

	// initialize hatch settings from system variables.

	EditString itemText;

	//if boundary or bpoly was used to start command use a change Window Caption.  Otherwise,
	//leave as default for boundary hatch.

	if (m_boundType == POLYLINE) {
		itemText = ResourceString(IDC_ICADBHATCH_TITLE_BOUNDARY, "Boundary");
		SetWindowText(itemText);
	}

	TC_ITEM TabCtrlItem;
		TabCtrlItem.mask=TCIF_TEXT;

	//Add Boundary Tab
	itemText = ResourceString(IDC_ICADBHATCH_BOUNDARY, "Boundary");
	TabCtrlItem.pszText = itemText;
	m_TabCtrl.InsertItem(0,&TabCtrlItem);

	//Add top tab info
		if (!m_boundaryTab)
		m_boundaryTab.Create(BHATCH_BOUNDARY,this);

	//if hatch add pattern and pattern propreties tabs.
	if (m_boundType == HATCH) {
		itemText = ResourceString(IDC_ICADBHATCH_PATTERN_PROP, "Pattern Properties");
		TabCtrlItem.pszText = itemText;
		m_TabCtrl.InsertItem(1,&TabCtrlItem);

		itemText = ResourceString(IDC_ICADBHATCH_PATTERN, "Pattern");
		TabCtrlItem.pszText = itemText;
		m_TabCtrl.InsertItem(2,&TabCtrlItem);

		m_patternTab.Create(BHATCH_PATTERN, this);
		m_patPropTab.Create(BHATCH_PAT_PROP, this);
	}


	m_boundaryTab.EnableWindow(TRUE);
		m_boundaryTab.ShowWindow(SW_SHOWNORMAL);

	CWnd *item = GetDlgItem(IDOK);
	if (m_boundaryTab.m_pickPointsCalled || m_boundaryTab.m_selectObjectsCalled)
	{
		item->EnableWindow(TRUE);
	}

	//Set icon on Help button
	CButton *helpWnd = (CButton*)GetDlgItem(IDHELP);
	helpWnd->SetIcon(AfxGetApp()->LoadIcon(HELP_ICON));

	if (m_boundType == HATCH)
		{
		//Initialize hatch so settings are the same as the dialog box.
		InitializeHatch();
		RecalcPattern();
		}

	return TRUE;	// return TRUE unless you set the focus to a control
								// EXCEPTION: OCX Property Pages should return FALSE
}

void BHatchDialog::OnSelchangeTab2(NMHDR* pNMHDR, LRESULT* pResult)
{
		switch(m_prevTab) {
				case BOUNDARY:
						m_boundaryTab.EnableWindow(FALSE);
						m_boundaryTab.ShowWindow(SW_HIDE);
						break;

		case PATTERN_PROP:
			m_patPropTab.EnableWindow(FALSE);
			m_patPropTab.ShowWindow(SW_HIDE);
			break;

		case PATTERN:
			m_patternTab.EnableWindow(FALSE);
			m_patternTab.ShowWindow(SW_HIDE);
			break;
	}

	m_prevTab = (Tab) m_TabCtrl.GetCurSel(); //Get the current selection

		switch(m_prevTab) {
				case BOUNDARY:
				if (!m_boundaryTab)
				m_boundaryTab.Create(BHATCH_BOUNDARY,this);

						m_boundaryTab.EnableWindow(TRUE);
						m_boundaryTab.ShowWindow(SW_SHOWNORMAL);
						break;

		case PATTERN_PROP:
				if (!m_patPropTab)
				m_patPropTab.Create(BHATCH_PAT_PROP,this);

			m_patPropTab.EnableWindow(TRUE);
			m_patPropTab.ShowWindow(SW_SHOWNORMAL);
			break;

		case PATTERN:
				if (!m_patternTab)
				m_patternTab.Create(BHATCH_PATTERN,this);

			m_patternTab.EnableWindow(TRUE);
			m_patternTab.ShowWindow(SW_SHOWNORMAL);
			break;
	}

	*pResult = 0;
}

void BHatchDialog::OnHelp()
{
	if (m_boundType == POLYLINE) {
		CommandRefHelp( m_hWnd, HLP_BPOLY);
	}
	else{
		CommandRefHelp( m_hWnd, HLP_BHATCH);
	}
}

void BHatchDialog::EnablePatternTab(BOOL enable)
{
	m_patternTab.EnableControls(enable);
}

void BHatchDialog::OnDestroy()
{
	CDialog::OnDestroy();

	//set system variables
	resbuf rb;

	m_boundaryTab.UpdateData(TRUE);

	// save style
	rb.restype = RTSHORT;
	rb.resval.rint = (int) m_boundaryTab.m_islandRadio;
	sds_setvar("HPSTYLE", &rb);

	if (m_boundType == HATCH)
	{
		m_patPropTab.UpdateData(TRUE);
		m_patternTab.UpdateData(TRUE);

		// save hatch data back to dialogs
		if (m_patPropTab.m_patType == BHatchPatPropTab::PREDEFINED)
		{
			rb.restype=RTSTR;
			rb.resval.rstring = m_patternTab.m_patternName.GetBuffer(0);
			sds_setvar("HPNAME"/*DNT*/,&rb);

			rb.restype=RTREAL;
			rb.resval.rreal = m_patPropTab.m_scale;
			sds_setvar("HPSCALE"/*DNT*/,&rb);

		}
		else
		{
			rb.restype=RTSTR;
			// cannot pass buffer to const
			CString temp = USER;
			rb.resval.rstring = temp.GetBuffer(0);
			sds_setvar("HPNAME"/*DNT*/,&rb);

			rb.restype=RTREAL;
			rb.resval.rreal = m_patPropTab.m_spacing;
			sds_setvar("HPSPACE"/*DNT*/,&rb);

			//*** Set the double-hatch flag
			rb.restype=RTSHORT;
			rb.resval.rint = m_patPropTab.m_crossHatch;
			sds_setvar("HPDOUBLE"/*DNT*/,&rb);
		}

		//*** Set the angle of the hatch pattern
		rb.restype=RTREAL;
		rb.resval.rreal = m_patPropTab.m_angle * IC_PI / 180.0;  //convert to radians
		sds_setvar("HPANG"/*DNT*/,&rb);
	}
}

void BHatchDialog::OnCancel()
{
	// Restore original system variables.  Since hatch dialog is destroyed while still active
	// (when picking points) Need to save the original system variables so that they can be
	// restored when the user cancels

	// OnDestroy is called after OnCancel so we need to reset the dialog values and they will
	// be saved to system varibles in OnDestroy.

	m_boundaryTab.m_islandRadio = m_origStyle;
	m_boundaryTab.UpdateData(FALSE);

	if (m_boundType == HATCH)
	{
		m_patternTab.m_patternName = m_origName;
		m_patPropTab.m_scale = m_origScale;
		m_patPropTab.m_spacing = m_origSpace;
		m_patPropTab.m_crossHatch = m_origCrossHatch;
		m_patPropTab.m_angle = m_origAngle;

		m_patternTab.UpdateData(FALSE);
		m_patPropTab.UpdateData(FALSE);
	}

	CDialog::OnCancel();
}

void BHatchDialog::OnOK()
{
	SDS_RedrawDrawing(SDS_CURDWG,NULL,NULL,1);
	CDialog::OnOK();
}

void
BHatchDialog::CopyHatchPropertiesAndPattern()
{

	sds_real	ScaleSpace, HatchAng;
	bool		bPredefined = true;

	db_handitem*	pEntity;

	for(pEntity = NULL; !pEntity; )
	{
		sds_name	EntityName;
		sds_point	SomePoint;
		switch(sds_entsel(ResourceString(IDS_COPYHATCH_CMD, "\nSelect hatch entity to copy properties from: "), EntityName, SomePoint))
		{
		case RTNORM :
			pEntity = (db_handitem*)EntityName[0];
			if(!pEntity || pEntity->ret_type() != DB_HATCH)
			{
				pEntity = NULL;
				cmd_ErrorPrompt(CMD_ERR_SELECTHATCH, 0);
			}
			break;
		case RTCAN :
			return;
		}
	}

	db_hatch*	pHatch = (db_hatch*)pEntity;
	resbuf		rb;

	CString		MessageStr = ResourceString(IDC_HATCH_COPY_PROP, "\nProperties copied: "),
				TmpStr;

	char*		pHatchName;

	pHatch->get_2(&pHatchName);

	rb.restype = RTSTR;
	rb.resval.rstring = pHatchName;
	sds_setvar("HPNAME", &rb);

	MessageStr += ResourceString(IDC_HATCH_COPY_NAME, "Name ");
	MessageStr += pHatchName;

	if(_stricmp(pHatchName, USER) != 0)
	{
		// predefined
		rb.restype = RTREAL;
		pHatch->get_41(&rb.resval.rreal);
		sds_setvar("HPSCALE", &rb);

		ScaleSpace = rb.resval.rreal;

		MessageStr += ResourceString(IDC_HATCH_COPY_SCALE, ", Scale ");
		TmpStr.Format("%f", rb.resval.rreal);
		MessageStr += TmpStr;
	}
	else
	{
		// user
		bPredefined = false;

		rb.restype = RTREAL;
		pHatch->get_41(&rb.resval.rreal);
		sds_setvar("HPSPACE", &rb);

		ScaleSpace = rb.resval.rreal;

		MessageStr += ResourceString(IDC_HATCH_COPY_SPACING, ", Spacing ");
		TmpStr.Format("%f", rb.resval.rreal);
		MessageStr += TmpStr;

		int		iCrossFlag;
		pHatch->get_77(&iCrossFlag);
		rb.restype = RTSHORT;
		rb.resval.rint = iCrossFlag;
		sds_setvar("HPDOUBLE", &rb);
		MessageStr += ", Cross Hatch ";

		if(rb.resval.rint)
			MessageStr += ResourceString(IDC_HATCH_COPY_TRUE, "true");
		else
			MessageStr += ResourceString(IDC_HATCH_COPY_FALSE, "false");

	}

	rb.restype = RTREAL;
	pHatch->get_52(&rb.resval.rreal);
	sds_setvar("HPANG", &rb);

	HatchAng = rb.resval.rreal;

	MessageStr += ResourceString(IDC_HATCH_COPY_ANGLE, ", Angle ");
	rb.resval.rreal = rb.resval.rreal * 180/IC_PI;
	TmpStr.Format("%f", rb.resval.rreal);
	MessageStr += TmpStr;


	/*D.G.*/// Copy pattern definition.

	if(m_patternTab.m_patternDef)
		cmd_hatch_freehpdefll(&m_patternTab.m_patternDef);

	int		iNumOfLines;
	pHatch->get_78(&iNumOfLines);

	if(iNumOfLines <= 0)
		return;

	m_patternTab.m_numPatLines = iNumOfLines;

	if(!bPredefined && iNumOfLines < 2)	// We always create double pattern definition for user defined.
		iNumOfLines++;

	cmd_hpdefll		*pTmpItem = NULL,
					*pCur_To_Item = NULL;
	for(int i = 0; i < iNumOfLines; i++)
	{
		sds_real		CosAng, SinAng;

		pTmpItem = pCur_To_Item;

		pCur_To_Item = new cmd_hpdefll;
		if(!pCur_To_Item)
		{
			cmd_ErrorPrompt(CMD_ERR_MOREMEM, 0);
			cmd_hatch_freehpdefll(&m_patternTab.m_patternDef);
			return;
		}
		memset(pCur_To_Item,0,sizeof(cmd_hpdefll));

		if(i)
			pTmpItem->next = pCur_To_Item;
		else
			pTmpItem = m_patternTab.m_patternDef = pCur_To_Item;

		db_hatchpatlinelink*	pCur_From_Item = pHatch->ret_patline(bPredefined ? i : 0);


		if(bPredefined)
		{
			pCur_To_Item->angle = pCur_From_Item->ang - HatchAng;

			CosAng = cos(HatchAng);
			SinAng = -sin(HatchAng);
			pCur_To_Item->startpt[0] = (pCur_From_Item->base[0]*CosAng - pCur_From_Item->base[1]*SinAng) / ScaleSpace;
			pCur_To_Item->startpt[1] = (pCur_From_Item->base[0]*SinAng + pCur_From_Item->base[1]*CosAng) / ScaleSpace;

			CosAng = cos(pCur_From_Item->ang);
			SinAng = -sin(pCur_From_Item->ang);
			pCur_To_Item->xoffset = (pCur_From_Item->offset[0]*CosAng - pCur_From_Item->offset[1]*SinAng) / ScaleSpace;
			pCur_To_Item->yoffset = (pCur_From_Item->offset[0]*SinAng + pCur_From_Item->offset[1]*CosAng) / ScaleSpace;
		}
		else
		{
			if(i)
				pCur_To_Item->angle = IC_PI / 2.0;

			pCur_To_Item->yoffset = 1.0;
		}

		pCur_To_Item->linelen = 0.0;

		
		if(!(pCur_To_Item->linedef = new cmd_hplinedef ))
		{
			cmd_ErrorPrompt(CMD_ERR_MOREMEM, 0);
			cmd_hatch_freehpdefll(&m_patternTab.m_patternDef);
			return;
		}
		memset(pCur_To_Item->linedef,0,sizeof(cmd_hplinedef));

		pCur_To_Item->linedef->next = pCur_To_Item->linedef;

		if(pCur_From_Item->ndashes)
		{
			cmd_hplinedef*	pCur= pCur_To_Item->linedef;

			for(int j = 0; j < pCur_From_Item->ndashes; j++)
			{
				pCur->drawlength = fabs(pCur_From_Item->dash[j]) / (bPredefined ? ScaleSpace : 1.0);

				pCur_To_Item->linelen += pCur->drawlength;

				if(pCur_From_Item->dash[j] < 0.0)
					pCur->pendown = 0;
				else
					pCur->pendown = 1;

				if(j < pCur_From_Item->ndashes-1)
				{
					cmd_hplinedef*	pNext = new cmd_hplinedef ;
					if(!pNext)
					{
						cmd_ErrorPrompt(CMD_ERR_MOREMEM, 0);
						cmd_hatch_freehpdefll(&m_patternTab.m_patternDef);
						return;
					}
					memset(pNext,0,sizeof(cmd_hplinedef));
					pNext->next = pCur_To_Item->linedef;
					pCur = pCur->next = pNext;
				}
			}
		}
		else
		{
			pCur_To_Item->linedef->pendown = -1;
			pCur_To_Item->linedef->drawlength = 0.0;
			pCur_To_Item->linelen = -1.0;
		}
	}

	m_bCopied = true;

	sds_printf(MessageStr);
}
