// DCLParsr.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DCLParsr.h"
#include "DCLParsrDlg.h"
#include "main.h"
#include "constants.h"
#include "treedebugger.h"
#include "errno.h"
#include "fileinfo.h"
#include "sds.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


Main g_Main;

//BOOL g_bErrorOccured = FALSE;     //flag  set  to true when  error occurs 

/////////////////////////////////////////////////////////////////////////////
// CDCLParsrApp

BEGIN_MESSAGE_MAP(CDCLParsrApp, CWinApp)
	//{{AFX_MSG_MAP(CDCLParsrApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDCLParsrApp construction

CDCLParsrApp::CDCLParsrApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDCLParsrApp object

CDCLParsrApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDCLParsrApp initialization

BOOL CDCLParsrApp::InitInstance()
	{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need.

	TreeDebugger debug;                 // This varaible writes the tree generated during parsing to a file.
	FileInfo *pFileInfo =  NULL;         
	POSITION pos;
	int ndlgid;               
	int ndlgidtodelete;
	int nReturn;
	BOOL bTest = FALSE;
	FILE *fp;

	CString szFilter = " DCL Files(*.dcl)|*.dcl|AllFiles(*.*) | *.*||";
	CFileDialog fOpenDialog(TRUE/*Open dialog box*/,_T(".dcl"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,szFilter);
	
	nReturn  = g_Main.Init(_T(""),TRUE);         // Call the main init function to load the primitives & base DCL files from resource.
    	
	if ( nReturn ==  SDS_RTERROR)
		{
		TRACE("Error Initailizing Parser\n");
		return FALSE;
		}
    
	     
	while ( fOpenDialog.DoModal() ==  IDOK )
		{ 

		if ( remove(TEST_OUT) == EACCES )
					AfxMessageBox("Unable to overwrite file test.out");


        int nDlgReturn; 
		nDlgReturn = g_Main.dlg_load_dialog(fOpenDialog.m_ofn.lpstrFile,&ndlgid); 

        if ( nDlgReturn == SDS_RTERROR && g_Main.m_DCLParser.get_AuditLevel() != 0 )
			{
			AfxMessageBox("Error in DCL File");
			}

		// to test unloading of intermediate files
		if ( bTest == FALSE )
			{
			bTest = TRUE;
			ndlgidtodelete = ndlgid;
			}

		}
        pos = g_Main.m_DCLParser.m_ListOfDCLFileData.GetTailPosition() ;
		 
		// To write the tree details  generated during parsing of DCL files.
		while  ( pos != NULL )  
			{
			pFileInfo =(FileInfo*) g_Main.m_DCLParser.m_ListOfDCLFileData.GetPrev(pos);
		    fp = fopen(TEST_OUT,"a+");
 
			if ( !fp )
				{
				TRACE("Unable to open test file");
				break;
				}
            
			// Write the DCL Filename and the fileid.
			fprintf(fp,"\nFileName=%s,FileID=%d\n",pFileInfo->get_DCLFileName(),pFileInfo->get_DCLFileID()); 
			fclose(fp);
			if ( pFileInfo )
				{
				// Call the function to write details of each tree generated.
				debug.LogParsedTree(pFileInfo); 
				} 
			if ( pos == NULL )
				break;
			}
			    


	g_Main.dlg_unload_dialog(ndlgidtodelete);


	g_Main.Exit(); 

	

	// Since the dialog has been closed, return FALSE so that we exit the
	// application, rather than start the application's message pump.
	return FALSE;
}


















