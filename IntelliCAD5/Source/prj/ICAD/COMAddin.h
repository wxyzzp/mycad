#ifndef _COMADDIN_H
#define _COMADDIN_H

class CAutoDoc;

class COMAddin
{
public:
	COMAddin();
	~COMAddin();

								// IDTExtensibility2
	HRESULT load(CIcadApp* icad);
	HRESULT destroy(void);

								// ICADWorkspaceEvents
	HRESULT fireWorkspaceStartup(IIcadWorkspace* icad_ws);
	HRESULT fireWorkspaceDocumentEvent(IIcadWorkspace* icad_ws, int id,
									   IIcadDocument* doc);
	HRESULT fireWorkspaceBeforeExit(IIcadWorkspace* icad_ws);
	HRESULT fireWorkspaceExit(IIcadWorkspace* icad_ws);
	HRESULT fireAddinRun(IIcadWorkspace* icad_ws, const char* action);
	HRESULT fireWorkspaceReinit(IIcadWorkspace* icad_ws);

								// _EIcadDocument
	HRESULT fireDocumentEvent(int id, CAutoDoc* doc);
private:

	CComQIPtr<IDispatch> addin;
};

#endif
