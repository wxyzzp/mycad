/* F:\ICADDEV\PRJ\ICAD\OPTIONSPATHSTAB.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

// OptionsPathsTab.cpp : implementation file
//
#include "icad.h"
#include "OptionsPathsTab.h"
#include "paths.h"
#include <shlobj.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// OptionsPathsTab property page

IMPLEMENT_DYNCREATE(OptionsPathsTab, CPropertyPage)

OptionsPathsTab::OptionsPathsTab() : CPropertyPage(OptionsPathsTab::IDD), m_pathList(OptionPathListCtrl::PATH),
	m_fileList(OptionPathListCtrl::FILE)
{
	//{{AFX_DATA_INIT(OptionsPathsTab)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

OptionsPathsTab::~OptionsPathsTab()
{
}

void OptionsPathsTab::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(OptionsPathsTab)
	DDX_Control(pDX, OPTION_PATH_LIST, m_pathList);
	DDX_Control(pDX, OPTION_FILE_LIST, m_fileList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(OptionsPathsTab, CPropertyPage)
	//{{AFX_MSG_MAP(OptionsPathsTab)
	ON_BN_CLICKED(OPTION_PATH_BROWSE, OnPathBrowse)
	ON_BN_CLICKED(OPTION_FILE_BROWSE, OnFileBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OptionsPathsTab message handlers


BOOL OptionsPathsTab::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CMainWindow *mainWnd = SDS_CMainWindow; //(CMainWindow*) GetParentOwner();
	resbuf rb;

	//set paths
	m_pathList.InsertColumn(0, ResourceString(IDC_OPTION_PATHS, "User Paths"), LVCFMT_LEFT, 100);
	m_pathList.InsertColumn(1, ResourceString(IDC_OPTION_LOCATION, "Location"), LVCFMT_LEFT, 243);

	m_pathList.InsertItem(DRAWINGS, ResourceString(IDC_OPTION_PATH_DRAWINGS, "Drawings"));
	m_pathList.SetItemText(DRAWINGS, 1, mainWnd->m_paths->m_drawingPath);

	m_pathList.InsertItem(FONTS, ResourceString(IDC_OPTION_PATH_FONTS, "Fonts"));
	m_pathList.SetItemText(FONTS, 1, mainWnd->m_paths->m_fontPath);

	m_pathList.InsertItem(HELP, ResourceString(IDC_OPTION_PATH_HELP, "Help"));
	m_pathList.SetItemText(HELP, 1, mainWnd->m_paths->m_helpPath);

	m_pathList.InsertItem(XREFS, ResourceString(IDC_OPTION_PATH_XREFS, "Xrefs"));
	m_pathList.SetItemText(XREFS, 1, mainWnd->m_paths->m_xrefPath);

	m_pathList.InsertItem(MENUS, ResourceString(IDC_OPTION_PATH_MENUS, "Menus"));
	m_pathList.SetItemText(MENUS, 1, mainWnd->m_paths->m_menuPath);

	m_pathList.InsertItem(HATCH_PAT, ResourceString(IDC_OPTION_PATH_HATCHS, "Hatch patterns"));
	m_pathList.SetItemText(HATCH_PAT, 1, mainWnd->m_paths->m_hatchPatPath);

	m_pathList.InsertItem(BLOCKS, ResourceString(IDC_OPTION_PATH_BLOCKS, "Blocks"));
	m_pathList.SetItemText(BLOCKS, 1, mainWnd->m_paths->m_blocksPath);


	// EBATECH(CNBR) -[ 2002/6/20 Add SAVEFILEPATH
	m_pathList.InsertItem(SAVEFILEPATH, ResourceString(IDC_OPTION_PATH_SAVEFILEPATH, "Auto backup"));
	sds_getvar("SAVEFILEPATH", &rb);
	if (rb.resval.rstring){
		m_pathList.SetItemText(SAVEFILEPATH, 1, rb.resval.rstring);
		IC_FREE(rb.resval.rstring);
	} // EBATECH(CNBR) End

	m_pathList.InsertItem(TEMPLATES, ResourceString(IDC_OPTION_PATH_TEMPLATES, "Templates"));
	m_pathList.SetItemText(TEMPLATES, 1, mainWnd->m_paths->m_templatesPath);


	//set files
	m_fileList.InsertColumn(0, ResourceString(IDC_OPTION_SYSFILES, "System Files"), LVCFMT_LEFT, 100);
	m_fileList.InsertColumn(1, ResourceString(IDC_OPTION_FILE, "File"), LVCFMT_LEFT, 243);

	m_fileList.InsertItem(LOGFILE, ResourceString(IDC_OPTION_FILE_LOGFILE, "Log file"));
	sds_getvar("LOGFILENAME", &rb);
	m_fileList.SetItemText(LOGFILE, 1, rb.resval.rstring);
	if (rb.resval.rstring)
		IC_FREE(rb.resval.rstring);

	m_fileList.InsertItem(TEMPLATE, ResourceString(IDC_OPTION_FILE_TEMPLATE, "Template"));
	sds_getvar("BASEFILE", &rb);
	m_fileList.SetItemText(TEMPLATE, 1, rb.resval.rstring);
	if (rb.resval.rstring)
		IC_FREE(rb.resval.rstring);

	m_fileList.InsertItem(ALT_FONT, ResourceString(IDC_OPTION_FILE_ALTFONT, "Alternate font"));
	sds_getvar("FONTALT", &rb);
	m_fileList.SetItemText(ALT_FONT, 1, rb.resval.rstring);
	if (rb.resval.rstring)
		IC_FREE(rb.resval.rstring);

	m_fileList.InsertItem(FONT_MAP, ResourceString(IDC_OPTION_FILE_FONTMAP, "Font mapping"));
	sds_getvar("FONTMAP", &rb);
	m_fileList.SetItemText(FONT_MAP, 1, rb.resval.rstring);
	if (rb.resval.rstring)
		IC_FREE(rb.resval.rstring);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL OptionsPathsTab::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	LV_DISPINFO *dispinfo = (LV_DISPINFO*) lParam;

	if (dispinfo->hdr.code == LVN_ENDLABELEDIT)
	{
		CPropertyPage::OnNotify(wParam, lParam, pResult);

		if (wParam == OPTION_PATH_LIST)
		{
			m_pathList.ValidateData();
		}
		else
		{
			m_fileList.ValidateData();
		}

		return TRUE;
	}

	else
		return CPropertyPage::OnNotify(wParam, lParam, pResult);
}

void OptionsPathsTab::SaveListItemToSysVar(int item, int subitem, char *sysvar)
{
	resbuf rb;
    rb.restype=RTSTR;

	CString data = m_fileList.GetItemText(item, subitem);

	rb.resval.rstring= new char [ data.GetLength()+1];
    strcpy(rb.resval.rstring,(LPCTSTR)data);
    sds_setvar(sysvar, &rb);

    if(rb.resval.rstring)
		IC_FREE(rb.resval.rstring);
}
void OptionsPathsTab::OnOK()
{
	//paths
	CMainWindow *mainWnd = (CMainWindow*) SDS_CMainWindow;// GetParentOwner();
	mainWnd->m_paths->m_drawingPath = m_pathList.GetItemText(DRAWINGS, 1);
	mainWnd->m_paths->m_fontPath = m_pathList.GetItemText(FONTS, 1);
	mainWnd->m_paths->m_helpPath = m_pathList.GetItemText(HELP, 1);
	mainWnd->m_paths->m_xrefPath = m_pathList.GetItemText(XREFS, 1);
	mainWnd->m_paths->m_menuPath = m_pathList.GetItemText(MENUS, 1);
	mainWnd->m_paths->m_hatchPatPath = m_pathList.GetItemText(HATCH_PAT, 1);
	mainWnd->m_paths->m_blocksPath = m_pathList.GetItemText(BLOCKS, 1);
	mainWnd->m_paths->m_templatesPath = m_pathList.GetItemText(TEMPLATES, 1);

	// Add combination of paths to search path
	CString	searchPath, data;

	/*DG - 4.7.2003*/// avoid exceeding SRCHPATH length
	// TODO: This doesn't check for duplicate paths
	for (int i = 0; i < m_pathList.GetItemCount(); i++)
	{
		data = m_pathList.GetItemText(i, 1);

		// Go ahead and add the directory together if the data is valid,
		// but for some reason skip the BLOCKS and SAVEFILEPATH paths (this is also the case in paths.cpp)
		if( (!data.IsEmpty()) && (i!=BLOCKS) && (i!=SAVEFILEPATH) )
		{
			if(!searchPath.IsEmpty() && searchPath.Right(1) != ResourceString(IDC_OPTION_FILESEPARATOR, ";"))
			{
				if(searchPath.GetLength() + strlen(ResourceString(IDC_OPTION_FILESEPARATOR, ";")) >= db_Getsysvar()[DB_QSRCHPATH].nc)
					break;
				searchPath += ResourceString(IDC_OPTION_FILESEPARATOR, ";");
			}
			if(searchPath.GetLength() + data.GetLength() + 1 > db_Getsysvar()[DB_QSRCHPATH].nc)
			{
				data = data.Left(db_Getsysvar()[DB_QSRCHPATH].nc - searchPath.GetLength() - 1);
				int	pos = data.ReverseFind(';');
				if(pos > 0)
					searchPath += data.Left(pos);
				break;
			}
			searchPath += data;
		}
	}

	searchPath.Replace(',', ';');
	resbuf	rb;
    rb.restype = RTSTR;
    rb.resval.rstring = new char [ searchPath.GetLength() + 1];
    strcpy(rb.resval.rstring, (LPCTSTR)searchPath);

    sds_setvar("SRCHPATH"/*DNT*/,&rb);
    if(rb.resval.rstring)
		IC_FREE(rb.resval.rstring);

	// SAVEFILEPATH -[ 2002/6/28 EBATECH(CNBR)
	data  = m_pathList.GetItemText(SAVEFILEPATH, 1);
	if((i = data.Find(';')) > 0 ){
		data = data.Left(i);
	}
    rb.restype=RTSTR;
	rb.resval.rstring= new char [ data.GetLength()+1];
    strcpy(rb.resval.rstring,(LPCTSTR)data);
	// Note: setvar SAVEFILEPATH refuse unexisting path.
	if(sds_setvar("SAVEFILEPATH"/*DNT*/,&rb) != RTNORM ){
		sds_alert(ResourceString(IDC_OPTION_ERROR_SAVEFILEPATH,	"Temporary file path not found."));
	}
    if(rb.resval.rstring)
		IC_FREE(rb.resval.rstring);
	rb.resval.rstring = 0;
	//	EBATECH(CNBR) ]-

	//files
	SaveListItemToSysVar(LOGFILE, 1, "LOGFILENAME");
	SaveListItemToSysVar(TEMPLATE, 1, "BASEFILE");
	SaveListItemToSysVar(ALT_FONT, 1, "FONTALT");
	SaveListItemToSysVar(FONT_MAP, 1, "FONTMAP");

	CPropertyPage::OnOK();
}

void OptionsPathsTab::OnPathBrowse()
{
	//Ensure an item is selected
	POSITION pos = m_pathList.GetFirstSelectedItemPosition();

	if (!pos)
	{
		int result = MessageBox(ResourceString(IDC_OPTION_PATHLISTSELECT, "Please select which path you would like to define first."),
								ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),
								MB_OK | MB_ICONEXCLAMATION);

		return;

	}

	// "Shell Task Allocator"
	LPMALLOC pMalloc;
	SHGetMalloc(&pMalloc);

	char buffer[MAX_PATH];

	LPITEMIDLIST pidlRoot;

	// This struct holds the various options for the dialog
	BROWSEINFO bi;
	bi.hwndOwner = this->m_hWnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = buffer;
	bi.lpszTitle = "";
	bi.ulFlags = 0;
	bi.lpfn = NULL;

	if((pidlRoot = SHBrowseForFolder(&bi)) == NULL){
		return;
	}

	// get ASCII pathname LPITEMIDLIST struct.
	if(SHGetPathFromIDList(pidlRoot, buffer))
	{
		int nItem = m_pathList.GetNextSelectedItem(pos);

		ASSERT (nItem >= 0);

		CString path = m_pathList.GetItemText(nItem, 1);

		if (!path.IsEmpty())
			path += ResourceString(IDC_OPTION_FILESEPARATOR, ";");

		path += buffer;

		m_pathList.SetItemText(nItem, 1, path.GetBuffer(MAX_PATH));
	}

	pMalloc->Free(pidlRoot);
}

void OptionsPathsTab::OnFileBrowse()
{
	//Ensure an item is selected
	POSITION pos = m_fileList.GetFirstSelectedItemPosition();

	if (!pos)
	{
		int result = MessageBox(ResourceString(IDC_OPTION_FILELISTSELECT, "Please select which file you would like to define first."),
								ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),
								MB_OK | MB_ICONEXCLAMATION);

		return;

	}

	CString filters = "";
	CString extension = "";

	Files nItem = (Files) m_fileList.GetNextSelectedItem(pos);

	switch (nItem)
	{
	case LOGFILE:
		filters = "Log files (*.log)|*.log";
		extension = "log";
		break;

	case TEMPLATE:
		filters = "Template files (*.dwt)|*.dwt|Drawing Files (*.dwg)|*.dwg";
		extension = "dwt";
		break;

	case ALT_FONT:
		filters = "Font files (*.shx)|*.shx";
		extension = "shx";
		break;

	case FONT_MAP:
		filters = "Font map files (*.fmp)|*.fmp";
		extension = "fmp";
		break;

	default:
		//shouldn't get here;
		ASSERT(false);
		return;
	}

	filters += "|All files (*.*)|*.*||";

	CFileDialog fileDlg(TRUE, extension.GetBuffer(4), NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
						filters.GetBuffer(100), this);


	if (fileDlg.DoModal() ==  IDOK)
	{
		m_fileList.SetItemText(nItem, 1, fileDlg.GetPathName().GetBuffer(MAX_PATH));
	}
}



