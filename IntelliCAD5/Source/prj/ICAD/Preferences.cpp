/* D:\ICADDEV\PRJ\ICAD\PREFERENCES.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * 
 * Abstract
 * 
 * Common utilities for managing user preferences.
 * 
 */ 

#include "Icad.h"
#include "IcadCursor.h"
#include "Preferences.h"
#include "win32misc.h"

CString PreferenceKey::m_applicationRoot;


void PreferenceKey::Init(HKEY tree, const CString& subkey, bool create)         // Modify by SystemMetrix(Maeda) 09.10.2001
    {
    CRegKey root;
    
        // initialize static string for application root in registry
    if ( m_applicationRoot EQ "" )
        {
        CString     temp;
        // EBATECH-[Create by Applicaton name(AFX_IDS_APP_TITLE) and RegistryRoot(IDS_REGISTRY_ROOT)
        /*
        const int   length = 100;
        LPTSTR      buffer = temp.GetBuffer( length);

                                // don't rely on CString::LoadString in static constructor
        int loaded = ::LoadString( ::GetModuleHandle( NULL), IDS_REGISTRY_ROOT, buffer, length);

                                // last resort if string resource fails
        if ( !loaded )  
        {
            //Modified Cybage VSB 13/04/2001 DD/MM/YYYY [
            //Reason : Mail from Surya Sarda dated 06/04/2001 
            //temp = TEXT( "Software\\ITC\\IntelliCAD");
            temp = TEXT( ResourceString( IDS_REGISTRY_ROOT, "Software\\ITC\\IntelliCAD"));
            //Modified Cybage VSB 13/04/2001 DD/MM/YYYY ]
        }
        */
        // get registry key name (check /p option)
        getRegCurrentProfileRoot(temp.GetBuffer(IC_ACADBUF),IC_ACADBUF); // Modify by SystemMetrix(Maeda) 09.10.2001
	temp.ReleaseBuffer();
        // ]-EBATECH

        m_applicationRoot = temp;
        }
    
    if (create)
        {
        root.Create( tree, m_applicationRoot);
        VERIFY( Create( root, subkey) EQ ERROR_SUCCESS );
        }
    else
        {
        if(root.Open(tree, m_applicationRoot, KEY_READ | KEY_CREATE_SUB_KEY) == ERROR_SUCCESS)
			Open(root, subkey);
        }
    }

PreferenceKey::PreferenceKey( HKEY tree, const CString& subkey, bool create)    // Add by SystemMetrix(Maeda)09.10.2001
    {
		//BugZilla No. 78379 ; 25-11-2002
		//store initial application root

	CString strApplicationRoot= m_applicationRoot;
		if ( tree == HKEY_CURRENT_USER && subkey == "Setup" )
		{
			//"Setup" info is stored in 'Software\ITC\IntelliCAD' and not for particular user.
			int index = m_applicationRoot.ReverseFind('\\');
			if ( index != -1 )
				m_applicationRoot = m_applicationRoot.Left(index);
		}			
		Init(tree, subkey, create);
		//re-store application root
		m_applicationRoot = strApplicationRoot;
    }

void PreferenceKey::Reload(HKEY tree, const CString& subkey, bool create)       // Add by SystemMetrix(Maeda)09.10.2001
    {
    m_applicationRoot.Empty();
    Init(tree, subkey, create);
    }

LONG PreferenceKey::FlushKey(HKEY tree, const CString& keyname)
    {
    CRegKey root;
    root.Open(tree, m_applicationRoot);
    root.RecurseDeleteKey(keyname);
    return Create(root, keyname);
    }

void CIntelliCadPrefKey::Init(HKEY tree, const CString& subkey, bool create)         // Modify by SystemMetrix(Maeda) 09.10.2001
    {
	CRegKey root;
	getRegistryICADRoot(m_IntelliCadRoot.GetBuffer(IC_ACADBUF), IC_ACADBUF);
	m_IntelliCadRoot.ReleaseBuffer();
    
    if (create)
        {
        root.Create( tree, m_IntelliCadRoot);
        VERIFY( Create( root, subkey) EQ ERROR_SUCCESS );
        }
    else
        {
        root.Open(tree, m_IntelliCadRoot, KEY_READ | KEY_CREATE_SUB_KEY);
        Open(root, subkey);
        }
    }

CIntelliCadPrefKey::CIntelliCadPrefKey( HKEY tree, const CString& subkey, bool create)
    {
    Init(tree, subkey, create);
    }

void CIntelliCadPrefKey::Reload(HKEY tree, const CString& subkey, bool create)      
    {
    m_IntelliCadRoot.Empty();
    Init(tree, subkey, create);
    }

LONG CIntelliCadPrefKey::FlushKey(HKEY tree, const CString& keyname)
    {
    CRegKey root;
    root.Open(tree, m_IntelliCadRoot);
    root.RecurseDeleteKey(keyname);
    return Create(root, keyname);
    }


BooleanPreference::BooleanPreference(CRegKey& parent, const CString name, bool defaultValue, bool bReadData)
		: RegistryPreference(parent), m_name(name), m_default(defaultValue) 
{ 
	if(bReadData) 
		readFromRegistry(); 
	else
		m_value = m_default;
}

bool BooleanPreference::readFromRegistry() 
{
    DWORD   value;
    if ( m_key.QueryValue( value, m_name) EQ ERROR_SUCCESS )
        m_value = value ? true : false;
    else
        m_value = m_default;
	return true;
}

void BooleanPreference::setKey()
{
	if(m_value == m_default)
		m_key.DeleteValue(m_name);
	else
		VERIFY(m_key.SetValue(m_value, m_name) == ERROR_SUCCESS);
}


IntPreference::IntPreference(CRegKey& parent, const CString name, int defaultValue, bool bReadData) 
		: RegistryPreference(parent), m_name( name), m_default( defaultValue) 
{ 
	if(bReadData) 
		readFromRegistry(); 
    else
        m_value = m_default;
}

IntPreference::~IntPreference()
{
    if ( m_value EQ m_default )
        m_key.DeleteValue( m_name);
    else
        VERIFY( m_key.SetValue( m_value, m_name) EQ ERROR_SUCCESS );
}


bool IntPreference::readFromRegistry()
{
    DWORD   value;
    if ( m_key.QueryValue( value, m_name) EQ ERROR_SUCCESS )
        m_value = (int)value;
    else
        m_value = m_default;

	return true;
}


StringPreference::StringPreference(CRegKey& parent, const CString& name, CString defaultValue, bool bReadData)
		: RegistryPreference(parent), m_name(name), m_default(defaultValue) 
{ 
	if(bReadData) 
		readFromRegistry(); 
	else
		m_value = m_default;
}

StringPreference::~StringPreference()
{
	if(m_value == m_default)
		m_key.DeleteValue(m_name);
	else
		VERIFY(m_key.SetValue(m_value, m_name) == ERROR_SUCCESS);
}

bool StringPreference::readFromRegistry()
{
	DWORD	size = MAX_PATH;
	char	value[MAX_PATH] = "";

	if(m_key.QueryValue(value, m_name, &size) == ERROR_SUCCESS)
		m_value = value;
	else
		m_value = m_default;

	return true;
}


void DirStringPreference::setToCurDir()
{
	if(m_value.IsEmpty())
	{
		DWORD	len = GetCurrentDirectory(0, NULL);
		if(len)
		{
			TCHAR*	buf = new TCHAR[len];
			if(buf)
			{
				if(GetCurrentDirectory(len, buf))
					m_value = buf;
				delete [] buf;
			}
		}
	}
}


PreferenceKey		configKey( HKEY_CURRENT_USER, "Config", FALSE);	// Modify by SystemMetrix(Maeda) 09.10.2001
BooleanPreference	UserPreference::crosshairPreference( configKey, "AlwaysUseCrosshair", false, false);
IntPreference		UserPreference::crosshairSizePreference( configKey, "CursorSize", 5, false);
//BugZilla No. 78432  ; 03-03-2003
//BooleanPreference	UserPreference::GripsAttractionPreference(configKey, "EnableAttraction", true, false);
BooleanPreference	UserPreference::GripsAttractionPreference(configKey, "EnableAttraction", false, false);
IntPreference		UserPreference::GripsAttractionDistancePreference(configKey, "AttractionDistance", 3, false);

BooleanPreference	UserPreference::ContinuousMotionPreference(configKey, "ContinuousMotion", false, false);

// DP: Now we save and load this variable together with others.
//IntPreference     UserPreference::OSModePreference(configKey, "OSMODE", 0, false);

BooleanPreference	UserPreference::SnapFlyOverEnablePreference(configKey, "EnableFlyOver", true, false);
BooleanPreference	UserPreference::SnapMarkerPreference(configKey, "DisplaySnapMarker", true, false);
IntPreference		UserPreference::SnapMarkerSizePreference(configKey, "SnapMarkerSize", 6, false);
IntPreference		UserPreference::SnapMarkerThicknessPreference(configKey, "SnapMarkerThickness", 2, false);
IntPreference		UserPreference::SnapMarkerColorPreference(configKey, "SnapMarkerColor", 2, false);
BooleanPreference	UserPreference::SnapMarkerDrawInAllViews(configKey, "SnapMarkerDrawInAllViews", false, false);
BooleanPreference	UserPreference::SnapTooltipsPreference(configKey, "DisplayTooltips", true, false);
BooleanPreference	UserPreference::SnapApertureBoxPreference(configKey, "DisplaySnapApertureBox", true, false);
BooleanPreference	UserPreference::SnapShowOsnapCursorDecorationPreference(configKey, "ShowOsnapCursorDecoration", false, false);
BooleanPreference   UserPreference::s_bEnableHyperlinkMenu(configKey, "EnableHyperlinkMenu", true, false);
BooleanPreference   UserPreference::s_bEnableHyperlinkTooltip(configKey, "EnableHyperlinkTooltip", false, false);

PreferenceKey		plotConfigKey(HKEY_CURRENT_USER, "Config\\plot", FALSE);
BooleanPreference	UserPreference::s_bSaveToLayout(plotConfigKey, "SaveChangeToLayout", true, false);
BooleanPreference	UserPreference::s_bUsePrintSettingsFromRegistry(plotConfigKey, "UseRegistryValues", false, false);
StringPreference	UserPreference::s_MtextPosition(configKey,"MtextPosition","218 284 813 569",false);
#ifdef USE_ICAD_SPOOLING
BooleanPreference	UserPreference::s_bForcePrintToFile(plotConfigKey, "ForcePrintToFile", false, false);
BooleanPreference	UserPreference::s_bUseOutputFolder(plotConfigKey, "UseOutputFolder", true, false);
DirStringPreference	UserPreference::s_outputFolder(plotConfigKey, "OutputFolder", "", false);
#endif

// must follow preference declarations for proper construction

IcadCursor  cursorSingleton;


void UserPreference::readFromRegistry()
{
    UserPreference::crosshairPreference.readFromRegistry();
    UserPreference::crosshairSizePreference.readFromRegistry();
    UserPreference::GripsAttractionPreference.readFromRegistry();
    UserPreference::GripsAttractionDistancePreference.readFromRegistry();
    UserPreference::ContinuousMotionPreference.readFromRegistry();
    //UserPreference::OSModePreference.readFromRegistry();
    UserPreference::SnapFlyOverEnablePreference.readFromRegistry();
    UserPreference::SnapMarkerPreference.readFromRegistry();
    UserPreference::SnapMarkerSizePreference.readFromRegistry();
    UserPreference::SnapMarkerThicknessPreference.readFromRegistry();
    UserPreference::SnapMarkerColorPreference.readFromRegistry();
    UserPreference::SnapMarkerDrawInAllViews.readFromRegistry();
    UserPreference::SnapTooltipsPreference.readFromRegistry();
    UserPreference::SnapApertureBoxPreference.readFromRegistry();
    UserPreference::SnapShowOsnapCursorDecorationPreference.readFromRegistry();
    UserPreference::s_bEnableHyperlinkMenu.readFromRegistry();
    UserPreference::s_bEnableHyperlinkTooltip.readFromRegistry();
	UserPreference::s_bSaveToLayout.readFromRegistry();
	UserPreference::s_bUsePrintSettingsFromRegistry.readFromRegistry();
	UserPreference::s_MtextPosition.readFromRegistry();

#ifdef USE_ICAD_SPOOLING
	UserPreference::s_bForcePrintToFile.readFromRegistry();
	UserPreference::s_bUseOutputFolder.readFromRegistry();
	UserPreference::s_outputFolder.readFromRegistry();
#endif
}



