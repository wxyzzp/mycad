/* D:\DEV\PRJ\ICAD\ICADMENUMAIN.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

//	IcadMenuMain.cpp
//	Copyright (C) 1994,1995,1996,1997 by Visio, Inc.
//

//** Includes
#include "Icad.h"/*DNT*/
#include "IcadMenuMain.h"/*DNT*/
#include "IcadMenu.h"/*DNT*/

/////////////////////////////////////////////////////////////////////////////
// CIcadMenuMain

CIcadMenuMain::CIcadMenuMain(CIcadApp* pApp, CMainWindow* pMain)
{
	m_pApp=pApp;
	m_pMain=pMain;
	m_pMenuList=NULL;
	m_pDefaultMenu=NULL;
	m_pCurrentMenu=NULL;
}

CIcadMenuMain::~CIcadMenuMain()
{
	//*** Delete the menu list.
	if(m_pMenuList!=NULL)
	{
		POSITION pos = m_pMenuList->GetHeadPosition();
		while(pos!=NULL)
		{
			CIcadMenu* pMenu = (CIcadMenu*)m_pMenuList->GetNext(pos);
			delete pMenu;
		}
		delete m_pMenuList;
		m_pMenuList=NULL;
	}
	m_pDefaultMenu=NULL;
	m_pCurrentMenu=NULL;
}

void CIcadMenuMain::RemoveFromList(CIcadMenu* pIcadMenu)
{
	//*** Delete the menu from the list.
	if(NULL==pIcadMenu || pIcadMenu==GetDefaultMenu())
		return;

	if(pIcadMenu==GetCurrentMenu())
	{
		SetCurrentMenu(GetDefaultMenu());
	}
	if(NULL!=m_pMenuList)
	{
		POSITION pos = m_pMenuList->Find(pIcadMenu);
		if(NULL!=pos)
		{
			delete pIcadMenu;
			m_pMenuList->RemoveAt(pos);
		}
		if(m_pMenuList->IsEmpty())
		{
			delete m_pMenuList;
			m_pMenuList=NULL;
		}
	}
}

BOOL CIcadMenuMain::LoadMenu(LPCTSTR pszMenuFile)
{
	//*** Create the menu.
	CIcadMenu* pNewMenu = new CIcadMenu(m_pApp,m_pMain);
	if(NULL==pNewMenu)
	{
		return FALSE;
	}
	//*** Load the menu.
	if(!pNewMenu->LoadMenu(pszMenuFile,FALSE))
	{
		delete pNewMenu;
		return FALSE;
	}
	//*** Add the menu to the list of menus.
	if(NULL==m_pMenuList)
	{
		m_pMenuList = new CPtrList();
	}
	m_pMenuList->AddTail(pNewMenu);
	if(NULL==pszMenuFile)
	{
		//*** Set the pointer to the default menu.
		m_pDefaultMenu=pNewMenu;
	}
	//*** Set the pointer to the current menu.
	m_pCurrentMenu=pNewMenu;

	return TRUE;
}

CIcadMenu* CIcadMenuMain::IsLoaded(LPCTSTR pszMenuFile)
{
//*** Enumerate through the list of loaded menus and return the pointer to
//*** the menu "pszMenuFile".  If it has not already been loaded, return NULL.
	if(NULL==pszMenuFile || NULL==m_pMenuList)
	{
		return NULL;
	}
	CIcadMenu* pMenu;
	POSITION pos = m_pMenuList->GetHeadPosition();
	while(pos!=NULL)
	{
		pMenu=(CIcadMenu*)m_pMenuList->GetNext(pos);
		if(NULL!=pMenu)
		{
			if(pMenu->m_strMenuFile.CompareNoCase(pszMenuFile)==0)
			{
				return pMenu;
			}
		}
	}

	return NULL;
}

BOOL CIcadMenuMain::UpdateMRU(BOOL bSetCurrent)
{
	if(m_pMenuList!=NULL)
	{
		POSITION pos = m_pMenuList->GetHeadPosition();
		while(pos!=NULL)
		{
			CIcadMenu* pMenu = (CIcadMenu*)m_pMenuList->GetNext(pos);
			pMenu->UpdateVisibility();
		}
		if(bSetCurrent && NULL!=m_pCurrentMenu)
		{
			m_pCurrentMenu->SetMenu();
		}
	}
	return TRUE;
}


