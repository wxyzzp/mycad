/* D:\ICADDEV\PRJ\ICAD\PREFERENCES.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * 
 * Abstract
 * 
 * Common utilities for managing user preferences.
 * 
 */ 

#ifndef _PREFERENCES_H
#define _PREFERENCES_H


/*  PreferenceKey is just a simple way to open a static named key
 *  All PreferenceKeys are subkeys of the application root.
 */

class PreferenceKey : public CRegKey
    {
public:
    PreferenceKey( HKEY tree, const CString& subkey, bool create = TRUE);
    LONG FlushKey(HKEY tree, const CString& keyname);
    void Init(HKEY tree, const CString& subkey, bool create = TRUE);            // Add by SystemMetrix(Maeda)09.10.2001
    void Reload(HKEY tree, const CString& subkey, bool create = TRUE);          // Add by SystemMetrix(Maeda)09.10.2001

private:
    static  CString m_applicationRoot;
    };

class CIntelliCadPrefKey : public CRegKey
    {
public:
    CIntelliCadPrefKey( HKEY tree, const CString& subkey, bool create = TRUE);
    LONG FlushKey(HKEY tree, const CString& keyname);
    void Init(HKEY tree, const CString& subkey, bool create = TRUE);            // Add by SystemMetrix(Maeda)09.10.2001
    void Reload(HKEY tree, const CString& subkey, bool create = TRUE);          // Add by SystemMetrix(Maeda)09.10.2001

private:
    CString m_IntelliCadRoot;
    };

/*  Registry preference classes support loading and saving of preference values
 *  to the registry upon application initialization and exit.
 *
 *  Each preference value has a name and exists within a parent key in the registry.
 *  The value is saved if it is different from the default value, otherwise the value
 *  is deleted from the registry.
 *
 */

    // base class for various value types
class RegistryPreference : public CRegKey
    {
public:
    RegistryPreference( CRegKey& key) : m_key( key) {}
	virtual bool readFromRegistry() = 0;
protected:
    CRegKey&    m_key;
    };

    // Boolean preference value
class BooleanPreference : public RegistryPreference
    {
public:
    BooleanPreference(CRegKey& parent, const CString name, bool defaultValue = false, bool bReadData = true);
    ~BooleanPreference() { setKey(); }

	virtual bool readFromRegistry();
    bool operator =( bool value)    { return m_value = value; }
    operator bool()                 { return m_value; }

	void setKey();

private:
    CString m_name;
    bool    m_default;
    bool    m_value;
    };


    // Integer preference value
class IntPreference : public RegistryPreference
    {
public:
    IntPreference(CRegKey& parent, const CString name, int defaultValue = 0, bool bReadData = true); 
    ~IntPreference();

	virtual bool readFromRegistry();
    int operator =( int value)      { return m_value = value; }
    operator int()                  { return m_value; }

private:
    CString m_name;
    int     m_default;
    int     m_value;
    };


// String preference value
// Author: Dmitry Gavrilov
class StringPreference : public RegistryPreference
{
public:
	StringPreference(CRegKey& parent, const CString& name, CString defaultValue = CString(), bool bReadData = true);
	~StringPreference();

	virtual bool readFromRegistry();
	const CString& operator = (const CString& value) { return m_value = value; }
	operator CString()								 { return m_value; }

protected:
	CString	m_name;
	CString	m_default;
	CString	m_value;
};

// Directory string preference value, empty values converted to the current directory path
// Author: Dmitry Gavrilov
class DirStringPreference : public StringPreference
{
public:
	DirStringPreference(CRegKey& parent, const CString& name, CString defaultValue = CString(), bool bReadData = true) 
		: StringPreference(parent, name, defaultValue, bReadData) { setToCurDir(); }

	const CString& operator = (const CString& value)
	{
		m_value = value;
		setToCurDir();
		return m_value;
	}
	
	void setToCurDir();
};
	
/*DG - 2.12.2002*/// Comment out the following defining if you don't want to use icad's spooling features
// (e.g. print to PRN file directly w/o interaction with user, store output printing files to a particular folder).
// NOTE: Keep in sync USE_ICAD_SPOOLING definings in dlgspoolingsettings.cpp, icadview.h and preferences.h files!
#define USE_ICAD_SPOOLING


class UserPreference
{
public:
    static BooleanPreference    crosshairPreference;
    static IntPreference        crosshairSizePreference;
	
    static BooleanPreference    GripsAttractionPreference;
    static IntPreference        GripsAttractionDistancePreference;
	
    static BooleanPreference    ContinuousMotionPreference;
	
    // DP: Now we save and load this variable together with others.
    //static IntPreference      OSModePreference;
	
    static BooleanPreference    SnapFlyOverEnablePreference;
    static BooleanPreference    SnapMarkerPreference;
    static IntPreference        SnapMarkerSizePreference;
    static IntPreference        SnapMarkerThicknessPreference;
    static IntPreference        SnapMarkerColorPreference;
    static BooleanPreference    SnapMarkerDrawInAllViews;
    static BooleanPreference    SnapTooltipsPreference;
    static BooleanPreference    SnapApertureBoxPreference;
    static BooleanPreference    SnapShowOsnapCursorDecorationPreference;
	
    static BooleanPreference    s_bEnableHyperlinkMenu;
    static BooleanPreference    s_bEnableHyperlinkTooltip;

	static BooleanPreference	s_bSaveToLayout;
	static BooleanPreference	s_bUsePrintSettingsFromRegistry;

	static StringPreference		s_MtextPosition;
	
#ifdef USE_ICAD_SPOOLING
	static BooleanPreference	s_bForcePrintToFile;
	static BooleanPreference	s_bUseOutputFolder;
	static DirStringPreference	s_outputFolder;
#endif

	static void readFromRegistry();
};


#endif


