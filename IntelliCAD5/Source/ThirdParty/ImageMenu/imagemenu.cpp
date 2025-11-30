//-----------------------------------------------------------------------------
//  imagemenu.CPP
//  Copyright (C) 1998 by Visio Corporation
//
//  THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED
//  WARRANTY.  ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR
//  PURPOSE AND OF MERCHANTABILITY ARE HEREBY DISCLAIMED.
//

//  Description:
//
//  Revision History:
//  Original started: 3/5/98       Author: David Gates
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "imagemenu.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <io.h>
// Modified Cybage SBD 21/05/2001 [ 
// Modified Cybage PP 13/04/2001 
// Reason: Mail from Surya, not being able to compile cleanly.
//#include "..\..\prj\api\sds\sds.h"
#include "sds.h"
// Modified Cybage SBD 21/05/2001 ] 
#include "menuread.h"
 
// Utility definition to get an array's element count (at compile time).
//   For  example:
//
//   int  arr[] = {1,2,3,4,5};
//
//   printf("%d", ARRAYELEMENTS(arr));
//
// would print a five.  ARRAYELEMENTS("abc") can also be used to tell how
// many bytes are in a string constant INCLUDING THE TRAILING NULL.
#define ARRAYELEMENTS(array) (sizeof(array)/sizeof((array)[0]))

// Define the structure of the table: a string giving the IntelliCAD name
// of the function, and a pointer to a function returning type int.
struct func_def { char *func_name; int (*func) (struct sds_resbuf *); };

// Define the array of function names and handlers.
static struct func_def func_table[] = {
    {"imagemenu"    , imagemenu},
};
// To add more functions to this table, just put them in the list, after
// declaring the function names.

char      adsw_AppName[512];
char      ads_appname[512];
HWND      adsw_hwndAcad;
HINSTANCE adsw_hInstance;

HWND      adsw_hWnd;
int       adsw_wait;

sds_matrix ads_identmat;

// Global Command string
char g_Cmd[256] = "";

// Protos
int SDS_main   (int, char**);
extern "C" int SDS_GetGlobals(char *appname,HWND *hwnd,HINSTANCE *hInstance);
void __declspec(dllexport) SDS_EntryPoint(HWND hWnd);


void __declspec(dllexport) SDS_EntryPoint(HWND hWnd) {
	
	int i,j;
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	for(i=0; i<=3; i++) for(j=0; j<=3; j++)	ads_identmat[i][j]=0.0;
	for(i=0; i<=3; i++) ads_identmat[i][i]=1.0;

	SDS_GetGlobals(adsw_AppName,&adsw_hwndAcad,&adsw_hInstance);
	strncpy(ads_appname,adsw_AppName,sizeof(ads_appname)-1);
	SDS_main(0,NULL);
    return;
}


// The code from here to the end of invokefun() is UNCHANGED when you add or delete
// functions.

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/* MAIN - the main routine */

int main(int argc, char** argv)
{
   short scode = RSRSLT;             // Normal result code (default)
   int stat;
   char errmsg[80];

   sds_init(argc, argv);             // Open communication with lisp

   for ( ;; ) {                      // Request/Result loop
       if ((stat = sds_link(scode)) < 0) {
           sprintf(errmsg, "Templt: bad status from sds_link() = %d\n", stat);
           sds_printf(errmsg);
           sds_exit(-1);
       }
       scode = RSRSLT;               // Reset result code

       switch (stat) {
       case RQXLOAD:                 // Load & define functions
           scode = funcload() == RTNORM ? RSRSLT : RSERR;
           break;
       case SDS_RQXUNLD:
           break;
       case RQSUBR:                  // Handle external function requests
           scode = invokefun() == RTNORM ? RSRSLT : RSERR;
           break;
       case SDS_RQSAVE:
           break;
       case SDS_RQEND:
           break;
       case SDS_RQQUIT:
           break;
       case SDS_RQCFG:
           break;
       case SDS_RQHUP:
           break;
       case SDS_RQXHELP:
           break;
       default:
           break;
       }
   }
   return(1);
}

//-----------------------------------------------------------------------------
// FUNCLOAD  --  Define this application's external functions.  Return
//               RTERROR on error, else RTNORM.

static int funcload()
{
    short i;

    for (i = 0; i < ARRAYELEMENTS(func_table); i++) {
        if (!sds_defun(func_table[i].func_name, i))
            return RTERROR;
    }
    return RTNORM;
}

//-----------------------------------------------------------------------------
// invokefun -- Execute external function (called upon an RQSUBR request).
//              Return value from the function executed, RTNORM or RTERROR.

static int invokefun()
{
    struct sds_resbuf *rb;
    int val;

    // Get the function code and check that it's within range.
    // (It can't fail to be, but paranoia doesn't hurt.)
    if ((val = sds_getfuncode()) < 0 || val >= ARRAYELEMENTS(func_table)) {
        sds_fail("Received nonexistent function code.");
        return RTERROR;
    }

    // Fetch the arguments, if any.
    rb = sds_getargs();

    // Call the handler and return its success-failure status.
    val = (*func_table[val].func)(rb);
    sds_relrb(rb);
    return val;

}
//-----------------------------------------------------------------------------
// The code from the beginning of main() to here is UNCHANGED when you add or
// delete functions.
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// imagemenu
//
static int imagemenu(struct sds_resbuf *rb)
{
	// Check that rb contains two string parameters
	if (!rb) {
		sds_printf("\nMust supply parameters\n");
		return RTERROR;
	}
	if (rb->restype != RTSTR) {
		sds_printf("\nFirst parameter must be a string of the menu file name\n");
		return RTERROR;
	}
	if (!rb->rbnext) {
		sds_printf("\nTwo parameters must be supplied\n");
		return RTERROR;
	}
	if (rb->rbnext->restype != RTSTR) {
		sds_printf("\nThe second parameter must be a string of the starting image section\n");
		return RTERROR;
	}

	// Find the file from the first parameter passed in.
	char sFile[512];
	if (sds_findfile(rb->resval.rstring, sFile) != RTNORM) {
		sds_printf("\nFile %s not found\n", rb->resval.rstring);
		return RTERROR;
	}

	// Get the image section passed in from the second parameter.
	char sSection[256];
	strcpy(sSection, "**");
	strcat(sSection, rb->rbnext->resval.rstring);
	strcat(sSection, "\n");

	strcpy(g_Cmd, "");
	CReadMenu rm;
	char *psTemp;
	while (rm.Do(sFile, sSection)) {
		if (strlen(g_Cmd)) {
			if (!strncmp(g_Cmd, "$I=", 3)) {
				strcpy(sSection, "**");
				strcat(sSection, g_Cmd + 3);
				psTemp = strchr(sSection, ' ');
				if (psTemp)
					strcpy(psTemp, "");
				strcat(sSection, "\n");
			} else {
				sds_sendmessage(g_Cmd);
				break;
			}
		}
	}
	return RTNORM;
}


/////////////////////////////////////////////////////////////////////////////
// CImagemenuApp

BEGIN_MESSAGE_MAP(CImagemenuApp, CWinApp)
	//{{AFX_MSG_MAP(CImagemenuApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImagemenuApp construction

CImagemenuApp::CImagemenuApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CImagemenuApp object

CImagemenuApp theApp;
