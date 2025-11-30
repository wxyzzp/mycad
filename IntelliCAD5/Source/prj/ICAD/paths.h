/* F:\ICADDEV\PRJ\ICAD\PATHS.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * General support for filepath operations
 *
 */

#ifndef _PATHS_H
#define _PATHS_H

//Modified Cybage MM 22/10/2001 DD/MM/YYYY [
//Reason : Fix for Bug No.77806 from Bugzilla
CString ValidateLocation(CString strTyped);
//Modified Cybage MM 22/10/2001 DD/MM/YYYY ]

	// utility class for parsing file strings
class FileString : public CString
	{
public:
				FileString() {}
				FileString( const CString& string) : CString( string) {}

	CString		GetDrive() const;
	CString		GetDirectory() const;
	CString		GetFilename() const;
	CString		GetExtension() const;

static FileString	ModuleFile();
static FileString	ModulePath();
	};


	// class to process files found on path
class AbstractFileProcessor
	{
public:
	virtual void ProcessFile( LPCTSTR, WIN32_FIND_DATA&) = 0;
	};


class FilePath : public CString
	{
public:
	FilePath() {};
	FilePath( LPCTSTR string) : CString( string) {}

	int  FindFiles( AbstractFileProcessor&, LPCTSTR pattern);
	CString GetFirstPath(void);
	};


class SystemPaths
{
public:
	SystemPaths();

	bool LoadFromRegistry();
	bool SaveToRegistry();

	//*** System Paths from options dialog
	CString m_drawingPath;
	CString m_fontPath;
	CString m_helpPath;
	CString m_xrefPath;
	CString m_menuPath;
	CString m_hatchPatPath;
	CString m_blocksPath;
	CString m_saveFilePath; // EBATECH(CNBR) 2002/6/20 for $SAVEFILEPATH
	CString m_templatesPath; // EBATECH(CNBR) 2002/6/20 for $SAVEFILEPATH

protected:
	LONG LoadKey(HKEY hKey, CString valueName, CString &value);
	void SaveKey(HKEY hKey, CString valueName, CString value);
};


bool	SearchDrawingFile( LPCTSTR, TCHAR *);
bool	SearchFontFile( LPCTSTR, TCHAR *);
bool	SearchHatchFile( LPCTSTR, TCHAR *);
bool	SearchHelpFile( LPCTSTR which, TCHAR *);
bool	SearchMenuFile( LPCTSTR, TCHAR *);
bool	SearchXrefFile( LPCTSTR, TCHAR *);
void	CommandRefHelp( HWND window, DWORD context);
bool	SearchHelpFile( LPCTSTR which, TCHAR *result);


#endif

