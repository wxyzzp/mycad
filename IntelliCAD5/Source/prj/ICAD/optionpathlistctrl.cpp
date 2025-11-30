/* F:\ICADDEV\PRJ\ICAD\OPTIONPATHLISTCTRL.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// OptionPathListCtrl.cpp: implementation of the OptionPathListCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "icad.h"
#include "OptionPathListCtrl.h"
#include "paths.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

OptionPathListCtrl::OptionPathListCtrl(EditType type) :	m_editing(false), m_editRow(0), m_editColumn(0)
{
	m_editType = type;
}

OptionPathListCtrl::~OptionPathListCtrl()
{

}

CEdit* OptionPathListCtrl::EditSubLabel(int row, int column)
{
	m_editColumn = column;
	m_editRow = row;

	return CListCtrlEx::EditSubLabel(row, column);
}

CEdit* OptionPathListCtrl::EditInvalidSubLabel()
{
	SetItemState(m_editRow, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	return EditSubLabel(m_editRow, m_editColumn);
}


BOOL OptionPathListCtrl::ValidateData()
{
	if (m_editType == FILE)
		return ValidateFileData();

	return ValidatePathData();
}

BOOL OptionPathListCtrl::ValidateFileData()
{
	CString fileData =  GetItemText(m_editRow, m_editColumn);

	//OK to have empty path
	if (fileData.IsEmpty())
	{
		m_editing = false;
		return TRUE;
	}

	// determine if file already exists
	int result = _access(fileData.GetBuffer(512), 0);

	if (result == -1)
	{
		//determine if file is creatable
		try {
			CFile file(fileData, CFile::modeCreate);
			file.Close();
			file.Remove(fileData);
		}
		catch (...)
		{
			CString error = fileData + ResourceString (IDC_OPTION_ERROR_FILE, " is an invalid filename."); 
			int result = MessageBox(error, ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ), MB_OK | MB_ICONEXCLAMATION);

			EditSubLabel(m_editRow, m_editColumn);
			return FALSE;
		}
	}

	m_editing = false;

	return TRUE;
}

BOOL OptionPathListCtrl::ValidatePathData()
{
	CString fileData =  GetItemText(m_editRow, m_editColumn);

	//OK to have empty path
	if (fileData.IsEmpty())
	{
		m_editing = false;
		return TRUE;
	}

	int begPos = 0;
	int separatorPos = 0;

	CString singlePath;
	//Modified Cybage MM 22/10/2001 DD/MM/YYYY [
	//Reason : Fix for Bug No.77806 from Bugzilla
	CString strFinal="";
	//Modified Cybage MM 22/10/2001 DD/MM/YYYY ]
	bool last = false;

	while (!last)
	{
		if ((separatorPos = fileData.Find(ResourceString(IDC_OPTION_FILESEPARATOR, ";"), begPos))!= -1)
		{
			singlePath = fileData.Mid(begPos, separatorPos - begPos);
			begPos = separatorPos + 1;
		}
		else
		{
			singlePath = fileData.Mid(begPos);
			last = true;

			//OK to have trailing ;
			if (singlePath.IsEmpty())
			{
				m_editing = false;
				return TRUE;
			}
		}

		int result = _access(singlePath, 0);
		if (result == -1)
			result = _access( FileString::ModulePath() + singlePath, 0);
	
		if (result == -1)
		{
			CString	error = singlePath + ResourceString (IDC_OPTION_ERROR_PATH, " does not exist.  Would you like to use this path anyway?");

			int result = MessageBox(error, ResourceString(AFX_IDS_APP_TITLE, "IntelliCAD" ), MB_YESNO | MB_ICONEXCLAMATION);
			//Modified Cybage MM 22/10/2001 DD/MM/YYYY [
			//Reason : Fix for Bug No.77806 from Bugzilla
			/*if (result != IDYES)
			{
				EditSubLabel(m_editRow, m_editColumn);

				return FALSE;
			}
		}
	} 

	m_editing = false;

	return TRUE;*/
			if (result == IDYES)
			{
				strFinal+=singlePath;  
				if(!last) strFinal+=";" ;
			}
		}
		else
		{
			strFinal+=singlePath;  
			if(!last) strFinal+=";" ;
		}
	}
	strFinal.TrimRight(";");
	if(fileData == strFinal)
	{
		m_editing = false;
		return TRUE;
	}
	if(strFinal.IsEmpty())// If none of the paths in the string are valid then we 
	// set a default path depending on the user path 
	{
		TCHAR buffer[MAX_PATH];		// Buffer for storing the default path
		DWORD bufferSize = MAX_PATH;
		if(m_editRow == 0 || m_editRow == 3 || m_editRow == 6) // If the user paths is one amongst
		//Drawing or Xrefs or Blocks set the default path to the personal of the current user
		{
			CRegKey myDocsKey;
			myDocsKey.Open(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders");
			LONG result = myDocsKey.QueryValue(buffer, "Personal", &bufferSize);
			if(result != ERROR_SUCCESS)
				assert(false);
		}
		else // Else Set the default path to module path
		{
			strcpy(buffer,FileString::ModulePath()); 
			int len = strlen(buffer);
			if( len > 0 )
				buffer[len-1]='\0'; // Removing the trailing / (slash) from the module path
		}
		strFinal = (CString)buffer;
	}
	SetItemText(m_editRow, m_editColumn,strFinal); // Setting the location to a valid or a 
	//default path	
	EditSubLabel(m_editRow, m_editColumn);
	return FALSE;
}
//Modified Cybage MM 22/10/2001 DD/MM/YYYY ]

BEGIN_MESSAGE_MAP(OptionPathListCtrl, CListCtrlEx)
	//{{AFX_MSG_MAP(OptionPathListCtrl)
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void OptionPathListCtrl::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
 	LV_ITEM		 *plvItem = &pDispInfo->item;	
	
	// overridden from CListCtrlEx as we want to allow null text
	SetItemText(plvItem->iItem, plvItem->iSubItem, plvItem->pszText);	

	*pResult = FALSE;
}


void OptionPathListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	int index;
	CListCtrlEx::OnLButtonDown(nFlags, point);


	if (!m_editing)
	{
		int colnum;
		if((index = HitTestEx(point, &colnum)) != -1 )
		{
			SetItemState(index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED); 

			if(colnum == 1)
			{
				EditSubLabel(index, colnum);
				m_editing = true;
			}
		}
	}
}



