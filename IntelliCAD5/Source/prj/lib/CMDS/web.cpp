/* F:\ICADDEV\PRJ\LIB\CMDS\WEB.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#include "cmds.h"

// Web functions and defines from Visio


#define cchlen(X)	(sizeof(X) / sizeof(X[0]))
#define FSTRLEN( S )				strlen( S )
#define FSTRCPY( D, S ) 			strcpy( D, S )
#define FSTRCAT( D, S ) 			strcat( D, S )
#define FSTRSTR( S1, S2 )			strstr( S1, S2 )

/***********************************************************************
 *						 The William Files
 *
 *	Stolen from Williams' CLinkWeb.
 ***********************************************************************/

/***********************************************************************
 *+	GetRegisteredBrowser
 *
 *	Find the browser registered to handle .HTML or .HTM files.
 *
 */

HRESULT GetRegisteredBrowser(		// SUCCESS if we rocked.
	LPTSTR pBuf)					//	o:Output Buffer
	{
	HRESULT err = E_FAIL;
	HKEY hKey = NULL;
	HKEY hKeyApp = NULL;
	DWORD dwCt, dwType = 0;
	LRESULT lRet = ERROR_SUCCESS;
	LPTSTR pszTmp = NULL;
	TCHAR szKey[MAX_PATH];

	/*	First we try the .html handler and see if we get a handler.
	 *
	 *	Bug #107838-With Netscape Communicator 4.0 they register the
	 *	.html handler but the value is empty, hence we changed the
	 *	algorithm to drop through to .htm if the key is empty.	The
	 *	setup does register the .html key correctly.
	 */
	// EBATECH(CNBR) 2002/9/24 Use KEY_READ instead of KEY_ALL_ACCESS
	lRet = RegOpenKeyEx(HKEY_CLASSES_ROOT, ".html",
						0, KEY_READ, &hKey);

	if ( ERROR_SUCCESS == lRet )
		{
		dwCt = sizeof(szKey);
		RegQueryValueEx(hKey, "", NULL, &dwType, (LPBYTE)szKey, &dwCt);
		RegCloseKey(hKey);
		hKey = NULL;
		}

	if ( (ERROR_SUCCESS != lRet) || !szKey[0] )
		{
		lRet = RegOpenKeyEx(HKEY_CLASSES_ROOT, ".htm",
							0, KEY_READ, &hKey);

		if ( ERROR_SUCCESS != lRet )
			goto CU;

		dwCt = sizeof(szKey);
		RegQueryValueEx(hKey, "", NULL, &dwType, (LPBYTE)szKey, &dwCt);
		RegCloseKey(hKey);
		hKey = NULL;

		if ( !szKey[0] )
			goto CU;
		}

	/*	Default browser
	 *	hKey =
	 *	HKEY_CLASSES_ROOT\NetscapeMarkup
	 */
	lRet = RegOpenKeyEx(HKEY_CLASSES_ROOT, szKey,
						0, KEY_READ, &hKey );

	if ( ERROR_SUCCESS != lRet )
		goto CU;

	/*	Command line for browser
	 *	hKey =
	 *	HKEY_CLASSES_ROOT\NetscapeMarkup\shell\open\command
	 */
	ZeroMemory(szKey, sizeof(szKey));

	lRet = RegOpenKeyEx(hKey, "shell\\open\\command",
						0, KEY_READ, &hKeyApp );

	if ( ERROR_SUCCESS != lRet )
		goto CU;

	dwCt = MAX_PATH;
	RegQueryValueEx(hKeyApp, "", NULL, &dwType, (LPBYTE)pBuf, &dwCt);

	if( !(*pBuf) )
		goto CU;

	err = ERROR_SUCCESS;
CU:
	RegCloseKey(hKeyApp);
	RegCloseKey(hKey);

	return err;
	}


/***********************************************************************
 *+	OpenWebDoc
 *
 *	ABSTRACT
 *		Launch web browser with URL on command line.
 *
 *	ASSUMPTIONS
 *		pURL is quoted only if it contains white space.
 */

HRESULT OpenWebDoc(
	LPCTSTR pURL)					// i :URL to Navigate
	{
	HRESULT err = E_FAIL;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	TCHAR chBrowser[(2*MAX_PATH) + 1];
	TCHAR chURL[MAX_PATH];
	LPTSTR pTemp = NULL;

	chURL[0] = 0;
	chBrowser[0] = 0;

	/*	Check parameters.
	 */
	if (!pURL)
		goto CU;

	/*	See if there's anyone out there to talk to before we do
	 *	anything stupid.
	 */
	if (err = GetRegisteredBrowser(chBrowser))
		goto CU;

	if (cchlen(chBrowser) <= (FSTRLEN(chBrowser) + FSTRLEN(pURL)) )
		goto CU;

	/*	If no quotes are on URL then add em.
	 */
	if ( '\"' != *pURL )
		{
		FSTRCPY(chURL, "\"");
		FSTRCAT(chURL, pURL);
		FSTRCAT(chURL, "\"");
		}
	else
		FSTRCPY(chURL, pURL);

	/*	Build command line string.
	 */
	if ( pTemp = FSTRSTR(chBrowser, "%1") )
		{
		TCHAR chTemp[MAX_PATH];

		/*	Copy remainder of string for save keeping.
		 */
		FSTRCPY(chTemp, pTemp + 2);

		/*	Assume that if there is a quote immediately before the
		 *	argument position then they've already handled quotes for us.
		 */
		if ( '\"' == *(pTemp - 1) )
			FSTRCPY(pTemp - 1, chURL);
		else
			FSTRCPY(pTemp, chURL);

		if ( '\"' == *chTemp )
			FSTRCAT(chBrowser, chTemp + 1);
		else
			FSTRCAT(chBrowser, chTemp);
		}
	else
		{
		FSTRCAT(chBrowser, " ");
		FSTRCAT(chBrowser, pURL);
		}

	/*	Fire off the process.
	 */
	memset(&pi, 0, sizeof(pi));
	memset(&si, 0, sizeof(si));
	si.cb  = sizeof(si);

	CreateProcess(	NULL,			//filename
					chBrowser,		//Command line
					NULL,			//proccesss attributes
					NULL,			//thread attributes
					FALSE,			//inherit handles
					CREATE_NEW_PROCESS_GROUP | NORMAL_PRIORITY_CLASS,	//flags
					NULL,			//environment variables
					NULL,			//current directory
					&si,			//startup info
					&pi);			//proccess info

CU:
	return err;
	}


