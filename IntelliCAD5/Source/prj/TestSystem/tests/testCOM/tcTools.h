//File: <DevDir>/source/prj/TestSystem/tests/COM/tcTools.h
//

#pragma once


// standard precompiled header defines/includes

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

//#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC OLE automation classes
//#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


// our precompiled stuff

namespace ICAD_AUTOMATION
{
#include "IcadTLB.h"
}
using namespace ICAD_AUTOMATION;

#include "testunit/collection.h"
#include "testunit/failure.h"
using namespace testunit;


// This function initializes the COM library by CoInitialize,
// creates an instance of IntelliCAD app, sets it visible and
// returns the pointer to it in success or NULL in failure.
IIcadApplication* getIcadApp();

// This function calls IIcadApplication::Quit,
// releases the reference to the IIcadApplication object
// and closes the COM library by CoUninitialize.
void releaseIcadApp();

// This function calls IIcadApplication::RunCommand and waits until
// the command is finished.
void commandRunAndFinish(BSTR* pCommandStr, test* pTest, IIcadDocument* pDoc);


// macros for convenient usage of testunit's messaging/asserting mechanisms,
// with "1" - within any function and
// with "2" suffix - for using within test classes methods

#define _tellOnFail1(funcall, message, _test) \
if(FAILED(funcall)) \
	tout << failure(testunit::exception(message, __FILE__, __LINE__), _test)

#define _tellOnFail2(funcall, message) \
_tellOnFail1(funcall, message, this)

#define _tellIfNot1(condition, message, _test) \
if(!(condition)) \
	tout << failure(testunit::exception(message, __FILE__, __LINE__), _test)

#define _tellIfNot2(condition, message) \
_tellIfNot1(condition, message, this)


#define _stopIfNot(condition, message) \
assertex(condition, message)

#define _stopOnFail(funcall, message) \
assertex(SUCCEEDED(funcall), message)
