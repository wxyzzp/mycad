/* D:\DEV\PRJ\ICAD\ICADOLELINKSDIALOG.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

// IcadOleLinksDialog.cpp : implementation file
//

#include "Icad.h"/*DNT*/
#include "IcadOleLinksDialog.h"/*DNT*/
#include "IcadCntrItem.h"/*DNT*/

#ifdef _DEBUG

	#if !defined(USE_SMARTHEAP)
	#define new DEBUG_NEW
	#endif

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Common code for all OLE UI dialogs

UINT CALLBACK
IcadOleHookProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_INITDIALOG)
		return (UINT)AfxDlgProc(hWnd, message, wParam, lParam);

	if (message == WM_COMMAND && LOWORD(wParam) == IDC_OLEUIHELP)
	{
		// just translate the message into the AFX standard help command.
		SendMessage(hWnd, WM_COMMAND, ID_HELP, 0);
		return TRUE;
	}
	return 0;
}

SCODE IcadParseDisplayName(LPMONIKER lpmk, LPBC lpbc, LPTSTR lpszRemainder,
	ULONG* cchEaten, LPMONIKER* plpmkOut)
{
	USES_CONVERSION;

	ASSERT(lpmk != NULL);
	ASSERT(AfxIsValidString(lpszRemainder));
	ASSERT(cchEaten != NULL);
	ASSERT(plpmkOut != NULL);

	SCODE sc;
	if (lpbc != NULL)
	{
		// ask moniker to parse the display name itself
		sc = lpmk->ParseDisplayName(lpbc, NULL, T2OLE(lpszRemainder), cchEaten,
			plpmkOut);
	}
	else
	{
		// skip leading delimiters
		int cEaten = 0;
		LPTSTR lpszSrc = lpszRemainder;
		while (*lpszSrc != '\0' && (*lpszSrc == '\\' || *lpszSrc == '/' ||
			*lpszSrc == ':' || *lpszSrc == '!' || *lpszSrc == '['))
		{
			if (_istlead(*lpszSrc))
				++lpszSrc, ++cEaten;
			++lpszSrc;
			++cEaten;
		}

		// parse next token in lpszRemainder
		TCHAR szItemName[_MAX_PATH];
		LPTSTR lpszDest = szItemName;
		while (*lpszSrc != '\0' && *lpszSrc != '\\' && *lpszSrc != '/' &&
			*lpszSrc != ':' && *lpszSrc != '!' && *lpszSrc != '[' &&
			cEaten < _MAX_PATH-1)
		{
			if (_istlead(*lpszSrc))
				*lpszDest++ = *lpszSrc++, ++cEaten;
			*lpszDest++ = *lpszSrc++;
			++cEaten;
		}
		*cchEaten = cEaten;
		sc = CreateItemMoniker(OLESTDDELIMOLE, T2COLE(szItemName), plpmkOut);
	}

	return sc;
}

LPMONIKER AFXAPI IcadOleGetFirstMoniker(LPMONIKER lpmk)
{
	if (lpmk == NULL)
		return NULL;

	DWORD dwMksys;
	if (lpmk->IsSystemMoniker(&dwMksys) == S_OK
		&& dwMksys == MKSYS_GENERICCOMPOSITE)
	{
		LPENUMMONIKER lpenumMoniker = NULL;
		SCODE sc = lpmk->Enum(TRUE, &lpenumMoniker);
		if (sc != S_OK)
			return NULL;

		ASSERT(lpenumMoniker != NULL);
		LPMONIKER lpmkFirst = NULL;
		sc = lpenumMoniker->Next(1, &lpmkFirst, NULL);
		lpenumMoniker->Release();
		return lpmkFirst;
	}

	// otherwise -- return the moniker itself
	lpmk->AddRef();
	return lpmk;
}

DWORD AFXAPI IcadOleGetLenFilePrefixOfMoniker(LPMONIKER lpmk)
{
	USES_CONVERSION;

	if (lpmk == NULL)
		return 0;

	DWORD nLen = 0;
	LPMONIKER lpmkFirst = IcadOleGetFirstMoniker(lpmk);
	if (lpmkFirst != NULL)
	{
		DWORD  dwMksys;
		if (lpmkFirst->IsSystemMoniker(&dwMksys) == S_OK &&
			dwMksys == MKSYS_FILEMONIKER)
		{
			LPBC lpbc = NULL;
			SCODE sc = CreateBindCtx(0, &lpbc);
			if (sc == S_OK)
			{
				LPOLESTR lpw = NULL;
				sc = lpmkFirst->GetDisplayName(lpbc, NULL, &lpw);
				LPTSTR lpsz = OLE2T(lpw);
				if (sc == S_OK && lpsz != NULL)
				{
					nLen = lstrlen(lpsz);
					CoTaskMemFree(lpw);
				}
				lpbc->Release();
			}
		}
		lpmkFirst->Release();
	}
	return nLen;
}

/////////////////////////////////////////////////////////////////////////////
// CIcadOleLinksDialog

IMPLEMENT_DYNAMIC(CIcadOleLinksDialog, COleDialog)

CIcadOleLinksDialog::CIcadOleLinksDialog(COleDocument* pDoc, CView* pView,
	DWORD dwFlags, CWnd* pParentWnd) : COleDialog(pParentWnd),
	m_xLinkInfo(pDoc)
{
	ASSERT_VALID(pDoc);
	if (pView != NULL)
		ASSERT_VALID(pView);

	memset(&m_el, 0, sizeof(m_el)); // initialize structure to 0/NULL

	// fill in common part
	m_el.cbStruct = sizeof(m_el);
	m_el.dwFlags = dwFlags;
//	if (!afxData.bWin4 && AfxHelpEnabled())
//		m_el.dwFlags |= ELF_SHOWHELP;
	m_el.lpfnHook = IcadOleHookProc;
	m_nIDHelp = AFX_IDD_EDITLINKS;

	// specific to this dialog
	if (pView != NULL)
		m_xLinkInfo.m_pSelectedItem = pDoc->GetPrimarySelectedItem(pView);
	else
		m_xLinkInfo.m_pSelectedItem = NULL;
	m_el.lpOleUILinkContainer = &m_xLinkInfo;
}

CIcadOleLinksDialog::~CIcadOleLinksDialog()
{
}

int CIcadOleLinksDialog::DoModal()
{
	ASSERT_VALID(this);
	ASSERT(m_el.lpfnHook != NULL);  // can still be a user hook

	// this function is always used for updating links
	m_xLinkInfo.m_bUpdateLinks = TRUE;

	m_el.hWndOwner = PreModal();
	int iResult = MapResult(::OleUIEditLinks(&m_el));
	PostModal();
	return iResult;
}

/////////////////////////////////////////////////////////////////////////////
// CIcadOleLinksDialog diagnostics

#ifdef _DEBUG
void CIcadOleLinksDialog::Dump(CDumpContext& dc) const
{
	COleDialog::Dump(dc);

	dc << (LPCTSTR)ResourceString(IDC_ICADOLELINKSDIALOG__NM_E_0, "\nm_el.cbStruct = " ) << m_el.cbStruct;
	dc << (LPCTSTR)ResourceString(IDC_ICADOLELINKSDIALOG__NM_E_1, "\nm_el.dwFlags = " ) << (void*)m_el.dwFlags;
	dc << (LPCTSTR)ResourceString(IDC_ICADOLELINKSDIALOG__NM_E_2, "\nm_el.hWndOwner = " ) << (UINT)m_el.hWndOwner;
	dc << (LPCTSTR)ResourceString(IDC_ICADOLELINKSDIALOG__NM_E_3, "\nm_el.lpszCaption = " ) << m_el.lpszCaption;
	dc << (LPCTSTR)ResourceString(IDC_ICADOLELINKSDIALOG__NM_E_4, "\nm_el.lCustData = " ) << (void*)m_el.lCustData;
	dc << (LPCTSTR)ResourceString(IDC_ICADOLELINKSDIALOG__NM_E_5, "\nm_el.hInstance = " ) << (UINT)m_el.hInstance;
	dc << (LPCTSTR)ResourceString(IDC_ICADOLELINKSDIALOG__NM_E_6, "\nm_el.lpszTemplate = " ) << (void*)m_el.lpszTemplate;
	dc << (LPCTSTR)ResourceString(IDC_ICADOLELINKSDIALOG__NM_E_7, "\nm_el.hResource = " ) << (UINT)m_el.hResource;
	if (m_el.lpfnHook == IcadOleHookProc)
		dc << (LPCTSTR)ResourceString(IDC_ICADOLELINKSDIALOG__NHOO_8, "\nhook function set to standard MFC hook function" );
	else
		dc << (LPCTSTR)ResourceString(IDC_ICADOLELINKSDIALOG__NHOO_9, "\nhook function set to non-standard hook function" );

	dc << "\n"/*DNT*/;
}

void CIcadOleLinksDialog::AssertValid() const
{
	COleDialog::AssertValid();
}
#endif

////////////////////////////////////////////////////////////////////////////
// CIcadOleUILinkInfo

CIcadOleUILinkInfo::CIcadOleUILinkInfo(COleDocument* pDocument)
{
	ASSERT(pDocument == NULL ||
		pDocument->IsKindOf(RUNTIME_CLASS(COleDocument)));
	m_pDocument = pDocument;
	m_pSelectedItem = NULL;
	m_pos = NULL;
	m_bUpdateLinks = FALSE;
	m_bUpdateEmbeddings = FALSE;
}

STDMETHODIMP_(ULONG) CIcadOleUILinkInfo::AddRef()
{
	return 0;
}

STDMETHODIMP_(ULONG) CIcadOleUILinkInfo::Release()
{
	return 0;
}

STDMETHODIMP CIcadOleUILinkInfo::QueryInterface(
	REFIID, LPVOID*)
{
	return E_NOTIMPL;
}

STDMETHODIMP_(DWORD) CIcadOleUILinkInfo::GetNextLink(
	DWORD dwLink)
{
	ASSERT(m_pDocument != NULL);
	if (dwLink == 0)
	{
		// start enumerating from the beginning
		m_pos = m_pDocument->GetStartPosition();
	}
	CIcadCntrItem* pItem;
	while ((pItem = (CIcadCntrItem*)m_pDocument->GetNextClientItem(m_pos)) != NULL)
	{
		// check for links
		OLE_OBJTYPE objType = pItem->GetType();
		if (m_bUpdateLinks && objType == OT_LINK)
		{
			// link found -- return it
			return (DWORD)(void*)pItem;
		}
		// check for embeddings
		if (m_bUpdateEmbeddings && objType == OT_EMBEDDED)
		{
			// embedding w/mismatched target device
			return (DWORD)(void*)pItem;
		}
	}
	return 0;   // link not found
}

STDMETHODIMP CIcadOleUILinkInfo::SetLinkUpdateOptions(
	DWORD dwLink, DWORD dwUpdateOpt)
{
	CIcadCntrItem* pItem = (CIcadCntrItem*)dwLink;
	ASSERT_VALID(pItem);
	ASSERT_KINDOF(CIcadCntrItem, pItem);
	ASSERT(pItem->GetType() == OT_LINK);

	SCODE sc;
	TRY
	{
		// item is a link -- get its link options
		pItem->SetLinkUpdateOptions((OLEUPDATE)dwUpdateOpt);
		sc = S_OK;
	}
	CATCH_ALL(e)
	{
		sc = COleException::Process(e);
//		DELETE_EXCEPTION(e);
	}
	END_CATCH_ALL

	pItem->UpdateHandItem(NULL);
	return sc;
}

STDMETHODIMP CIcadOleUILinkInfo::GetLinkUpdateOptions(
	DWORD dwLink, DWORD* lpdwUpdateOpt)
{
	CIcadCntrItem* pItem = (CIcadCntrItem*)dwLink;
	ASSERT_VALID(pItem);
	ASSERT_KINDOF(CIcadCntrItem, pItem);

	SCODE sc;
	TRY
	{
		if (pItem->GetType() == OT_LINK)
			*lpdwUpdateOpt = pItem->GetLinkUpdateOptions();
		else
			*lpdwUpdateOpt = OLEUPDATE_ALWAYS;  // make believe it is auto-link
		sc = S_OK;
	}
	CATCH_ALL(e)
	{
		sc = COleException::Process(e);
//		DELETE_EXCEPTION(e);
	}
	END_CATCH_ALL

	return sc;
}

STDMETHODIMP CIcadOleUILinkInfo::SetLinkSource(
	DWORD dwLink, LPTSTR lpszDisplayName, ULONG lenFileName,
	ULONG* pchEaten, BOOL  fValidateSource)
{
	USES_CONVERSION;

	CIcadCntrItem* pItem = (CIcadCntrItem*)dwLink;
	ASSERT_VALID(pItem);
	ASSERT_KINDOF(CIcadCntrItem, pItem);
	ASSERT(pItem->GetType() == OT_LINK);

	LPOLEOBJECT lpObject = NULL;
	CLSID clsid;

	// parse the portion known to be a file name into a file moniker
	TCHAR szName[_MAX_PATH];
	lstrcpyn(szName, lpszDisplayName, (int)lenFileName + 1);
	LPMONIKER lpmk = NULL;
	SCODE sc = CreateFileMoniker(T2COLE(szName), &lpmk);
	if (lpmk == NULL)
		return sc;

	LPBC lpbc = NULL;
	if (fValidateSource)
	{
		sc = CreateBindCtx(0, &lpbc);
		if (sc != S_OK)
		{
			lpmk->Release();
			return sc;
		}
	}

	// nUneaten is the number of chars left to parse
	UINT nUneaten = lstrlen(lpszDisplayName) - lenFileName;

	// lpszRemainder is the left over display name
	LPTSTR lpszRemainder = lpszDisplayName + lenFileName;
	*pchEaten = lenFileName;

	// parse the rest of the display name
	while (nUneaten > 0)
	{
		// attempt to parse next moniker
		ULONG nEaten = 0;
		LPMONIKER lpmkNext = NULL;
		sc = IcadParseDisplayName(lpmk, lpbc, lpszRemainder, &nEaten, &lpmkNext);
		if (sc != S_OK)
		{
			lpmk->Release();
			lpbc->Release();
			return sc;
		}

		// advance through the display name
		nUneaten -= nEaten;
		*pchEaten += nEaten;
		lpszRemainder += nEaten;

		if (lpmkNext != NULL)
		{
			// create composite out of current and next
			LPMONIKER lpmkTemp = NULL;
			sc = CreateGenericComposite(lpmk, lpmkNext, &lpmkTemp);
			if (FAILED(sc))
			{
				lpmk->Release();
				lpmkNext->Release();
				lpbc->Release();
				return sc;
			}

			// make current = next
			lpmkNext->Release();
			lpmk->Release();
			lpmk = lpmkTemp;
		}
	}

	if (fValidateSource)
	{
		// attempt to bind the the object
		sc = lpmk->BindToObject(lpbc, NULL, IID_IOleObject, (void**)&lpObject);
		if (FAILED(sc))
		{
			pItem->m_bLinkUnavail = TRUE;
			lpbc->Release();
			lpmk->Release();
			lpObject->Release();
			return sc;
		}
		ASSERT(lpObject != NULL);

		// call GetUserClassID while bound so default handler updates
		lpObject->GetUserClassID(&clsid);
		pItem->m_bLinkUnavail = FALSE;
	}

	// get IOleLink interface
	LPOLELINK lpOleLink;
	pItem->m_lpObject->QueryInterface(IID_IOleLink,(void**)&lpOleLink);
	ASSERT(lpOleLink != NULL);

	// set source from moniker
	sc = lpOleLink->SetSourceMoniker(lpmk, clsid);

	// update the cache if object was successfully bound
	if (lpObject != NULL)
	{
		lpObject->Update();
		lpObject->Release();
	}

	pItem->UpdateHandItem(NULL);
	// cleanup
	lpOleLink->Release();
	lpmk->Release();
	lpbc->Release();

	return sc;
}

STDMETHODIMP CIcadOleUILinkInfo::GetLinkSource(
	DWORD dwLink, LPTSTR* lplpszDisplayName, ULONG* lplenFileName,
	LPTSTR* lplpszFullLinkType, LPTSTR* lplpszShortLinkType,
	BOOL* lpfSourceAvailable, BOOL* lpfIsSelected)
{
	CIcadCntrItem* pItem = (CIcadCntrItem*)dwLink;
	ASSERT_VALID(pItem);
	ASSERT_KINDOF(CIcadCntrItem, pItem);
	ASSERT(pItem->GetType() == OT_LINK);

	// set OUT params to NULL
	ASSERT(lplpszDisplayName != NULL);
	*lplpszDisplayName  = NULL;
	if (lplpszFullLinkType != NULL)
		*lplpszFullLinkType = NULL;
	if (lplpszShortLinkType != NULL)
		*lplpszShortLinkType = NULL;
	if (lplenFileName != NULL)
		*lplenFileName = 0;
	if (lpfSourceAvailable != NULL)
		*lpfSourceAvailable = !pItem->m_bLinkUnavail;

	// get IOleLink interface
	LPOLELINK lpOleLink;
	pItem->m_lpObject->QueryInterface(IID_IOleLink,(void**)&lpOleLink);
	ASSERT(lpOleLink != NULL);

	// get moniker & object information
	LPMONIKER lpmk;
	if (lpOleLink->GetSourceMoniker(&lpmk) == S_OK)
	{
		if (lplenFileName != NULL)
			*lplenFileName = IcadOleGetLenFilePrefixOfMoniker(lpmk);
		lpmk->Release();
	}


	// attempt to get the type names of the link
	if (lplpszFullLinkType != NULL)
	{
		LPOLESTR lpOleStr = NULL;
		pItem->m_lpObject->GetUserType(USERCLASSTYPE_FULL, &lpOleStr);
		*lplpszFullLinkType = TASKSTRINGOLE2T(lpOleStr);
		if (*lplpszFullLinkType == NULL)
		{
			TCHAR szUnknown[256];
			VERIFY(AfxLoadString(AFX_IDS_UNKNOWNTYPE, szUnknown) != 0);
			*lplpszFullLinkType = AfxAllocTaskString(szUnknown);
		}
	}
	if (lplpszShortLinkType != NULL)
	{
		LPOLESTR lpOleStr = NULL;
		pItem->m_lpObject->GetUserType(USERCLASSTYPE_SHORT, &lpOleStr);
		*lplpszShortLinkType = TASKSTRINGOLE2T(lpOleStr);
		if (*lplpszShortLinkType == NULL)
		{
			TCHAR szUnknown[256];
			VERIFY(AfxLoadString(AFX_IDS_UNKNOWNTYPE, szUnknown) != 0);
			*lplpszShortLinkType = AfxAllocTaskString(szUnknown);
		}
	}

	// get source display name for moniker
	LPOLESTR lpOleStr = NULL;
	SCODE sc = lpOleLink->GetSourceDisplayName(&lpOleStr);
	*lplpszDisplayName = TASKSTRINGOLE2T(lpOleStr);
	lpOleLink->Release();
	if (sc != S_OK)
		return sc;

	// see if item is selected if specified
	if (lpfIsSelected)
	{
		*lpfIsSelected = (m_pSelectedItem == pItem);
	}

	return S_OK;
}

STDMETHODIMP CIcadOleUILinkInfo::OpenLinkSource(DWORD dwLink)
{
	CIcadCntrItem* pItem = (CIcadCntrItem*)dwLink;
	ASSERT_VALID(pItem);
	ASSERT_KINDOF(CIcadCntrItem, pItem);
	ASSERT(pItem->GetType() == OT_LINK);

	SCODE sc;
	TRY
	{
		// Note: no need for valid CView* since links don't activate inplace
		pItem->DoVerb(OLEIVERB_SHOW, NULL);
		sc = S_OK;
	}
	CATCH_ALL(e)
	{
		sc = COleException::Process(e);
//		DELETE_EXCEPTION(e);
	}
	END_CATCH_ALL

	return sc;
}

STDMETHODIMP CIcadOleUILinkInfo::UpdateLink(
	DWORD dwLink, BOOL /*fErrorMessage*/, BOOL /*fErrorAction*/)
{
	CIcadCntrItem* pItem = (CIcadCntrItem*)dwLink;
	ASSERT_VALID(pItem);
	ASSERT_KINDOF(CIcadCntrItem, pItem);

	SCODE sc;
	TRY
	{
		// link not up-to-date, attempt to update it
		//*** This calls our override of update link in CIcadCntrItem which does not
		//*** check for the link being up to date because it didn't work.
		if (!pItem->UpdateLink())
			AfxThrowOleException(pItem->GetLastStatus());
		pItem->m_bLinkUnavail = FALSE;
		sc = S_OK;
	}
	CATCH_ALL(e)
	{
		pItem->m_bLinkUnavail = TRUE;
		sc = COleException::Process(e);
		pItem->ReportError(sc);
//		DELETE_EXCEPTION(e);
	}
	END_CATCH_ALL

	return sc;
}

STDMETHODIMP CIcadOleUILinkInfo::CancelLink(DWORD dwLink)
{
	CIcadCntrItem* pItem = (CIcadCntrItem*)dwLink;
	ASSERT_VALID(pItem);
	ASSERT_KINDOF(CIcadCntrItem, pItem);
	ASSERT(pItem->GetType() == OT_LINK);

	SCODE sc = E_FAIL;
	TRY
	{
		if (pItem->FreezeLink())
			sc = S_OK;
	}
	CATCH_ALL(e)
	{
		sc = COleException::Process(e);
//		DELETE_EXCEPTION(e);
	}
	END_CATCH_ALL

	// report error
	if (sc != S_OK)
		pItem->ReportError(sc);

	pItem->UpdateHandItem(NULL);
	return S_OK;
}

STDMETHODIMP CIcadOleUILinkInfo::GetLastUpdate(DWORD dwLink, FILETIME*)
{
	CIcadCntrItem* pItem = (CIcadCntrItem*)dwLink;
	ASSERT_VALID(pItem);

	// Note: leave last update time at unknown!

	return S_OK;
}


