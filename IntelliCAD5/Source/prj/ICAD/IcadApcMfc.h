/* D:\INTELLICAD\PRJ\ICAD\ICADAPCMFC.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.1.1.1 $ $Date: 2000/01/19 13:41:16 $ 
 * 
 * Abstract
 * 
 *  Extracts from apcMfc.h and other VBA SDK files to create dummy wrappers
 * 
 */ 

#ifndef ICADAPCMFC_H_INCLUDED			// protect against multiple inclusions
#define ICADAPCMFC_H_INCLUDED

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#ifndef __ATLBASE_H__
	#error include <atlbase.h> before including this file
#endif

#ifndef __ATLCOM_H__
	#error include <atlcom.h> before including this file
#endif


namespace MSAPC 
{

#define APC_RAW(name) raw_##name
#define APC_GET(name) get_##name
#define APC_PUT(name) put_##name
#define APC_PUTREF(name) putref_##name
#define ApcCallHR(call) { hr=(call); if (FAILED(hr)) return(hr); }
#define ApcCallV(call) { HRESULT hr=(call); if (FAILED(hr)) { _ASSERTE(FALSE); return; }}


struct Picture: IPictureDisp {};

enum AxHelpStyle
{
    axHelpHTML = 0,
    axHelpWindows = 1
};

enum AxSecurityLevel
{
    axSecurityNone = 1,
    axSecurityMedium = 2,
    axSecurityHigh = 3
};

enum AxProjectFlag
{
    axProjectNormal = 0,
    axProjectHidden = 1,
    axProjectDisableMacros = 2,
    axProjectDisableSave = 4,
    axProjectThrowAwayCompiledState = 8
};

enum AxProjectMode
{
    axModeDesign = 0,
    axModeRun = 1
};

enum AxProjectItemType
{
    axTypeCodeModule = 0,
    axTypeClassModule = 1,
    axTypeDesigner = 2,
    axTypeHostProjectItem = 3,
    axTypeHostClass = 4,
    axTypeOther = 5
};


struct __declspec(uuid("eab38d22-26dc-11d2-98c0-00104b24170b"))
IApc : IDispatch
{
    HRESULT __stdcall put_HelpStyle (
        enum AxHelpStyle pStyle ) { return NOERROR; };
    HRESULT __stdcall put_EnableDigitalSignatures (
        VARIANT_BOOL pbEnabled ) { return NOERROR; };
    HRESULT __stdcall put_SecurityLevel (
        enum AxSecurityLevel pSecurityLevel ) { return NOERROR; };
    HRESULT __stdcall put_Picture (
        struct Picture * ppPicture ) { return NOERROR; };
    HRESULT __stdcall put_BackColor (
        OLE_COLOR pBackColor ) { return NOERROR; };
    HRESULT __stdcall get_MiniBitmaps (
        struct IApcMiniBitmaps * * ppMiniBitmaps ) { return NOERROR; };
    HRESULT __stdcall raw_ShowSecurityLevelDialog (
        enum AxSecurityLevel * SecurityLevel )  { return NOERROR; };
};

struct __declspec(uuid("eab38d2e-26dc-11d2-98c0-00104b24170b"))
IApcProject : IDispatch
{
    HRESULT __stdcall put_Name (
        BSTR pbstrName ) {return NOERROR;};
    HRESULT __stdcall put_Dirty (
        VARIANT_BOOL pbIsDirty ) {return NOERROR; };
    HRESULT __stdcall get_Mode (
        enum AxProjectMode * pMode ) {return NOERROR; };
};

struct __declspec(uuid("eab38d34-26dc-11d2-98c0-00104b24170b"))
IApcProjectItem : IDispatch
{
    HRESULT __stdcall get_Name (
        BSTR * pbstrName ) { return E_NOTIMPL;};
    HRESULT __stdcall put_Name (
        BSTR pbstrName ) { return E_NOTIMPL;};
    HRESULT __stdcall put_AllowViewObject (
        VARIANT_BOOL pbAllowViewObject )  { return E_NOTIMPL;};
    HRESULT __stdcall Activate ( )  { return E_NOTIMPL;};
};

struct __declspec(uuid("eab38d63-26dc-11d2-98c0-00104b24170b"))
IApcHostAddIns : IDispatch
{
	IDispatch * GetAddIns ( ) { return NULL; };
	HRESULT ShowAddInsDialog ( ) {return NOERROR; };
};

struct __declspec(uuid("eab38d46-26dc-11d2-98c0-00104b24170b"))
IApcMiniBitmaps : IDispatch
{
    HRESULT __stdcall raw_Add (
        BSTR ClassName,
        struct Picture * MiniBitmap,
        OLE_COLOR BackColor,
        struct IApcMiniBitmap * * ppMiniBitMap ){ return NOERROR; };
};

struct __declspec(uuid("eab38d48-26dc-11d2-98c0-00104b24170b"))
IApcMiniBitmap : IDispatch
{
};

struct __declspec(uuid("eab38d60-26dc-11d2-98c0-00104b24170b"))
IApcCompManager : IDispatch
{
    HRESULT __stdcall raw_PreTranslateMessage (
        void * pMsg,
        long * pbConsumed ) { return NOERROR; };
};





class _HostMember : public CComPtr<IApc>
{
public:
    BOOL m_fHostIsActiveComponent;
	IApcCompManager* GetApcCompManager() { return NULL; }
	HRESULT Create(HWND hWnd,BSTR bstrHostName,IDispatch* pdisp,BSTR bstrLicenseKey = NULL,long nLcid = 0,BSTR bstrFileFilter = NULL,BSTR bstrRegKey = NULL){return NOERROR;}
	void WmClose(BOOL& fTerminated, HWND hWndClosing = NULL) { }
	void WmDestroy() { }
	void WmActivate(WPARAM wParam) { }
	void WmEnable(WPARAM wParam) { }
};

class CApcHost
{
public:

	_HostMember ApcHost;
	CApcHost()  {}

}; 


template <class _ProjectOwner>
class CApcProjectMember: public CComPtr<IApcProject>
{
public:
	CApcProjectMember() { }
	~CApcProjectMember() { }
	HRESULT Open(IApc* pApcHost, AxProjectFlag Flags) {return E_NOTIMPL;}
	void Close() {}
	HRESULT Create(IApc* pApcHost, AxProjectFlag Flags, BSTR bstrName = NULL) {return E_NOTIMPL;}
	HRESULT FinishLoading() {return E_NOTIMPL;}

	STDMETHOD(InitNew)(IStorage* pStg) {return E_NOTIMPL;}
	STDMETHOD(Load)(IStorage* pStg) {return E_NOTIMPL;}
	STDMETHOD(Save)(IStorage* pStg, BOOL fSame) {return E_NOTIMPL;}
	STDMETHOD(SaveCompleted)(IStorage* pStg) {return E_NOTIMPL;}

};

template <class T, class _ProjectMember = CApcProjectMember<T> >
class CApcProject
{
public:
	_ProjectMember ApcProject;

	CApcProject(const OLECHAR* pszStgName = L"apc") {}
};


class CApcProjectItemPtr: public CComPtr<IApcProjectItem>
{
public:
	long ID() {return 0;}
	template <class Project>
	HRESULT Register(Project& project, IDispatch* pdisp, long nID) { return E_NOTIMPL;}
};


template <class _ProjectItemOwner>
class CApcProjectItemMember: public CApcProjectItemPtr
{
public:
	template <class _Project>
	HRESULT Define(_Project& project, IDispatch* pdisp, AxProjectItemType projectItemType, BSTR bstrRequestName, BSTR* pbstrActualName = NULL) { return E_NOTIMPL;}
};

template <class T, class _ProjectItemMember = CApcProjectItemMember<T> >
class CApcProjectItem
{
public:
	_ProjectItemMember ApcProjectItem;

	CApcProjectItem()  {}
};


template <class T, class AppBase = CWinApp>
class CApcApplication : 
	public AppBase, 
	public CApcHost
{
public:
	CApcApplication() {	}
	BOOL ApcOleInit() { return AfxOleInit(); }
	BOOL OnIdle(LONG lCount) { return FALSE; }
	BOOL PreTranslateMessage(MSG* pMsg) { return AppBase::PreTranslateMessage(pMsg); }
	virtual int Run() {	return ExitInstance(); }
	virtual int ExitInstance() { return AppBase::ExitInstance(); }
	HRESULT ApcHost_OnCreate() { return E_NOTIMPL;}

	//{{AFX_MSG(CApcApplication)
	afx_msg void OnVbaIde() {}
	afx_msg void OnUpdateVbaIde(CCmdUI* pCmdUI) {}
	afx_msg void OnVbaPropertiesWindow() {}
	//}}AFX_MSG

};

template <class T, class DocBase = COleServerDoc>
class CApcDocument :
	public DocBase,
	public CApcProject<T>
{
public:
	CApcDocument() { }
    CApcDocument(OLECHAR* pStgName) : CApcProject<T>(pStgName) { }
	virtual BOOL OnNewDocument() { return DocBase::OnNewDocument();}
	void PreCloseFrame(CFrameWnd* pFrame) { DocBase::PreCloseFrame(pFrame);}
	void DeleteContents() { DocBase::DeleteContents();}
	BOOL IsModified() { return DocBase::IsModified();}
	HRESULT ApcLoad() { return S_OK;}
};

/////////////////////////////////////////////////////////////////////////////


template<class Owner>
class CApcHostAddInsMember : public CComPtr<IApcHostAddIns>
	{
	public:
		CApcHostAddInsMember(Owner* pOwner) { }
		~CApcHostAddInsMember() { }
				
		void* GetApcHostAddIns() { return NULL; }
		HRESULT Create(IDispatch* pDispHost,BOOL fRemoveOnFailure = TRUE, BOOL fDisableOnStartup = FALSE,BSTR bstrLicKey = NULL, long nLcid = 0,BSTR bstrRegPath = NULL) { return E_NOTIMPL;}
		HRESULT Load()  { return E_NOTIMPL;}
		void Destroy() {}
	};

template <class T, class _AddInsMember = CApcHostAddInsMember<T> >
class CApcHostAddIns
	{
	public:
		_AddInsMember ApcHostAddIns;
		
		CApcHostAddIns() : ApcHostAddIns(static_cast<T*>(this)) {}
	};

template<class Owner>
class CIcadRecorderMember : public CComPtr<IDispatch>
{
public:
	CIcadRecorderMember(Owner* pOwner)  { }

	template <class ProjectOwner>
	HRESULT Create(ProjectOwner* pProjectOwner, const char* name) { return E_NOTIMPL;}
	template <class ProjectOwner>
	HRESULT Run(ProjectOwner* pProjectOwner, const char* title){ return E_NOTIMPL;}
	HRESULT RunMacro (BSTR MacroName) { return E_NOTIMPL;}
};

class CIcadRecorder
{
public:
	CIcadRecorderMember<CIcadRecorder> ApcRecorder;
	CIcadRecorder() : ApcRecorder(this) { }
};


} // namespace MSAPC

#endif



