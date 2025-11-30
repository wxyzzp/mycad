// Registrar.h: interface for the OdAxTypeLibRegistrar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGISTRAR_H__233580DF_A612_4625_A3AA_2336FCE65460__INCLUDED_)
#define AFX_REGISTRAR_H__233580DF_A612_4625_A3AA_2336FCE65460__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <atlbase.h>
#include <comdef.h>
#include "OdString.h"
#include "RegExport.h"

/**
    {group:Other_Classes} 
*/
class DDREG_API OdAxTypeLibRegistrar  
{
  ITypeLibPtr m_pLib;
  TLIBATTR    m_tlbAttr;
  OdString    m_sDLLPath;
  OdString    m_sTlbGUID;
  OdString    m_sNameSpace;
  OdString    m_sCurVer;
  OdString    m_sCurVerHex;
  OdString    m_sCurVerUpdate;  // To update Version-Independent ProgID values
  int         m_nToolBoxBitmap;
  DWORD       m_nMiscStatus;
  
  UINT openTypeLib( OdString sDLLPath );

  void regTypeLib();
  void unregTypeLib();
	void registerType(ITypeInfo2* pType);
	void registerCoClass(ITypeInfo2* pType);
	void unregisterType(ITypeInfo2* pType, OdString ver);
	void unregisterCoClass(ITypeInfo2* pType, OdString ver);
  void registerVersionIndependentProgid( 
    LPCTSTR vi_progid, LPCTSTR name, LPCTSTR guid, LPCTSTR progid ) const;
  bool isVersionHigher( int major, int minor ) const;
  // determine the youngest tlb left (for CurVer replacement)
  OdString getMaxVersionLeft();
  int majorVer() const { return m_tlbAttr.wMajorVerNum; }
  int minorVer() const { return m_tlbAttr.wMinorVerNum; }
  const OdString& decimalVerStr() const { return m_sCurVer; }
  const OdString& hexVerStr() const { return m_sCurVerHex; }
public:
	OdAxTypeLibRegistrar();
	virtual ~OdAxTypeLibRegistrar();

  void regTypeLib( OdString sDLLPath, int toolBoxBitmap = 1, DWORD nMiscStatus = OLEMISC_RECOMPOSEONRESIZE );
  
  void unregTypeLib( OdString sDLLPath);
};

DDREG_API HRESULT odAxRegisterTypeLib(const char* sPath, int toolBoxBitmap = 1, DWORD nMiscStatus = OLEMISC_RECOMPOSEONRESIZE );
DDREG_API HRESULT odAxUnregisterTypeLib(const char* sPath);


#endif // !defined(AFX_REGISTRAR_H__233580DF_A612_4625_A3AA_2336FCE65460__INCLUDED_)

