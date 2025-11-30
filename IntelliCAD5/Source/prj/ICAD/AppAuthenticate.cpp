/* D:\ICADDEV\PRJ\ICAD\APPAUTHENTICATE.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * 
 * Abstract
 * 
 * Define these functions to implement an authentication mechanism.
 * 
 */ 

#include "Icad.h"
#include "IcadApp.h"
#include "LicensedComponents.h"
#include "Preferences.h"
#include "paths.h"
#include "Configure.h"
#include "modeler.h"

CLicensedComponents	CIcadApp::m_LicensedComponents;
CString				CIcadApp::m_sApplicationTitle;

BOOL CIcadApp::isTestDrive=FALSE;

bool
CIcadApp::IsVBAAccessible()		// it's for using in DLLs (in autoDll for now)
{
	return m_LicensedComponents.IsAccessible(CLicensedComponents::Components::VBA);
}

bool 
CIcadApp:: SerialNumber( LPTSTR number, DWORD size)
{
	CIntelliCadPrefKey setupKey(HKEY_CURRENT_USER, "Setup");

	bool	successful = false;

	DWORD type;
	if (ERROR_SUCCESS==RegQueryValueEx(setupKey,
									   "SerialNumber"/*DNT*/,
									   NULL, &type,
									   (unsigned char *)number, &size))
		successful = true;
	
								// return empty string if not successful
	if (!successful)
		number[0] = 0;

	return successful;
}


bool
CIcadApp:: CheckAuthentication()
{
	bool	successful = true;
	
	// TODO: Define Authentication mechanism

#ifdef 	CRIPPLE_TESTDRIVE
	if (!successful)
	{
		isTestDrive=TRUE;
		
		m_pMainWnd->MessageBox(
			ResourceString(IDS_TESTDRIVE_WELCOME, "Welcome to IntelliCAD 5 testdrive." ),
			ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),
			MB_OK | MB_ICONINFORMATION);
		
		successful = true;
	}
#endif

	if ( !successful )
		m_pMainWnd->MessageBox(
						ResourceString(IDS_NOT_AUTHENTIC, "IntelliCAD 5 failed to initialize." ),
						ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ),
						MB_OK | MB_ICONEXCLAMATION
						);

	return successful;
}

bool CIcadApp::CheckLicense()
{
	// check available licensed components
	m_LicensedComponents.SetAccessibility(CLicensedComponents::Components::RENDER, true);
	m_LicensedComponents.SetAccessibility(CLicensedComponents::Components::RASTER, true);
	m_LicensedComponents.SetAccessibility(CLicensedComponents::Components::VBA, true);
	m_LicensedComponents.SetAccessibility(CLicensedComponents::Components::SPELLCHECK, true);
	m_LicensedComponents.SetAccessibility(CLicensedComponents::Components::QRENDER, false);

	bool bEnableAcisView = false;
	bool bEnableAcisEdit = false;
#ifdef BUILD_WITH_ACIS_SUPPORT
	bEnableAcisView = true;
# ifndef BUILD_WITH_LIMITED_ACIS_SUPPORT
	bEnableAcisEdit = true;
# endif
#endif
	CModelerLicense::get()->setLicenseLevel( 
			(bEnableAcisView && bEnableAcisEdit) ? CModelerLicense::eFull :
			(bEnableAcisView || bEnableAcisEdit) ? CModelerLicense::eLimited : CModelerLicense::eNoSupport);
	
	// Set application title.
	m_sApplicationTitle  = ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD");
	m_sApplicationTitle += " ";
	m_sApplicationTitle += ResourceString(IDS_APPTITLE  , "5 by ITC");

	return true;
}
