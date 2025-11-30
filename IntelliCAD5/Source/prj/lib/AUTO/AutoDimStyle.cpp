/* INTELLICAD\PRJ\LIB\AUTO\AUTODIMSTYLE.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Implementation of CAutoDimensionStyle - the Automation DimensionStyle object
 * 
 */ 

#include "StdAfx.h"
#include "autodimstyle.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoDimensionStyle

HRESULT CAutoDimensionStyle::Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem)
{
	CAutoDbItem::Init(pDoc, pParent);
		
	if (pDbHandItem)
		m_pDbHandItem = pDbHandItem;
	else
		m_pDbHandItem = new db_dimstyletabrec();
		
	if (NULL == m_pDbHandItem) 
		return FALSE;

	return NOERROR;
}
	
	// IIcadDimensionStyle
STDMETHODIMP CAutoDimensionStyle::get_DimPost(BSTR *pDimPost)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimPost == NULL)
		return E_POINTER;
	
	char* pStr;
	m_pDbHandItem->get_3(&pStr);
	CString str (pStr);
	*pDimPost = str.AllocSysString();
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimPost(BSTR DimPost)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str(DimPost);
	if (str.IsEmpty ())
		return E_INVALIDARG;

	if ( !m_pDbHandItem->set_3 ((char*)str.GetBuffer(0)) )
		return E_FAIL;
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimApost(BSTR *pDimApost)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimApost == NULL)
		return E_POINTER;
	
	char* pStr;
	m_pDbHandItem->get_4(&pStr);
	CString str (pStr);
	*pDimApost = str.AllocSysString();
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimApost(BSTR DimApost)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str(DimApost);
	if (str.IsEmpty ())
		return E_INVALIDARG;

	if ( !m_pDbHandItem->set_4 ((char*)str.GetBuffer(0)) )
		return E_FAIL;
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimBlk(BSTR *pDimBlk)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimBlk == NULL)
		return E_POINTER;
	
	char* pStr;
	((db_dimstyletabrec*)m_pDbHandItem)->get_5(&pStr);
	CString str (pStr);
	*pDimBlk = str.AllocSysString();
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimBlk(BSTR DimBlk)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str(DimBlk);
	if (str.IsEmpty ())
		return E_INVALIDARG;

	if ( !((db_dimstyletabrec*)m_pDbHandItem)->set_5 ((char*)str.GetBuffer(0)) ) 
		return E_FAIL;
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimBlk1(BSTR *pDimBlk1)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimBlk1 == NULL)
		return E_POINTER;
	
	char* pStr;
	m_pDbHandItem->get_6(&pStr);
	CString str (pStr);
	*pDimBlk1 = str.AllocSysString();
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimBlk1(BSTR DimBlk1)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str(DimBlk1);
	if (str.IsEmpty ())
		return E_INVALIDARG;

	if ( !m_pDbHandItem->set_6 ((char*)str.GetBuffer(0)) )
		return E_FAIL;
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimBlk2(BSTR *pDimBlk2)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimBlk2 == NULL)
		return E_POINTER;
	
	char* pStr;
	m_pDbHandItem->get_7(&pStr);
	CString str (pStr);
	*pDimBlk2 = str.AllocSysString();
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimBlk2(BSTR DimBlk2)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str(DimBlk2);
	if (str.IsEmpty ())
		return E_INVALIDARG;

	if ( !m_pDbHandItem->set_7 ((char*)str.GetBuffer(0)) )
		return E_FAIL;
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimScale(double *pDimScale)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimScale == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_40(pDimScale)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimScale(double DimScale)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (DimScale < 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_40(DimScale)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimAsz(double *pDimAsz)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimAsz == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_41(pDimAsz)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimAsz(double DimAsz)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (DimAsz < 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_41(DimAsz)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimExo(double *pDimExo)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimExo == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_42(pDimExo)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimExo(double DimExo)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (DimExo < 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_42(DimExo)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimDli(double *pDimDli)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimDli == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_43(pDimDli)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimDli(double DimDli)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (DimDli < 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_43(DimDli)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimExe(double *pDimExe)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimExe == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_44(pDimExe)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimExe(double DimExe)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (DimExe < 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_44(DimExe)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimRnd(double *pDimRnd)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimRnd == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_45(pDimRnd)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimRnd(double DimRnd)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (DimRnd < 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_45(DimRnd)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimDle(double *pDimDle)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimDle == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_46(pDimDle)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimDle(double DimDle)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (DimDle < 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_46(DimDle)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimTp(double *pDimTp)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimTp == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_47(pDimTp)) 
		return E_FAIL;
	
	return NOERROR;
}


STDMETHODIMP CAutoDimensionStyle::put_DimTp(double DimTp)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_47(DimTp)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimTm(double *pDimTm)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimTm == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_48(pDimTm)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimTm(double DimTm)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_48(DimTm)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimTol(int *pDimTol)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimTol == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_71(pDimTol)) 
		return E_FAIL;
	
	return NOERROR;
}


STDMETHODIMP CAutoDimensionStyle::put_DimTol(int DimTol)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_71(DimTol)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimLim(int *pDimLim)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimLim == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_72(pDimLim)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimLim(int DimLim)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_72(DimLim)) 
		return E_FAIL;
		
	return NOERROR;
}


STDMETHODIMP CAutoDimensionStyle::get_DimTih(int *pDimTih)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimTih == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_73(pDimTih)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimTih(int DimTih)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_73(DimTih)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimToh(int *pDimToh)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimToh == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_74(pDimToh)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimToh(int DimToh)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_74(DimToh)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimSe1(int *pDimSe1)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimSe1 == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_75(pDimSe1)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimSe1(int DimSe1)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_75(DimSe1)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimSe2(int *pDimSe2)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimSe2 == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_76(pDimSe2)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimSe2(int DimSe2)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_76(DimSe2)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimTad(int *pDimTad)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimTad == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_77(pDimTad)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimTad(int DimTad)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_77(DimTad)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimZin(int *pDimZin)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimZin == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_78(pDimZin)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimZin(int DimZin)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_78(DimZin)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimTxt(double *pDimTxt)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimTxt == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_140(pDimTxt)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimTxt(double DimTxt)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (DimTxt <= 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_140(DimTxt)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimCen(double *pDimCen)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimCen == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_141(pDimCen)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimCen(double DimCen)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_141(DimCen)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimTsz(double *pDimTsz)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimTsz == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_142(pDimTsz)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimTsz(double DimTsz)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (DimTsz < 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_142(DimTsz)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimAltf(double *pDimAltf)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimAltf == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_143(pDimAltf)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimAltf(double DimAltf)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (DimAltf <= 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_143(DimAltf)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimLfac(double *pDimLfac)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimLfac == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_144(pDimLfac)) 
		return E_FAIL;
	
	return NOERROR;
}
STDMETHODIMP CAutoDimensionStyle::put_DimLfac(double DimLfac)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_144(DimLfac)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimTvp(double *pDimTvp)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimTvp == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_145(pDimTvp)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimTvp(double DimTvp)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_145(DimTvp)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimTfac(double *pDimTfac)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimTfac == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_146(pDimTfac)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimTfac(double DimTfac)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (DimTfac <= 0.0)
		return E_INVALIDARG;

	if (!m_pDbHandItem->set_146(DimTfac)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimGap(double *pDimGap)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimGap == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_147(pDimGap)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimGap(double DimGap)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_147(DimGap)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimAlt(int *pDimAlt)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimAlt == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_170(pDimAlt)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimAlt(int DimAlt)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_170(DimAlt)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimAltd(int *pDimAltd)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimAltd == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_171(pDimAltd)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimAltd(int DimAltd)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_171(DimAltd)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimTofl(int *pDimTofl)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimTofl == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_172(pDimTofl)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimTofl(int DimTofl)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_172(DimTofl)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimSah(int *pDimSah)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimSah == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_173(pDimSah)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimSah(int DimSah)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_173(DimSah)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimTix(int *pDimTix)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimTix == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_174(pDimTix)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimTix(int DimTix)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_174(DimTix)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimSohd(int *pDimSohd)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimSohd == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_175(pDimSohd)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimSohd(int DimSohd)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_175(DimSohd)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimClrd(int *pDimClrd)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimClrd == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_176(pDimClrd)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimClrd(int DimClrd)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_176(DimClrd)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimClre(int *pDimClre)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimClre == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_177(pDimClre)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimClre(int DimClre)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_177(DimClre)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimClrt(int *pDimClrt)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimClrt == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_178(pDimClrt)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimClrt(int DimClrt)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_178(DimClrt)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimUnit(int *pDimUnit)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimUnit == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_270(pDimUnit)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimUnit(int DimUnit)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_270(DimUnit)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimDec(int *pDimDec)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimDec == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_271(pDimDec)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimDec(int DimDec)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_271(DimDec)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimTdec(int *pDimTdec)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimTdec == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_272(pDimTdec)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimTdec(int DimTdec)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_272(DimTdec)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimAltu(int *pDimAltu)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimAltu == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_273(pDimAltu)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimAltu(int DimAltu)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_273(DimAltu)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimAlttd(int *pDimAlttd)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimAlttd == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_274(pDimAlttd)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimAlttd(int DimAlttd)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_274(DimAlttd)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimAunit(int *pDimAunit)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimAunit == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_275(pDimAunit)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimAunit(int DimAunit)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_275(DimAunit)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimJust(int *pDimJust)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimJust == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_280(pDimJust)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimJust(int DimJust)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_280(DimJust)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimSd1(int *pDimSd1)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimSd1 == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_281(pDimSd1)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimSd1(int DimSd1)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_281(DimSd1)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimSd2(int *pDimSd2)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimSd2 == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_282(pDimSd2)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimSd2(int DimSd2)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_282(DimSd2)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimTolj(int *pDimTolj)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimTolj == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_283(pDimTolj)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimTolj(int DimTolj)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_283(DimTolj)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimTzin(int *pDimTzin)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimTzin == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_284(pDimTzin)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimTzin(int DimTzin)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_284(DimTzin)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimAltz(int *pDimAltz)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimAltz == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_285(pDimAltz)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimAltz(int DimAltz)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_285(DimAltz)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimAlttz(int *pDimAlttz)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimAlttz == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_286(pDimAlttz)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimAlttz(int DimAlttz)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_286(DimAlttz)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimFit(int *pDimFit)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimFit == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_287(pDimFit)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimFit(int DimFit)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_287(DimFit)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimUpt(int *pDimUpt)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimUpt == NULL)
		return E_POINTER;
	
	if (!m_pDbHandItem->get_288(pDimUpt)) 
		return E_FAIL;
	
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimUpt(int DimUpt)
{
	RETURN_IF_INVALID (m_pDbHandItem);

	if (!m_pDbHandItem->set_288(DimUpt)) 
		return E_FAIL;
		
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::get_DimTxSty(BSTR *pDimTxSty)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	if (pDimTxSty == NULL)
		return E_POINTER;
	
	char* pStr;
	m_pDbHandItem->get_340name(&pStr);
	CString str (pStr);
	*pDimTxSty = str.AllocSysString();
	return NOERROR;
}

STDMETHODIMP CAutoDimensionStyle::put_DimTxSty(BSTR DimTxSty)
{
	RETURN_IF_INVALID (m_pDbHandItem);
	CString str(DimTxSty);
	if (str.IsEmpty ())
		return E_INVALIDARG;

	if ( !m_pDbHandItem->set_340byname ((char*)str.GetBuffer(0), m_pDoc->m_dbDrawing)) 
		return E_FAIL;
	return NOERROR;
}
