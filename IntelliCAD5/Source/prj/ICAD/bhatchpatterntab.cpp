/* D:\ICADDEV\PRJ\ICAD\BHATCHPATTERNTAB.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Pattern tab of bhatch/boundary dialog
 * 
 */ 

#include "icad.h"
#include "IcadApi.h"
#include "BHatchPatternTab.h"
#include "cmds.h"
#include "BHatchDialog.h"
#include "paths.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// BHatchPatternTab dialog

const int ContainerRows = 5;
const int ContainerColumns = 3;

const int SizeContainers = ContainerRows * ContainerColumns;
long SlideContainers[SizeContainers] = {BHATCH_SLIDE1, BHATCH_SLIDE2, BHATCH_SLIDE3,
										BHATCH_SLIDE4, BHATCH_SLIDE5, BHATCH_SLIDE6,
										BHATCH_SLIDE7, BHATCH_SLIDE8, BHATCH_SLIDE9,
										BHATCH_SLIDE10, BHATCH_SLIDE11, BHATCH_SLIDE12,
										BHATCH_SLIDE13, BHATCH_SLIDE14, BHATCH_SLIDE15};

/*D.Gavrilov*/// I added this global variable (it's really in ICAD's codes bad style! sorry)
// to avoiding messages "Unable to find a hatch pattern with that name" (in cmd_hatch_loadpat,
// for example). We save the hatch name in HPNAME globally, so we'll save its file name too.
CString	g_hatchFileName = SDS_PATTERNFILE;

BHatchPatternTab::BHatchPatternTab(CWnd* pParent /*=NULL*/)
	: CDialog(BHatchPatternTab::IDD, pParent), m_patternDef(NULL), m_hatchFilePath(""),
	  m_curSlideLine(0), m_numPatterns(0), m_maxLine(0), m_tabEnabled(TRUE), m_numPatLines(0)

{
	
	//{{AFX_DATA_INIT(BHatchPatternTab)
	m_patternName = _T("");
	//}}AFX_DATA_INIT
}

BHatchPatternTab::~BHatchPatternTab() 
{
	if (m_patternDef) 	
		cmd_hatch_freehpdefll(&m_patternDef);
}


void BHatchPatternTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

//	if(pDX->m_bSaveAndValidate && ((BHatchDialog*)GetParent())->m_bCopied)		// Don't change anything.
//		return;

	//{{AFX_DATA_MAP(BHatchPatternTab)
	DDX_Control(pDX, IDC_SCROLLBAR1, m_scrollBar);
	DDX_Control(pDX, PATTERN_NAME, m_nameList);
	DDX_Control(pDX, PATTERN_FILE, m_fileCombo);
	if(!((BHatchDialog*)GetParent())->m_bCopied)		// Don't change anything.
		DDX_LBString(pDX, PATTERN_NAME, m_patternName);
	//}}AFX_DATA_MAP
}

class PatternCandidate : public AbstractFileProcessor 
	{
public:
	PatternCandidate( BHatchPatternTab& tab) : m_patternTab(tab){}
	void ProcessFile( LPCTSTR, WIN32_FIND_DATA&);

protected:
	BHatchPatternTab&	m_patternTab;
	};


void
PatternCandidate::ProcessFile( LPCTSTR, WIN32_FIND_DATA& data)
	{
	if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))	// skip directories
		m_patternTab.m_fileCombo.AddString(data.cFileName);
	}


void BHatchPatternTab::InitPatFile()
{
		// will add files found in path
	PatternCandidate	processor( *this);

	CMainWindow *mainWnd = (CMainWindow*) SDS_CMainWindow;// GetParentOwner();
	FilePath	patternPath( mainWnd->m_paths->m_hatchPatPath);
	m_fileCombo.ResetContent();

	patternPath.FindFiles( processor, "*.pat");
}

void BHatchPatternTab::InitPatList()
{
    char fs1[IC_ACADBUF], patstr[IC_ACADBUF], *fcp1, patnm[31],patdesc[81];

	CListBox *hatchList = (CListBox*) GetDlgItem(PATTERN_NAME);
	hatchList->ResetContent();
	int selection = m_fileCombo.GetCurSel();
	if (selection == CB_ERR)
		return;

	CString file;
	m_fileCombo.GetLBText(selection, file);

	if (!SearchHatchFile(file, fs1)) {
		//we should always find the path since we get the files using the hatch pattern path
		ASSERT(FALSE);
		return;
    }

	CString path = fs1;
	m_hatchFilePath = path.Left(path.ReverseFind('\\') + 1);

	FILE *fptr;
    if((fptr=fopen(fs1, "rt"/*DNT*/ ))==NULL) {
		CString error = file;
		error +=  ResourceString(IDC_HATCH_ERROR_FILE, " could not be opened.");
		MessageBox(error, ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ) , MB_OK | MB_ICONEXCLAMATION );		 
		return;
    }

	g_hatchFileName = file; /*D.Gavrilov*/// It's the only place we change g_hatchFileName.
    strcpy(patstr,"*"/*DNT*/);

    short ct;

    for(ct=0; ic_nfgets(fs1,sizeof(fs1)-1,fptr)!=0; ct++) {
        if(*fs1!='*'/*DNT*/) continue;
        if((fcp1=strchr(fs1,','/*DNT*/))==NULL) continue;
        *fcp1=0;
        strncpy(patdesc,fcp1+1,sizeof(patnm)-1);
        strncpy(patnm,fs1+1,sizeof(patdesc)-1);
		strupr(patnm);

		hatchList->AddString(patnm); 
    }

	if(fptr != NULL ) fclose(fptr); /* Albert.Isaev.ITC[31.7.2002/19:09:35] */

	m_numPatterns = hatchList->GetCount();
	m_curSlideLine =  0;

	// m_maxLine indicates the maximum top of screen line.
	m_maxLine = ((m_numPatterns + 2) / ContainerColumns) - ContainerRows;


	SCROLLINFO scrollInfo;
	scrollInfo.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
	scrollInfo.nMin = 0;
	scrollInfo.nMax = m_numPatterns / ContainerColumns; 
	scrollInfo.nPage = 5;
	scrollInfo.nPos = m_curSlideLine;
	
	m_scrollBar.SetScrollInfo(&scrollInfo, TRUE);
}

void BHatchPatternTab::EnableControls(BOOL enable)
{
	m_tabEnabled = enable;

	CWnd *wnd = GetDlgItem(PATTERN_FILE);
	wnd->EnableWindow(enable);

	wnd = GetDlgItem(PATTERN_NAME);
	wnd->EnableWindow(enable);

	m_scrollBar.EnableWindow(enable);
}

//After the dialog is initialized and the pattern name is loaded this should be called to set
//the file and the pattern data
int BHatchPatternTab::InitPatternData()
{
	if (m_patternDef)
		cmd_hatch_freehpdefll(&m_patternDef);

	CString file;
	int numlines, i;

	/*D.G.*/// Most likely the pattern name is in ñurrently selected file (if any) of the ComboBox.
	// So, try this file first.
	int		CurSel = m_fileCombo.GetCurSel();
	if(CurSel == CB_ERR)
		return RTERROR;

	m_fileCombo.GetLBText(CurSel, file);
	numlines = cmd_hatch_loadpat((LPCTSTR)file, m_patternName, &m_patternDef);

	if(numlines == RTERROR)
	{
		// if not in selected file, search all files found on path
		for(i = 0; numlines == RTERROR && i < m_fileCombo.GetCount(); i++)
		{
			if(i == m_fileCombo.GetCurSel())
				continue; // we've already searched in this file
			m_fileCombo.GetLBText(i, file);
			numlines = cmd_hatch_loadpat((LPCTSTR)file, m_patternName, &m_patternDef);
		}
	}
	else
		i = m_fileCombo.GetCurSel();

	if (numlines != RTERROR)
	{
		m_numPatLines = numlines;

		// Reinit the pattern info if the file the pattern is in is not the already selected file
		if (m_fileCombo.GetCurSel() != i)
		{
			m_fileCombo.SetCurSel(i);
			UpdateData(FALSE);

			//OnSelchangeFile will reinitialize the pattern list and lose the current pattern selection.
			//Therefore need to reset the proper pattern.
			CString tempPattern = m_patternName;
			OnSelchangeFile();
			m_patternName = tempPattern;
			UpdateData(FALSE);
		}
	}

	return numlines;
}

void BHatchPatternTab::UpdatePattern()
{
	BHatchDialog *parent = (BHatchDialog *) GetParent();

	parent->m_bCopied = false;

	parent->UndrawHatch();

	if (m_patternDef)
		cmd_hatch_freehpdefll(&m_patternDef);

	parent->m_hatch->set_2((char*)(LPCTSTR)m_patternName);
	if(strsame("SOLID"/*DNT*/, m_patternName))	/*D.Gavrilov*/// for solid fill hatches
		parent->m_hatch->set_70(1);				// support from user interface
	else
		// <alm: Fix for BugZilla No 78268>
		parent->m_hatch->set_70(0);

	//get filename
	int selection = m_fileCombo.GetCurSel();
	if (selection == CB_ERR)
		return;
	CString file;
	m_fileCombo.GetLBText(selection, file);

	int numlines = cmd_hatch_loadpat((LPCTSTR)file, m_patternName, &m_patternDef); 

	if (numlines == RTERROR)
		return;

	m_numPatLines = numlines;

	parent->RecalcPattern();

	parent->RedrawHatch();	
}



/////////////////////////////////////////////////////////////////////////////
// BHatchPatternTab message handlers


BEGIN_MESSAGE_MAP(BHatchPatternTab, CDialog)
	//{{AFX_MSG_MAP(BHatchPatternTab)
	ON_WM_PAINT()
	ON_LBN_SELCHANGE(PATTERN_NAME, OnSelchangeName)
	ON_CBN_SELCHANGE(PATTERN_FILE, OnSelchangeFile)
	ON_BN_CLICKED(BHATCH_SLIDE1, OnSlide1)
	ON_BN_CLICKED(BHATCH_SLIDE10, OnSlide10)
	ON_BN_CLICKED(BHATCH_SLIDE11, OnSlide11)
	ON_BN_CLICKED(BHATCH_SLIDE12, OnSlide12)
	ON_BN_CLICKED(BHATCH_SLIDE13, OnSlide13)
	ON_BN_CLICKED(BHATCH_SLIDE14, OnSlide14)
	ON_BN_CLICKED(BHATCH_SLIDE15, OnSlide15)
	ON_BN_CLICKED(BHATCH_SLIDE2, OnSlide2)
	ON_BN_CLICKED(BHATCH_SLIDE3, OnSlide3)
	ON_BN_CLICKED(BHATCH_SLIDE4, OnSlide4)
	ON_BN_CLICKED(BHATCH_SLIDE5, OnSlide5)
	ON_BN_CLICKED(BHATCH_SLIDE6, OnSlide6)
	ON_BN_CLICKED(BHATCH_SLIDE7, OnSlide7)
	ON_BN_CLICKED(BHATCH_SLIDE8, OnSlide8)
	ON_BN_CLICKED(BHATCH_SLIDE9, OnSlide9)
	//}}AFX_MSG_MAP

	ON_WM_VSCROLL()
END_MESSAGE_MAP()

BOOL BHatchPatternTab::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	int wNotifyCode = HIWORD(wParam); // notification code 
	int wID = LOWORD(wParam);         // item, control, or accelerator identifier 
//	HWND hwndCtl = (HWND) lParam;      // handle of control 	

	// don't know the #defined version of this message.
	if (wNotifyCode == 0)
	{
		// when "Enter" is hit send to parent's OK 
		if (wID == 1)
			return GetParent()->SendMessage(WM_COMMAND, wParam, lParam);

		// want to send escape codes to parent
		if (wID == 2)
			return GetParent()->SendMessage(WM_COMMAND, wParam, lParam);
	}
	
	return CDialog::OnCommand(wParam, lParam);
}

BOOL BHatchPatternTab::OnInitDialog() 
{
	CDialog::OnInitDialog();	
	
	InitPatFile();

	//***Set pattern file name
	//EBATECH(CNBR)-[ 2003/10/05 Default linetype file is dependent on $MEASUREMENT.
    resbuf SetGetRB;
	SDS_getvar(NULL,DB_QMEASUREMENT,&SetGetRB,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	g_hatchFileName = ( SetGetRB.resval.rint == DB_ENGLISH_MEASUREINIT ?
		SDS_PATTERNFILE : SDS_ISOPATTERNFILE);
	//EBATECH(CNBR)]-

	m_fileCombo.SelectString(-1, g_hatchFileName);	/*D.Gavrilov*/// It's the only place
													// where we use g_hatchFileName.
	InitPatList();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void BHatchPatternTab::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	RECT rect;
	CDC *slideDC;
	CWnd *slideWnd;

	int curItem = m_nameList.GetCurSel();

	CString slideName, patternName;
	//Hardcoded for test
	for (int i = 0; i < SizeContainers; i++)
	{
		slideWnd = GetDlgItem(SlideContainers[i]);	
		slideWnd->GetClientRect(&rect);
		slideDC = slideWnd->GetDC(); 

// I wanted to highlight the hatch using the system highlight color. Unfortunately, this appears to cause
// problems if the user selects a very light color.  I can also use the R2_NOT but I'm not sure I like the
// colors used to draw the text.  So for now we use black and white.
/*		if (curItem == m_slideBatch + i)
		{
			slideDC->FillRect(&rect, &CBrush(GetSysColor(COLOR_HIGHLIGHT)));
			slideDC->SetBkColor(GetSysColor(COLOR_HIGHLIGHT));

			// if the highlight color is white - set the pen to black
			if (GetSysColor(COLOR_HIGHLIGHT) == 0x00ffffff)
				slideDC->SetROP2(R2_BLACK);
			else
				slideDC->SetROP2(R2_WHITE);
		}
		else
		{
			slideDC->FillRect(&rect, &CBrush(BLACK_BRUSH));
			slideDC->SetBkColor(BLACK_BRUSH);
			slideDC->SetROP2(R2_WHITE);
		}
*/

		//fill areas before adding slide
		if (m_tabEnabled)
			slideDC->FillRect(&rect, &CBrush(BLACK_BRUSH));
		else
			slideDC->FillRect(&rect, &CBrush(GetSysColor(COLOR_INACTIVEBORDER)));

		slideDC->SetBkColor(BLACK_BRUSH);
		slideDC->SetROP2(R2_WHITE);

		if (((m_curSlideLine * ContainerColumns) + i) < m_numPatterns)
		{
			m_nameList.GetText((m_curSlideLine * ContainerColumns) + i, patternName);
			slideName = m_hatchFilePath + patternName + ".sld";

			IC_DrawSlideFile(slideName.GetBuffer(100), slideDC->GetSafeHdc(), &rect);

			if (curItem == (m_curSlideLine * ContainerColumns) + i)
				slideDC->InvertRect(&rect);
		}

		ReleaseDC(slideDC);
	}

	// Do not call CDialog::OnPaint() for painting messages
}

void BHatchPatternTab::OnSelchangeName() 
{
	UpdateData(TRUE);

	//If the pattern isn't already displayed, reset the slide line so it displayes the slides properly
	int curItem = m_nameList.GetCurSel();

	while (curItem < (m_curSlideLine * ContainerColumns))
		m_curSlideLine--;

	while (curItem > ((m_curSlideLine * ContainerColumns) + SizeContainers - 1))
		m_curSlideLine++;
		
	OnPaint();

	UpdatePattern();
}


void BHatchPatternTab::OnSelchangeFile() 
{
	UpdateData(TRUE);
	InitPatList();
	OnPaint();

	BHatchDialog *parent = (BHatchDialog*) GetParent();

	//set iso width field if file is iso.
	if (parent)
		parent->m_patPropTab.EnableControls();
}

void BHatchPatternTab::OnVScroll(UINT nCode, UINT nPos, CScrollBar *pScrollBar)
{
	switch (nCode)
	{
	case SB_LINEUP:
		m_curSlideLine--;
		break;

	case SB_PAGEUP:
		m_curSlideLine -= ContainerRows;
		break;

	case SB_THUMBPOSITION:
		m_curSlideLine = nPos;	
		break;

	case SB_PAGEDOWN:
		m_curSlideLine += ContainerRows;
		break;

	case SB_LINEDOWN:
		m_curSlideLine++;
		break;

	default:
		return;
	}

	// make sure it isn't out of range
	if (m_curSlideLine > m_maxLine)
		m_curSlideLine = m_maxLine;

	if (m_curSlideLine < 0)
		m_curSlideLine = 0;

	if (pScrollBar)
		pScrollBar->SetScrollPos(m_curSlideLine, TRUE);

	OnPaint();
}

void BHatchPatternTab::ClickSlide(int slideNum)
{
	if (m_tabEnabled)
	{
		m_nameList.SetCurSel((m_curSlideLine * ContainerColumns) + slideNum - 1);

		UpdateData(TRUE);

		OnPaint();
		UpdatePattern();
	}
}

void BHatchPatternTab::OnSlide1() 
{
	ClickSlide(1);
}

void BHatchPatternTab::OnSlide2() 
{
	ClickSlide(2);
}

void BHatchPatternTab::OnSlide3() 
{
	ClickSlide(3);
}

void BHatchPatternTab::OnSlide4() 
{
	ClickSlide(4);
}

void BHatchPatternTab::OnSlide5() 
{
	ClickSlide(5);
}

void BHatchPatternTab::OnSlide6() 
{
	ClickSlide(6);
}

void BHatchPatternTab::OnSlide7() 
{
	ClickSlide(7);
}

void BHatchPatternTab::OnSlide8() 
{
	ClickSlide(8);
}

void BHatchPatternTab::OnSlide9() 
{
	ClickSlide(9);
}

void BHatchPatternTab::OnSlide10() 
{
	ClickSlide(10);
}

void BHatchPatternTab::OnSlide11() 
{
	ClickSlide(11);
}

void BHatchPatternTab::OnSlide12() 
{
	ClickSlide(12);
}

void BHatchPatternTab::OnSlide13() 
{
	ClickSlide(13);
}

void BHatchPatternTab::OnSlide14() 
{
	ClickSlide(14);
}

void BHatchPatternTab::OnSlide15() 
{
	ClickSlide(15);
}

