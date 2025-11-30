// File  : 
// Author: Alexey Malov
#include "icadlib.h" // ResourseString
#include "Modeler.h"
#include "ModelerStub.h"
#include <assert.h>
#include "../../ICAD/res/icadrc2.h"
#include "../../ICAD/Configure.h"

#include <Delayimp.h>

using icl::point;
using icl::gvector;
using icl::transf;
using geodata::curve;
using std::vector;


CModelerLicense* CModelerLicense::get()
{
	static CModelerLicenseImpl s_license;
	return &s_license;
}


CModeler* CModeler::s_pModeler = 0;

HMODULE& getModelerDllHandle()
{
    static HMODULE s_hModelerDll = NULL;
    return s_hModelerDll;
}


static LONG DelayLoadDllExceptionFilter(PEXCEPTION_POINTERS pep) 
{
	// Assume we recognize this exception
	LONG lDisposition = EXCEPTION_EXECUTE_HANDLER;  

	// If this is a Delay-load problem, ExceptionInformation[0] points 
	// to a DelayLoadInfo structure that has detailed error info
	PDelayLoadInfo pdli = PDelayLoadInfo(pep->ExceptionRecord->ExceptionInformation[0]);

	struct resbuf rbCMDECHO;
	if(IcadSharedGlobals::CallGetVar(DB_QCMDECHO, &rbCMDECHO) != RTNORM)
		rbCMDECHO.resval.rint = 1;

	char buf[IC_ACADBUF];

	switch(pep->ExceptionRecord->ExceptionCode) 
	{
	case VcppException(ERROR_SEVERITY_ERROR, ERROR_MOD_NOT_FOUND):
		// The DLL module was not found at runtime
		if(rbCMDECHO.resval.rint)
		{
			//sprintf(buf, ResourceString(IDC_DLL_NOT_FOUND, "Dll not found: %s"), pdli->szDll); 
			//MessageBox(NULL, buf, ResourceString(DNT_ICADAPP_INTELLICAD_1, "IntelliCAD" ), MB_OK);
			IcadSharedGlobals::CallCmdMessage(ResourceString(IDC_ACIS_DLLS_ARE_ABSENT, 
				"\nSome dlls for editing acis solids are absent. Solids can be viewed but not edited."));
		}
		break;

	case VcppException(ERROR_SEVERITY_ERROR, ERROR_PROC_NOT_FOUND):
		// The DLL module was found, but it doesn't contain the function
		assert(pdli->dlp.fImportByName);
		if(rbCMDECHO.resval.rint)
		{
			sprintf(buf, ResourceString(IDC_FUNCTION_NOT_FOUND, "Function %s was not found in %s"), 
					pdli->dlp.szProcName, pdli->szDll);
			IcadSharedGlobals::CallCmdMessage(buf);
			//MessageBox(NULL, buf, ResourceString(DNT_ICADAPP_INTELLICAD_1, "IntelliCAD" ), MB_OK);
		}
		break; 

	default:
		// We don't recognize this exception
		lDisposition = EXCEPTION_CONTINUE_SEARCH;  
		break;
	}
	return lDisposition;
}

// this function catches case if a acis dll module was not found at runtime
static CModeler* safeCreator(CModeler* (*createModelerFunc)())
{
	CModeler* pModeler = NULL;
	__try
	{
		pModeler = createModelerFunc();
	}
	__except(DelayLoadDllExceptionFilter(GetExceptionInformation())) 
	{
	}
	return pModeler;
}

int CModeler::start()
{
    // D.G.
	// main functionality lines in this function are marked by numbers
	// TO DO. Show ACIS_LOADING_MODELER_MSG message.

    if (s_pModeler)
        return 1;

#if defined(BUILD_WITH_ACIS_SUPPORT)
    // In case of absence of ACIS modeler implementation, 
    // provide transition to default (empty) modeler implementation by breaking
    // this fictive endless loop, 
    do
    {
        // Try to load ACIS
        assert(!GetModuleHandle(ResourceString(ACIS_DLL_NAME, "acis.dll")));
		if(CModelerLicense::get()->getLicenseLevel() == CModelerLicense::eNoSupport)
		{
            // ??? MessageBox(NULL, "ERROR: You need to upgrade your license to use ACIS support.", ResourceString(DNT_ICADAPP_INTELLICAD_1, "IntelliCAD" ), MB_OK);
            break;
		}
        //sds_printf("Loading modeler's DLLs");
	    if (!(getModelerDllHandle() = LoadLibrary(ResourceString(ACIS_DLL_NAME, "acis.dll"))))
        {
			struct resbuf rbExpert;
			if(IcadSharedGlobals::CallGetVar(DB_QEXPERT,&rbExpert) != RTNORM
				|| rbExpert.resval.rint <= 2)
				MessageBox(NULL, ResourceString(ACIS_MSG_LOAD_DLL_FAILED, "ERROR: Could not load acis.dll library."),ResourceString(DNT_ICADAPP_INTELLICAD_1, "IntelliCAD" ), MB_OK);
			else
				IcadSharedGlobals::CallCmdMessage(ResourceString(ACIS_MSG_LOAD_DLL_FAILED, "ERROR: Could not load acis.dll library."));
            break;
        }

	    CModeler* (*createModelerFunc)();
getModelerFunc:
#if defined(BUILD_WITH_LIMITED_ACIS_SUPPORT)
	    if (!(createModelerFunc = (CModeler*(*)())GetProcAddress(getModelerDllHandle(), "createViewer")))
#else
	    if (!(createModelerFunc = CModelerLicense::get()->getLicenseLevel() == CModelerLicense::eFull ?
								  (CModeler*(*)())GetProcAddress(getModelerDllHandle(), "createModeler") :
								  (CModeler*(*)())GetProcAddress(getModelerDllHandle(), "createViewer")))
#endif
	    {
		    FreeLibrary(getModelerDllHandle());
		    getModelerDllHandle() = NULL;
		    MessageBox(NULL, ResourceString(ACIS_MSG_WRONG_DLL, "ERROR: acis.dll library is wrong."),ResourceString(DNT_ICADAPP_INTELLICAD_1, "IntelliCAD" ), MB_OK);
            break;
	    }

		s_pModeler = safeCreator(createModelerFunc);
		if(!s_pModeler)
        {
			if(CModelerLicense::get()->getLicenseLevel() == CModelerLicense::eFull)
			{
				CModelerLicense::get()->setLicenseLevel(CModelerLicense::eLimited);
				//IcadSharedGlobals::CallUpdateAppTitle();
				goto getModelerFunc; // Try open us Viewer
			}

		    FreeLibrary(getModelerDllHandle());
		    getModelerDllHandle() = NULL;
			MessageBox(NULL, ResourceString(ACIS_MSG_INIT_MODELER_FAILED, "ERROR: Could not initialize the Modeler."),ResourceString(DNT_ICADAPP_INTELLICAD_1, "IntelliCAD" ), MB_OK);
            break;
        }
        return 1;
    }
    while (false);
#endif

    // Create default (empty) modeler
	{
        static CModelerStub s_modeler;
        s_pModeler = &s_modeler;
	}

	return 1;
}

int CModeler::stop()
{
	if(!getModelerDllHandle())
		return 1;

	// Bug Fixes: free memory caches in ACIS DLL before unload DLL.
	//
	CModeler::get()->clear();

	FreeLibrary(getModelerDllHandle());	// unload library anyway
    getModelerDllHandle() = NULL;

	return 1;
}

CModeler* CModeler::get()
{
    if (!s_pModeler)
    {
        if (!start())
            return 0;
    }
    
    assert(s_pModeler);

    return s_pModeler;
}

int CModeler::checkFullModeler()
{
#if defined(BUILD_WITH_ACIS_SUPPORT)
	
#	if defined(BUILD_WITH_LIMITED_ACIS_SUPPORT)

		return 0;

#	else

		CModeler* p = get();
		if (p->canModify())
			return 1;
		return 0;

#	endif
#else

	return 0;

#endif
}

int CModeler::checkLimitedModeler()
{
#if defined(BUILD_WITH_ACIS_SUPPORT)
	
	CModeler* p = get();
	if (p && p->canView())
		return 1;
	return 0;

#else

	return 0;

#endif
}
