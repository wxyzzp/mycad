//	IcadMenuMain.h : header file
//	Copyright (C) 1994,1995,1996,1997 by Visio, Inc.
//

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CIcadMenuMain

class CIcadApp;
class CIcadMenu;

class CIcadMenuMain
{

//*** Member variables
public:

protected:
	CIcadApp* m_pApp;
	CMainWindow* m_pMain;
	CPtrList* m_pMenuList;
	CIcadMenu* m_pDefaultMenu;
	CIcadMenu* m_pCurrentMenu;

//*** Member functions
public:
	CIcadMenuMain(CIcadApp* pApp, CMainWindow* pMain);
	~CIcadMenuMain();
	BOOL LoadMenu(LPCTSTR pszMenuFile);
	CIcadMenu* IsLoaded(LPCTSTR pszMenuFile);
	BOOL UpdateMRU(BOOL bSetCurrent);
	void RemoveFromList(CIcadMenu* pIcadMenu);
	CIcadMenu* GetDefaultMenu()
	{
		return m_pDefaultMenu;
	}
	CIcadMenu* GetCurrentMenu()
	{
		return m_pCurrentMenu;
	}
	void SetDefaultMenu(CIcadMenu* pDefaultMenu)
	{
		m_pDefaultMenu=pDefaultMenu;
	}
	void SetCurrentMenu(CIcadMenu* pCurrentMenu)
	{
		m_pCurrentMenu=pCurrentMenu;
	}

protected:

};
