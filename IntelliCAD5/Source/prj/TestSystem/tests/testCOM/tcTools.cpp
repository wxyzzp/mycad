//File: <DevDir>/source/prj/TestSystem/tests/COM/tcTools.cpp
//

#include "tcTools.h"
#include "testunit/registry.h"
#include <atlbase.h>

#define CLSID_DEFINED
namespace ICAD_AUTOMATION
{
#include "../lib/auto/icad_i.c"
}
using namespace ICAD_AUTOMATION;


// this one is necessary for linking of testCOM.dll to its.exe
extern "C" __declspec(dllexport) void exportSymbol(){}


// this class is for internal usage only; getIcadApp and releaseIcadApp functions use it
class CIcadAppSingleton
{
	friend IIcadApplication* getIcadApp();
	friend void releaseIcadApp();

public:
	CIcadAppSingleton() : m_pIcadApp(NULL)
	{
	}

	~CIcadAppSingleton()
	{
		releaseIcadApp();
	}

private:
	IIcadApplication* getIcadApp()
	{
		if(!m_pIcadApp)
		{
			if( FAILED(CoInitialize(NULL)) ||
				FAILED(CoCreateInstance(CLSID_Application, NULL, CLSCTX_ALL, __uuidof(m_pIcadApp), (void**)&m_pIcadApp)) ||
				!m_pIcadApp ||
				FAILED(m_pIcadApp->put_Visible(VARIANT_TRUE)) )
			{
				m_pIcadApp = NULL;
			}
		}
		return m_pIcadApp;
	}

	void releaseIcadApp()
	{
		if(m_pIcadApp)
		{
			m_pIcadApp->Quit();
			m_pIcadApp->Release();
		}
		m_pIcadApp = NULL;
		CoUninitialize();
	}

	IIcadApplication* m_pIcadApp;

} g_icadAppSingleton;


// functions for init/de-init COM and IcadApp
IIcadApplication* getIcadApp()
{
	return g_icadAppSingleton.getIcadApp();
}


void releaseIcadApp()
{
	g_icadAppSingleton.releaseIcadApp();
}


// the root collection of tests of COM
testunit::collection& base_collection()
{
	using namespace testunit;
	class base_test_collection: public collection
	{
	public :
		base_test_collection() : collection("Root collection of tests for IntelliCAD COM API")
		{
			testunit::registry::get().add(this);
		}

		virtual void run()
		{
			try
			{
				collection::run();
				releaseIcadApp();
			}
			catch(...)
			{
				releaseIcadApp();
			}
		}
	};

	static base_test_collection* s_base = new base_test_collection;
	return *s_base;
}


void commandRunAndFinish(BSTR* pCommandStr, test* pTest, IIcadDocument* pDoc)
{
	_tellOnFail1(getIcadApp()->RunCommand(*pCommandStr), "IIcadApplication::RunCommand failed", pTest);

// See WaitForCommandFinish zwikipage for details.
// Here are 3 ways for waiting until the command is finished:
// 1. check CMDACTIVE var - it doesn't work, maybe CMDNAMES should be taken in account as well(?);
// 2. run SETVAR command for USERR5 sysvar and check it after - the main disadvantage is it affects undo stack;
// 3. use PMTHIST and GRAPHSCR commands and check SCREENMODE sysvar - not too nice but works and doesn't affect undo stack.
// These ways a shown below and the 3rd one is used.

/*
	time_t				start, finish;
	BSTR				varName = SysAllocString(L"CMDACTIVE");
	CComVariant			varValue;
	const CComVariant	zeroValue((short)0);
	pDoc->GetVariable(varName, &varValue);

	time(&start);
	time(&finish);
	while(varValue == zeroValue && difftime(finish, start) < 1.0)
	{
		pDoc->GetVariable(varName, &varValue);
		time(&finish);
	}

	time(&start);
	time(&finish);
	while(varValue != zeroValue && difftime(finish, start) < 1.0)
	{
		pDoc->GetVariable(varName, &varValue);
		time(&finish);
	}
	SysFreeString(varName);
*/

/*
	time_t				start, finish;
	BSTR				varName = SysAllocString(L"USERR5"), cmdStr;
	CComVariant			oldVarValue, curVarValue;
	const CComVariant	newVarValue(3.14);

	pDoc->GetVariable(varName, &oldVarValue);

	cmdStr = SysAllocString(L"(command \"setvar\" \"USERR5\" 3.14)");
	getIcadApp()->RunCommand(cmdStr);

	// wait until USERR5 changes to new value, but not for too long
	pDoc->GetVariable(varName, &curVarValue);
	time(&start);
	time(&finish);
	while(curVarValue != newVarValue && difftime(finish, start) < 3.0)
	{
		pDoc->GetVariable(varName, &curVarValue);
		time(&finish);
	}

	// now undo our dummy command
	SysReAllocString(&cmdStr, L"(command \"u\")");
	getIcadApp()->RunCommand(cmdStr);
	SysFreeString(cmdStr);
	SysFreeString(varName);
*/

	time_t				start, finish;
	const BSTR			varName = SysAllocString(L"SCREENMODE"),
						graphscrStr = SysAllocString(L"GRAPHSCR"), pmthistStr = SysAllocString(L"PMTHIST");
	CComVariant			oldVarValue, curVarValue;
	const CComVariant	zeroVarValue((short)0);

	// get old sysvar value
	pDoc->GetVariable(varName, &oldVarValue);
	// run according command
	if(oldVarValue == zeroVarValue)
		getIcadApp()->RunCommand(graphscrStr);
	else
		getIcadApp()->RunCommand(pmthistStr);
	// wait until SCREENMODE changes to new value, but not for too long
	pDoc->GetVariable(varName, &curVarValue);
	time(&start);
	time(&finish);
	while(curVarValue == oldVarValue && difftime(finish, start) < 3.0)
	{
		pDoc->GetVariable(varName, &curVarValue);
		time(&finish);
	}
	// restore old state
	if(oldVarValue != zeroVarValue)
		getIcadApp()->RunCommand(graphscrStr);
	else
		getIcadApp()->RunCommand(pmthistStr);
	// wait until SCREENMODE changes to old value, but not for too long
	pDoc->GetVariable(varName, &curVarValue);
	time(&start);
	time(&finish);
	while(curVarValue != oldVarValue && difftime(finish, start) < 3.0)
	{
		pDoc->GetVariable(varName, &curVarValue);
		time(&finish);
	}

	SysFreeString(graphscrStr);
	SysFreeString(pmthistStr);
	SysFreeString(varName);
}
