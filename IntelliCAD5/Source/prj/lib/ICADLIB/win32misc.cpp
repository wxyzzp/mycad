/* File  : <DevDir>\source\prj\lib\icadlib\win32misc.cpp
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 */
#include "win32misc.h"
#include <locale.h>
#include <mbctype.h>

#include "..\..\icad\res\Icadrc2.h"   
#include "..\..\icad\Icadres.h"     
#include "..\..\icad\Configure.h"

/********************************************************************************
 * Author:	Dmitry Gavrilov.
 * Purpose:	Set the program locale.
 * Returns:	None.
 * Modification: MASAMI, Chikahiro @ Ebatech
 *	 Set system default codepage for NON-ANSI system.
 *	 Initialize MSC MBCS Library as system default codepage.
 ********************************************************************************/
void
ic_setlocale()
{
/*
	// EBATECH(CNBR) ]-
	//setlocale(LC_ALL, "C");
	setlocale(LC_ALL, "");
	_setmbcp(_MB_CP_LOCALE);
	// EBATECH(CNBR) ]-
*/
// EBATECH(CNBR){ TalRegionalSettings
	setlocale(LC_ALL, "C");
	setlocale(LC_CTYPE, "");
	_setmbcp(_MB_CP_LOCALE);
// }EBATECH
}

/*D.G.*/// not used code:

/*
#define STRICT
#include <windows.h>
#include "win32misc.h"

const char*
ic_getvisiosharedpath(void)
{
	static char		szPath[MAX_PATH];
	static char*	zvisiosharedpath = NULL;
	static bool		zbsharedpathsought = false;

	if(!zbsharedpathsought)
	{
		HKEY	hkCurrentVersion;
		DWORD	cbSize = MAX_PATH;
		// EBATECH(CNBR) 2002/9/24 HKLM readonly open
		long	lSuccess = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion"/DNT/, 0, KEY_READ, &hkCurrentVersion);
		//long	lSuccess = RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion"/DNT/, &hkCurrentVersion);
		if(lSuccess == ERROR_SUCCESS)
		{
			lSuccess = RegQueryValueEx(hkCurrentVersion, "CommonFilesDir"/DNT/, 0, NULL, (LPBYTE)szPath, &cbSize);
			if(lSuccess == ERROR_SUCCESS)
			{
				strcat(szPath, "\\Visio Shared" /DNT/);
				zvisiosharedpath = szPath;
			}
		}
		RegCloseKey(hkCurrentVersion);
		zbsharedpathsought = true;
	}

	return zvisiosharedpath;
}
*/

// get intellicad registry root "Software\\CompanyName\\IntelliCAD" 
// returns success/error(buffer is small)
bool getRegistryICADRoot(char* buf, int lenbuf) 
{
	char strRegName [IC_ACADBUF];
	strcpy (strRegName, TEXT("Software\\"));
	strcat (strRegName ,TEXT(ResourceString(IDS_REGISTRY_ROOT, "ITC")));
	strcat (strRegName ,TEXT("\\"));
	

#ifdef BUILD_WITH_LICENSED_COMPONENTS
	strcat (strRegName ,TEXT(ResourceString(IDS_REGISTRY_INTELLICAD_5_PROFESSIONAL, "IntelliCAD 5.0 Professional"/*DNT*/)));
#else
	strcat (strRegName ,TEXT(ResourceString(IDS_REGISTRY_INTELLICAD_5_STANDARD, "IntelliCAD 5.0 Standard"/*DNT*/)));
#endif

	if(strlen(strRegName) < (size_t)lenbuf)
	{	  
		strcpy(buf, strRegName);
		return true;
	}
	else
	{
		strncpy(buf, strRegName, lenbuf - 1);
		buf[lenbuf-1] = '\0';
		return false;
	}
}					  


// get "Software\\CompanyName\\IntelliCAD\\Profiles" 
// returns success/error(buffer is small)
bool getRegistryProfilesRoot(char* buf, int lenbuf) 
{
	char strRegName[IC_ACADBUF];
	VERIFY(getRegistryICADRoot(strRegName, IC_ACADBUF));
	strcat (strRegName ,TEXT("\\"));
	strcat (strRegName , TEXT(ResourceString(IDS_REGISTRY_PROFILES, "Profiles")));

	if(strlen(strRegName) < (size_t)lenbuf)
	{	  
		strcpy(buf, strRegName);
		return true;
	}
	else
	{
		strncpy(buf, strRegName, lenbuf - 1);
		buf[lenbuf-1] = '\0';
		return false;
	}
}

// get current profile name from registry  
// returns success/error(buffer is small)
bool GetCurrentRegistryProfileName(char* buf, int lenbuf)
{
	char strRegName [IC_ACADBUF];
	VERIFY(getRegistryProfilesRoot(strRegName, IC_ACADBUF));
	bool bNameIsFound = false;
	HKEY hKey;
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, strRegName, 0,KEY_READ, &hKey))
	{
		DWORD retType;
		unsigned long nStrLen = lenbuf;
		if(ERROR_SUCCESS == RegQueryValueEx(hKey, ""/*DNT*/, NULL, &retType, (unsigned char*)buf, &nStrLen)
			&& strlen(buf) > 0) // Windows 95/98/Me: Every key has a default value that initially does not contain data
		{
			bNameIsFound = true;
		}
		RegCloseKey(hKey);
	}
	if(!bNameIsFound)
		strcpy(buf, TEXT("Default"));
	return true;
}

// set current profile name to registry root
// returns success/error(buffer is small)
bool SetCurrentRegistryProfileName(const char *ptrName)
{
	char strRegName [IC_ACADBUF];
	VERIFY(getRegistryProfilesRoot(strRegName, IC_ACADBUF));

	HKEY hKey;
	DWORD retval;
	if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, strRegName, 0, ""/*DNT*/,
									  REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &retval))
	{
		RegSetValueEx(hKey,""/*DNT*/, 0, REG_SZ, (const unsigned char *)ptrName, strlen(ptrName) + 1);
		RegCloseKey(hKey);
		return true;
	}
	return false;
}

// get current profile name from registry root (full path name) "Software\\CompanyName\\IntelliCAD\\Profiles\\MyProfile" 
// returns success/error(buffer is small)
bool getRegCurrentProfileRoot(char* buf, int lenbuf)
{
	char strRegName [IC_ACADBUF];
	VERIFY(getRegistryProfilesRoot(strRegName,IC_ACADBUF));
	char name[IC_ACADBUF];
	VERIFY(GetCurrentRegistryProfileName(name, IC_ACADBUF));
	strcat (strRegName ,TEXT("\\"));
	strcat (strRegName,name);
	if(strlen(strRegName) < (size_t)lenbuf)
	{	  
		strcpy(buf, strRegName);
		return true;
	}
	else
	{
		strncpy(buf, strRegName, lenbuf - 1);
		buf[lenbuf-1] = '\0';
		return false;
	}
}


