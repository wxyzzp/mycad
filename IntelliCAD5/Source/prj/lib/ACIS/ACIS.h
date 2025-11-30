// File  : <DevDir>\source\prj\lib\acis\ACIS.h
// Author: Dmitry Gavrilov

#if _MSC_VER > 1000
#pragma once
#endif

#pragma warning (disable : 4786)	// for calm using of std containers templates

#ifndef _ACIS_H_
#define _ACIS_H_

// exported APIs
#ifdef ACIS_EXPORTS
 #define ACIS_API __declspec(dllexport)
#else
 #define ACIS_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

class CModeler;
ACIS_API CModeler* createModeler();
ACIS_API CModeler* createViewer();

#ifdef __cplusplus
};
#endif


#pragma comment(lib, "Delayimp.lib")

#define A3DT_MAJOR "1"
#define A3DT_MINOR "2"

#define makeAcisDllName(NAME) NAME##".dll"

// following dlls used if BUILD_WITH_LIMITED_ACIS_SUPPORT is commented
#ifdef _DEBUG
#define dll_SpaACIS		makeAcisDllName("SpaACISd")
#define dll_SpaBase		makeAcisDllName("SpaBased")
#else
#define dll_SpaACIS		makeAcisDllName("SpaACIS")
#define dll_SpaBase		makeAcisDllName("SpaBase")
#endif // _DEBUG

#define DelayLoad(NAME) "/DelayLoad:"##"\""##NAME##"\""

//#pragma comment(linker, DelayLoad(dll_SpaBase))
//#pragma comment(linker, DelayLoad(dll_SpaACIS)) 

#pragma comment(linker, "/Delay:unload") 

#endif // _ACIS_H_
