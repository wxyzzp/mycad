
/*DG - 11.2.2002*/// Some changes/additions in the following comments only.
/*DG - 1.2.2002*/// Here is a simple example of memory leaks alerter.
// It works in debug build (with _DEBUG defined) when USE_MEM_LEAKS_ALERTER
// is defined. It shows a message box on exiting of the program if memory
// leaks are detected. Only Normal and Client memory blocks are checked.
//
// Implementation notes:
//  All work is performed in ctor & dtor of the global object defined here,
//  and therefore this object must be constructed as soon as possibly when the
//  program starts, this is accomplished by
//  1) defining this object in icadauth.dll and
//  2) using init_seg(compiler) pragma.
//  It placed here because icadauth.dll is loaded first among icad's dlls.
//  "...what is the algorithm for determining the order in which the modules will be
//   initialized? The algorithm Windows NT uses has changed at least once, and it
//   would be nice to have a Microsoft technical note that at least gives some guidelines"
//  - said Matt Pietrek in his UnderTheHood Sep'1999 in MSDN . Also, some useful info
//  can be in Russ Osterlund's article about Windows 2000 Loader in MSDN Mar'2002.
//  (it's in newsstands now only :)
//  To know if the object is constructed 1st on starting set a breakpoint in
//  _initterm function in CRT0DAT.C file called from _DllMainCRTStartup
//  and step into (**pfbegin)(); call - you will see what the lib is being initialized.
//
//  If this implementation will not work in some case then it should be turned off
//  (let it be: it doesn't work when VC debug output window gives different results).


#define USE_MEM_LEAKS_ALERTER

#if defined(_DEBUG) && defined(USE_MEM_LEAKS_ALERTER)


#pragma warning(disable: 4074)
#pragma init_seg(compiler)


#include <windows.h>
#include <crtdbg.h>
#include <stdio.h>


class CMemLeaksAlerter
{
public:
	CMemLeaksAlerter()
	{
		_CrtMemCheckpoint(&s_memState_init);
	}

	~CMemLeaksAlerter()
	{
		_CrtMemState	memState_exit, memState_diff;
		_CrtMemCheckpoint(&memState_exit);
		if( _CrtMemDifference(&memState_diff, &s_memState_init, &memState_exit) &&
			(memState_diff.lSizes[_NORMAL_BLOCK] || memState_diff.lSizes[_CLIENT_BLOCK]) )
		{
			char	pMsgStr[200], pModuleName[200];
			GetModuleFileName(NULL, pModuleName, 100);
			sprintf(pMsgStr, "Memory leaks in %s:\n%ld bytes in Normal blocks\n%ld bytes in Client blocks",
					pModuleName, memState_diff.lSizes[_NORMAL_BLOCK], memState_diff.lSizes[_CLIENT_BLOCK]);
			MessageBox(NULL, pMsgStr, "!!! WARNING !!!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
			// maybe TO DO: write detailed info about blocks
		}
	}

private:
	static _CrtMemState s_memState_init;
} g_memleaksAlerter;

_CrtMemState CMemLeaksAlerter::s_memState_init;


#endif // _DEBUG && USE_MEM_LEAKS_ALERTER
