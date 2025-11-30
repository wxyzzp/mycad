/* F:\ICADDEV\PRJ\ICAD\PATHS.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

#include "icad.h"
#include "IcadApi.h"
#include "paths.h"
#include "Preferences.h"
#include "IcadHelp.h"
#include "win32misc.h"

// This Win 2000 API is not yet available in H files...
// EBATECH(CNBR) Nobody refer it.
//	Win 2000 API - SHGetSpecialFolderPath()
///*WINSHELLAPI*/ HRESULT WINAPI SHGetFolderPath(
//						HWND hwndOwner,
//						int nFolder,
//						HANDLE hToken,
//						DWORD dwFlags,
//						LPTSTR pszPath
//						)
//	{
//	_tcscpy( pszPath, ResourceString( IDS_DRAWINGPATH, "\\My Documents"));
//	return S_OK;
//	}
// EBATECH(CNBR)

class SearchResult : public AbstractFileProcessor
	{
public:
	SearchResult( CString& path) : m_path(path){}
	void ProcessFile( LPCTSTR, WIN32_FIND_DATA&);

protected:
	CString&	m_path;
	};


void
SearchResult::ProcessFile( LPCTSTR path, WIN32_FIND_DATA& data)
	{
	m_path = CString( path) + data.cFileName;
	}


static bool
SearchSystemFile( LPCTSTR name, TCHAR *fullPath, LPCTSTR systemPath)
	{
	CString 		resultPath;
	SearchResult	processor( resultPath);
	FilePath		path( systemPath);

	int count = path.FindFiles( processor, name);
	ASSERT( count < 2 );

	if ( count EQ 0 )
		return false;

	_tcscpy( fullPath, resultPath);
	return true;
	}


bool
SearchDrawingFile( LPCTSTR name, TCHAR *fullPath)
	{
	CMainWindow *mainWnd = (CMainWindow*) SDS_CMainWindow;// GetParentOwner();
	SystemPaths *path = mainWnd->m_paths;

	return SearchSystemFile( name, fullPath, path->m_drawingPath);
	}


bool
SearchFontFile( LPCTSTR name, TCHAR *fullPath)
	{
	CMainWindow *mainWnd = (CMainWindow*) SDS_CMainWindow;// GetParentOwner();
	SystemPaths *path = mainWnd->m_paths;

	return SearchSystemFile( name, fullPath, path->m_fontPath);
	}


bool
SearchHatchFile( LPCTSTR name, TCHAR *fullPath)
	{
	CMainWindow *mainWnd = (CMainWindow*) SDS_CMainWindow;// GetParentOwner();
	SystemPaths *path = mainWnd->m_paths;

	return SearchSystemFile( name, fullPath, path->m_hatchPatPath);
	}


bool
SearchHelpFile( LPCTSTR name, TCHAR *fullPath)
	{
	CMainWindow *mainWnd = (CMainWindow*) SDS_CMainWindow;// GetParentOwner();
	SystemPaths *path = mainWnd->m_paths;

	return SearchSystemFile( name, fullPath, path->m_helpPath);
	}


bool
SearchMenuFile( LPCTSTR name, TCHAR *fullPath)
	{
	CMainWindow *mainWnd = (CMainWindow*) SDS_CMainWindow;// GetParentOwner();
	SystemPaths *path = mainWnd->m_paths;

	return SearchSystemFile( name, fullPath, path->m_menuPath);
	}


bool
SearchXrefFile( LPCTSTR name, TCHAR *fullPath)
	{
	CMainWindow *mainWnd = (CMainWindow*) SDS_CMainWindow;// GetParentOwner();
	SystemPaths *path = mainWnd->m_paths;

	return SearchSystemFile( name, fullPath, path->m_xrefPath);
	}


void
CommandRefHelp( HWND window, DWORD context)
	{
	char fs1[IC_ACADBUF];
	if( !SearchHelpFile( SDS_COMMANDHELP, fs1) )
		{
		sprintf(fs1,ResourceString(CMD_ERR_FINDHELP, "Unable to find the requested help file.\n%s\nCheck installation." ),SDS_COMMANDHELP);
		MessageBox( window, fs1,NULL,MB_OK|MB_ICONEXCLAMATION);
		}
	else
		CMainWindow::IcadHelp( fs1, ICADHELP_CONTEXT, context);
	}

// EBATECH(CNBR) set m_drawingPath and m_xrefPath "" now.
// EBATECH(CNBR) 2002/6/20 set m_saveFilePath also
SystemPaths:: SystemPaths()
		: m_drawingPath(""), m_fontPath("Fonts"), m_helpPath("Help"), m_xrefPath(""),
		m_menuPath(""), m_hatchPatPath("Patterns;Patterns\\ISO"),  m_saveFilePath(""), m_templatesPath("Templates")
	{
	}


LONG SystemPaths::LoadKey(HKEY hKey, CString valueName, CString &value)
{
	DWORD type, size = MAX_PATH;
	char	string[MAX_PATH] = "";
	long	result = RegQueryValueEx(hKey, valueName, 0, &type, (LPBYTE)&string, &size);

	if ( result == ERROR_SUCCESS)
		value = string;
	else
		value = "";

	return result;
}

//Modified Cybage MM 22/10/2001 DD/MM/YYYY [
//Reason : Fix for Bug No.77806 from Bugzilla
CString ValidateLocation(CString strTyped)
{
	int index = strTyped.Find(';');
	CString strSubString1 = strTyped;	// Contains the hatch pattern path in the beginning
	//and is trimmed for each subsequent path in the string
	CString strSubString2 ;				// Contains the path to be validated
	CString strValid ;					// Contains the valid path
	LONG nTotLength = strSubString1.GetLength();
	LONG nCoveredLength = 0;
	while(index > -1)					// Index becomes -1 if the tested path does not exist
	{
		strSubString2 = strSubString1.Left(index - nCoveredLength);	// To extract one path
		//from strSubString1
		nCoveredLength = index + 1 ;
		strSubString1 = strSubString1.Right(nTotLength - nCoveredLength); // Trimming the
		//extracted path from strSubString1
		if(_access(LPCTSTR(strSubString2),0) != -1 || _access( FileString::ModulePath() + strSubString2, 0) != -1)
		// Validating the extracted string. Validation succeedes even if the path is a folfder at the module path
		{
			strValid += strSubString2;
			strValid += ";" ;				  // Appending ; after each valid path
		}
		index = strTyped.Find(';', index + 2);// Finding the next ; in the hatch pattern path
	}
	if((!strSubString1.IsEmpty()) && ((_access(LPCTSTR(strSubString1),0) != -1) || (_access( FileString::ModulePath() + strSubString1, 0) != -1)))
	// To check the remaining path and add it to strValid if it is valid
		strValid += strSubString1;
	strValid.TrimRight(";");	// To delete the ending ; if any
	return strValid;
}
//Modified Cybage MM 22/10/2001 DD/MM/YYYY ]


/*-------------------------------------------------------------------------*//**
Add the given path to srchpath string.

@author	Dmitry Gavrilov
@param	srchpath	<= srchpath to add to
@param	path		=> path to add
@return	'false' if srchpath cannot be enlarged anymore because of SRCHPATH's length limit, otherwise 'true'
*//*--------------------------------------------------------------------------*/
static bool addPathToSrchpath(CString& srchpath, const CString& path)
{
	if(!path.IsEmpty())
	{
		if(!srchpath.IsEmpty() && srchpath.Right(1) != ResourceString(IDC_OPTION_FILESEPARATOR, ";"))
		{
			if(srchpath.GetLength() + strlen(ResourceString(IDC_OPTION_FILESEPARATOR, ";")) >= db_Getsysvar()[DB_QSRCHPATH].nc)
				return false;
			srchpath += ResourceString(IDC_OPTION_FILESEPARATOR, ";");
		}
		if(srchpath.GetLength() + path.GetLength() + 1 > db_Getsysvar()[DB_QSRCHPATH].nc)
		{
			CString	cutPath = path.Left(db_Getsysvar()[DB_QSRCHPATH].nc - srchpath.GetLength() - 1);
			int		pos = cutPath.ReverseFind(';');
			if(pos > 0)
				srchpath += cutPath.Left(pos);
			return false;
		}
		srchpath += path;
	}
	return true;
}

bool SystemPaths::LoadFromRegistry()
{
	PreferenceKey		configKey(HKEY_CURRENT_USER, "Config");
	HKEY hKey = configKey;
	TCHAR buffer[MAX_PATH];
	DWORD bufferSize = MAX_PATH;
	//VERIFY( SUCCEEDED( SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, myDocs) ));
	CRegKey myDocsKey;
	// EBATECH(CNBR) 2002/9/24 Use KEY_READ instead of KEY_ALL_ACCESS
	myDocsKey.Open(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders", KEY_READ);
	//myDocsKey.Open(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders");
	LONG result = myDocsKey.QueryValue(buffer, "Personal", &bufferSize);
	if(result != ERROR_SUCCESS)
		assert(false);

	if(LoadKey(hKey, "Drawing", m_drawingPath) != ERROR_SUCCESS)
		m_drawingPath = buffer;

	if(LoadKey(hKey, "Blocks", m_blocksPath) != ERROR_SUCCESS)
		m_blocksPath = buffer;

	if(LoadKey(hKey, "Xref", m_xrefPath) != ERROR_SUCCESS)
		m_xrefPath = buffer;

	CString currentVersion;
	CRegKey installDir;

// EBATECH-[Create by Applicaton name(AFX_IDS_APP_TITLE) and RegistryRoot(IDS_REGISTRY_ROOT)
	//Modified Cybage VSB 13/04/2001 DD/MM/YYYY [
	//Reason : Mail from Surya Sarda dated 06/04/2001
	//installDir.Open(HKEY_LOCAL_MACHINE, "Software\\ITC\\IntelliCAD");
	//installDir.Open(HKEY_LOCAL_MACHINE, ResourceString( IDS_REGISTRY_ROOT, "Software\\ITC\\IntelliCAD"));
	//Modified Cybage VSB 13/04/2001 DD/MM/YYYY ]
	CString root;
	// get registry key name (check /p option)
	getRegCurrentProfileRoot(root.GetBuffer(IC_ACADBUF), IC_ACADBUF); // Modify by SystemMetrix(Maeda) 09.10.2001 
	root.ReleaseBuffer();
	// EBATECH(CNBR) 2002/9/24 Use KEY_READ instead of KEY_ALL_ACCESS
	installDir.Open(HKEY_LOCAL_MACHINE, root, KEY_READ);
	//installDir.Open(HKEY_LOCAL_MACHINE, root);
	// ]-EBATECH

	bufferSize = MAX_PATH;
	result = installDir.QueryValue(buffer, "CurVer", &bufferSize);

	// EBATECH(watanabe)-[ Hide assertion
	//if(result != ERROR_SUCCESS)
	//	ASSERT(false);
	// ]-EBATECH(watanabe)

	// EBATECH-[use previous create value(root).
	//Modified Cybage VSB 13/04/2001 DD/MM/YYYY [
	//Reason : Mail from Surya Sarda dated 06/04/2001
	//currentVersion.Format("Software\\ITC\\IntelliCAD\\%s", buffer);
	//CString strTemp = ResourceString( IDS_REGISTRY_ROOT, "Software\\ITC\\IntelliCAD");
	//currentVersion.Format("%s\\%s",strTemp,buffer);
	//Modified Cybage VSB 13/04/2001 DD/MM/YYYY ]
	currentVersion.Format("%s\\%s",root,buffer);
	// ]-EBATECH
	installDir.Close();
	// EBATECH(CNBR) 2002/9/24 Use KEY_READ instead of KEY_ALL_ACCESS
	installDir.Open(HKEY_LOCAL_MACHINE, currentVersion, KEY_READ);
	//installDir.Open(HKEY_LOCAL_MACHINE, currentVersion);
	bufferSize = MAX_PATH;
	result = installDir.QueryValue(buffer, "InstallDir", &bufferSize);
	// EBATECH(CNBR) -[ Set current directory if InstallDir doesn't find.
	if(result != ERROR_SUCCESS){
		strcpy(buffer,FileString::ModulePath());
		int len = strlen(buffer);
		if( len > 0 ){
			buffer[len-1]='\0';
		}
	}
	// ]-EBATECH(CNBR)

	if(LoadKey(hKey, "Font", m_fontPath) != ERROR_SUCCESS)
	{
		m_fontPath = buffer;
		m_fontPath = m_fontPath + "\\" + ResourceString( IDS_FONTPATH, "Fonts");
		//Modified Cybage MM 22/10/2001 DD/MM/YYYY [
		//Reason : Fix for Bug No.77806 from Bugzilla
		if(ValidateLocation(m_fontPath).IsEmpty()) m_fontPath = buffer; // Checking for the
		//validity of the font path
		//Modified Cybage MM 22/10/2001 DD/MM/YYYY ]
	}

	if(LoadKey(hKey, "Help", m_helpPath) != ERROR_SUCCESS)
	{
		m_helpPath = buffer;
		m_helpPath = m_helpPath + "\\" + ResourceString( IDS_HELPPATH, "Help");
		//Modified Cybage MM 22/10/2001 DD/MM/YYYY [
		//Reason : Fix for Bug No.77806 from Bugzilla
		if(ValidateLocation(m_helpPath).IsEmpty()) m_helpPath = buffer; // Checking for the
		//validity of the help path
		//Modified Cybage MM 22/10/2001 DD/MM/YYYY ]
	}

	if(LoadKey(hKey, "HatchPat", m_hatchPatPath) != ERROR_SUCCESS)
	{
		m_hatchPatPath = buffer;
		m_hatchPatPath = m_hatchPatPath + "\\" + ResourceString( IDS_PATTERNPATH, "Patterns;Patterns\\ISO");
		int index = m_hatchPatPath.Find(';');
		while(index > -1)
		{
			m_hatchPatPath.Insert(index + 1, '\\');
			m_hatchPatPath.Insert(index + 1, buffer);
			index = m_hatchPatPath.Find(';', index + 2);
		}
		//Modified Cybage MM 22/10/2001 DD/MM/YYYY [
		//Reason : Fix for Bug No.77806 from Bugzilla
		if(ValidateLocation(m_hatchPatPath).IsEmpty()) m_hatchPatPath = buffer; // Checking for
		//the validity of the hatch pattern path
		//Modified Cybage MM 22/10/2001 DD/MM/YYYY ]
	}

	if(LoadKey(hKey, "Menu", m_menuPath) != ERROR_SUCCESS)
	{
		m_menuPath = buffer;
	}

	if(LoadKey(hKey, "Templates", m_templatesPath) != ERROR_SUCCESS)
	{
		m_templatesPath = buffer;
		m_templatesPath = m_templatesPath + "\\" + ResourceString( IDS_TEMPLATEPATH, "Templates");
		if(ValidateLocation(m_templatesPath).IsEmpty()) m_templatesPath = buffer; // Checking for the validity of the path
	}

	// EBATECH(CNBR) -[ 2002/6/20 SAVEFILEPATH
	if(LoadKey(hKey, "SAVEFILEPATH", m_saveFilePath) != ERROR_SUCCESS)
	{
		resbuf rb;
		int len;
		GetTempPath( MAX_PATH, buffer );
		len = _tcslen( buffer );
		if( len > 2 && _tcscmp( &buffer[len-2], ":\\" ) != 0 )
		{
			buffer[len-1] = '\0'; // Elliminate last '\\'
		}
		m_saveFilePath = CString(buffer);
		rb.restype = RTSTR;
		rb.resval.rstring = buffer;
		sds_setvar("SAVEFILEPATH",&rb);
	}
	// EBATECH(CNBR) ]-


	/*DG - 2.7.2003*/// update SRCHPATH if necessary combining it from other path strings
	resbuf	rb;
	SDS_getvar(NULL, DB_QSRCHPATH, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	if(!rb.resval.rstring || !*rb.resval.rstring)
	{
		CString	srchpath;

		/*DG - 4.7.2003*/// avoid exceeding SRCHPATH length
		if( addPathToSrchpath(srchpath, m_drawingPath) &&
			addPathToSrchpath(srchpath, m_fontPath) &&
			addPathToSrchpath(srchpath, m_helpPath) &&
			addPathToSrchpath(srchpath, m_xrefPath) &&
			addPathToSrchpath(srchpath, m_menuPath) &&
			addPathToSrchpath(srchpath, m_hatchPatPath) )
			addPathToSrchpath(srchpath, m_templatesPath);
		// the Options dialog doesn't include the following paths to SRCHPATH, so we don't do it here too;
		// otherwise the following calls should be added to the previous statement
		//addPathToSrchpath(srchpath, m_blocksPath);
		//addPathToSrchpath(srchpath, m_saveFilePath);

		IC_FREE(rb.resval.rstring);
		rb.resval.rstring = new char[srchpath.GetLength() + 1];
		srchpath.Replace(',', ';');
		strcpy(rb.resval.rstring, (LPCTSTR)srchpath);
		SDS_setvar(NULL, DB_QSRCHPATH, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 0);
	}
	IC_FREE(rb.resval.rstring);

	return true;
}

void SystemPaths::SaveKey(HKEY hKey, CString valueName, CString value)
{
	// This was required because empty values were not being saved if a non-empty value preceded it.
	if (value.IsEmpty())
		RegDeleteValue(hKey, valueName);
	else
		RegSetValueEx(hKey, valueName, 0, REG_SZ, (CONST BYTE*) value.GetBuffer(0), value.GetLength());
}

bool SystemPaths::SaveToRegistry()
{
	PreferenceKey		configKey(HKEY_CURRENT_USER, "Config");
	HKEY hKey = configKey;

	if (hKey == NULL) {
		return false;
	}

	SaveKey(hKey, "Drawing", m_drawingPath);
	SaveKey(hKey, "Font", m_fontPath);
	SaveKey(hKey, "Help", m_helpPath);
	SaveKey(hKey, "Xref", m_xrefPath);
	SaveKey(hKey, "Menu", m_menuPath);
	SaveKey(hKey, "HatchPat", m_hatchPatPath);
	SaveKey(hKey, "Blocks", m_blocksPath);
	SaveKey(hKey, "SAVEFILEPATH", m_saveFilePath); // EBATECH(CNBR) 2002/6/20 $SAVEFILEPATH
	SaveKey(hKey, "Templates", m_templatesPath);

//	RegCloseKey(hKey);
	return true;
}


	// utility class for parsing file strings
CString
FileString:: GetDrive() const
	{
	TCHAR	buffer[ MAX_PATH];

	_tsplitpath( *this, buffer, NULL, NULL, NULL);
	return buffer;
	}


CString
FileString:: GetDirectory() const
	{
	TCHAR	buffer[ MAX_PATH];

	_tsplitpath( *this, NULL, buffer, NULL, NULL);
	return buffer;
	}


CString
FileString:: GetFilename() const
	{
	TCHAR	buffer[ MAX_PATH];

	_tsplitpath( *this, NULL, NULL, buffer, NULL);
	return buffer;
	}


CString
FileString:: GetExtension() const
	{
	TCHAR	buffer[ MAX_PATH];

	_tsplitpath( *this, NULL, NULL, NULL, buffer);
	return buffer;
	}


FileString
FileString:: ModuleFile()
	{
	TCHAR	module[ MAX_PATH];
	VERIFY( ::GetModuleFileName( _Module.GetModuleInstance(), module, sizeof( module)) );
	return module;
	}


FileString
FileString:: ModulePath()
	{
	FileString	module = ModuleFile();
	return FileString( module.GetDrive() + module.GetDirectory());
	}


	// finds all occurrences of pattern in the path
	// returns cound of files found
int FilePath::FindFiles( AbstractFileProcessor& processor, LPCTSTR pattern)
	{
	CString 	paths = *this;
	int 		count = 0;

		// iterate through segments of path (separated by ;)
	while ( paths.GetLength() )
		{
		int separator = paths.Find( ';');
		CString segment;

		if ( separator >= 0 )
			{
			segment = paths.Left( separator);
			paths = paths.Right( paths.GetLength() - separator - 1);
			}
		else
			{
			segment = paths;
			paths = paths.Right(0);
			}

		// look for occurrences of pattern
		WIN32_FIND_DATA findData;
		HANDLE fileHandle;

		fileHandle = FindFirstFile( segment + "\\" + pattern, &findData);

		// if nothing found, check relative to root
		if ( fileHandle EQ INVALID_HANDLE_VALUE )
			{
			segment = FileString::ModulePath() + segment;
			fileHandle = FindFirstFile( segment + "\\" + pattern, &findData);
			}

		if ( fileHandle != INVALID_HANDLE_VALUE )
			do
				{
				processor.ProcessFile( segment + "\\", findData);
				count += 1;
				} while ( FindNextFile(fileHandle, &findData) );

		FindClose(fileHandle);
		}

	return count;
	}

	// returns the first path in a list of paths separated by ';'
CString FilePath::GetFirstPath(void)
	{
	CString 	paths = *this;
	CString		strFirstPath = paths;
	int separator = paths.Find( ';');

	if ( separator >= 0 )
		strFirstPath = paths.Left( separator);

	return strFirstPath;
	}

