/* INTELLICAD\PRJ\LIB\AUTO\AUTODIMSTYLE.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoDimensionStyle - the Automation DimensionStyle object
 * 
 */ 

#ifndef __AUTODIMSTYLE_H_
#define __AUTODIMSTYLE_H_

#include "AutoTableItem.h"
#include "AutoBaseDefs.h"
#include "dimstyletabrec.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoDimensionStyle
class ATL_NO_VTABLE CAutoDimensionStyle : 
	public CAutoTableItem,
	public CComCoClass<CAutoDimensionStyle, &CLSID_DimensionStyle>,
	public IDispatchImpl<IIcadDimensionStyle, &IID_IIcadDimensionStyle, &LIBID_IntelliCAD>
{
public:
	CAutoDimensionStyle()
	{
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoDimensionStyle)
		COM_INTERFACE_ENTRY(IIcadDimensionStyle)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadDimensionStyle)
		COM_INTERFACE_ENTRY_CHAIN(CAutoTableItem)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);
	
public:
	DEFINE_IICADDBITEM_METHODS 
	DEFINE_IICADTABLEITEM_METHODS

	// IIcadDimensionStyle
	STDMETHOD(get_DimPost)(BSTR *pDimPost);
	STDMETHOD(put_DimPost)(BSTR DimPost);
	STDMETHOD(get_DimApost)(BSTR *pDimApost);
	STDMETHOD(put_DimApost)(BSTR DimApost);
	STDMETHOD(get_DimBlk)(BSTR *pDimBlk);
	STDMETHOD(put_DimBlk)(BSTR DimBlk);
	STDMETHOD(get_DimBlk1)(BSTR *pDimBlk1);
	STDMETHOD(put_DimBlk1)(BSTR DimBlk1);
	STDMETHOD(get_DimBlk2)(BSTR *pDimBlk2);
	STDMETHOD(put_DimBlk2)(BSTR DimBlk2);

	STDMETHOD(get_DimScale)(double *pDimScale);
	STDMETHOD(put_DimScale)(double DimScale);
	STDMETHOD(get_DimAsz)(double *pDimAsz);
	STDMETHOD(put_DimAsz)(double DimAsz);
	STDMETHOD(get_DimExo)(double *pDimExo);
	STDMETHOD(put_DimExo)(double DimExo);
	STDMETHOD(get_DimDli)(double *pDimDli);
	STDMETHOD(put_DimDli)(double DimDli);
	STDMETHOD(get_DimExe)(double *pDimExe);
	STDMETHOD(put_DimExe)(double DimSExe);
	STDMETHOD(get_DimRnd)(double *pDimRnd);
	STDMETHOD(put_DimRnd)(double DimRnd);
	STDMETHOD(get_DimDle)(double *pDimDle);
	STDMETHOD(put_DimDle)(double DimDle);
	STDMETHOD(get_DimTp)(double *pDimTp);
	STDMETHOD(put_DimTp)(double DimTp);
	STDMETHOD(get_DimTm)(double *pDimTm);
	STDMETHOD(put_DimTm)(double DimTm);

	STDMETHOD(get_DimTol)(int *pDimTol);
	STDMETHOD(put_DimTol)(int DimTol);
	STDMETHOD(get_DimLim)(int *pDimLim);
	STDMETHOD(put_DimLim)(int DimLim);
	STDMETHOD(get_DimTih)(int *pDimTih);
	STDMETHOD(put_DimTih)(int DimTih);
	STDMETHOD(get_DimToh)(int *pDimToh);
	STDMETHOD(put_DimToh)(int DimToh);
	STDMETHOD(get_DimSe1)(int *pDimSe1);
	STDMETHOD(put_DimSe1)(int DimSe1);
	STDMETHOD(get_DimSe2)(int *pDimSe2);
	STDMETHOD(put_DimSe2)(int DimSe2);
	STDMETHOD(get_DimTad)(int *pDimTad);
	STDMETHOD(put_DimTad)(int DimTad);
	STDMETHOD(get_DimZin)(int *pDimZin);
	STDMETHOD(put_DimZin)(int DimZin);

	STDMETHOD(get_DimTxt)(double *pDimTxt);
	STDMETHOD(put_DimTxt)(double DimTxt);
	STDMETHOD(get_DimCen)(double *pDimCen);
	STDMETHOD(put_DimCen)(double DimCen);
	STDMETHOD(get_DimTsz)(double *pDimTsz);
	STDMETHOD(put_DimTsz)(double DimTsz);
	STDMETHOD(get_DimAltf)(double *pDimAltf);
	STDMETHOD(put_DimAltf)(double DimAltf);
	STDMETHOD(get_DimLfac)(double *pDimLfac);
	STDMETHOD(put_DimLfac)(double DimLfac);
	STDMETHOD(get_DimTvp)(double *pDimTvp);
	STDMETHOD(put_DimTvp)(double DimTvp);
	STDMETHOD(get_DimTfac)(double *pDimTfac);
	STDMETHOD(put_DimTfac)(double DimTfac);
	STDMETHOD(get_DimGap)(double *pDimGap);
	STDMETHOD(put_DimGap)(double DimGap);

	STDMETHOD(get_DimAlt)(int *pDimAlt);
	STDMETHOD(put_DimAlt)(int DimAlt);
	STDMETHOD(get_DimAltd)(int *pDimAltd);
	STDMETHOD(put_DimAltd)(int DimAltd);
	STDMETHOD(get_DimTofl)(int *pDimTofl);
	STDMETHOD(put_DimTofl)(int DimTofl);
	STDMETHOD(get_DimSah)(int *pDimSah);
	STDMETHOD(put_DimSah)(int DimSah);
	STDMETHOD(get_DimTix)(int *pDimTix);
	STDMETHOD(put_DimTix)(int DimTix);
	STDMETHOD(get_DimSohd)(int *pDimSohd);
	STDMETHOD(put_DimSohd)(int DimSohd);
	STDMETHOD(get_DimClrd)(int *pDimClrd);
	STDMETHOD(put_DimClrd)(int DimClrd);
	STDMETHOD(get_DimClre)(int *pDimClre);
	STDMETHOD(put_DimClre)(int DimClre);
	STDMETHOD(get_DimClrt)(int *pDimClrt);
	STDMETHOD(put_DimClrt)(int DimClrt);
	
	STDMETHOD(get_DimUnit)(int *pDimUnit);
	STDMETHOD(put_DimUnit)(int DimUnit);
	STDMETHOD(get_DimDec)(int *pDimDec);
	STDMETHOD(put_DimDec)(int DimDec);
	STDMETHOD(get_DimTdec)(int *pDimTdec);
	STDMETHOD(put_DimTdec)(int DimTdec);
	STDMETHOD(get_DimAltu)(int *pDimAltu);
	STDMETHOD(put_DimAltu)(int DimAltu);
	STDMETHOD(get_DimAlttd)(int *pDimAlttd);
	STDMETHOD(put_DimAlttd)(int DimAlttd);
	STDMETHOD(get_DimAunit)(int *pDimAunit);
	STDMETHOD(put_DimAunit)(int DimAunit);

	STDMETHOD(get_DimJust)(int *pDimJust);
	STDMETHOD(put_DimJust)(int DimJust);
	STDMETHOD(get_DimSd1)(int *pDimSd1);
	STDMETHOD(put_DimSd1)(int DimSd1);
	STDMETHOD(get_DimSd2)(int *pDimSd2);
	STDMETHOD(put_DimSd2)(int DimSd2);
	STDMETHOD(get_DimTolj)(int *pDimTolj);
	STDMETHOD(put_DimTolj)(int DimTolj);
	STDMETHOD(get_DimTzin)(int *pDimTzin);
	STDMETHOD(put_DimTzin)(int DimTzin);
	STDMETHOD(get_DimAltz)(int *pDimAltz);
	STDMETHOD(put_DimAltz)(int DimAltz);
	STDMETHOD(get_DimAlttz)(int *pDimAlttz);
	STDMETHOD(put_DimAlttz)(int DimAlttz);
	STDMETHOD(get_DimFit)(int *pDimFit);
	STDMETHOD(put_DimFit)(int DimFit);
	STDMETHOD(get_DimUpt)(int *pDimUpt);
	STDMETHOD(put_DimUpt)(int DimUpt);

	STDMETHOD(get_DimTxSty)(BSTR *pDimTxSty);
	STDMETHOD(put_DimTxSty)(BSTR DimTxSty);
};

#endif //__AUTODIMSTYLE_H_


