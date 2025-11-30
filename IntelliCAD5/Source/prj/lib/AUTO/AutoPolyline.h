/* INTELLICAD\PRJ\LIB\AUTO\AUTOPOLYLINE.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of CAutoPolyline - the Automation Polyline object
 * 
 */ 

#ifndef __AUTOPOLYLINE_H_
#define __AUTOPOLYLINE_H_

#include "AutoEntity.h"
#include "AutoBaseDefs.h"
#include "AutoPoints.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoPolyline
class ATL_NO_VTABLE CAutoPolyline : 
	public CAutoEntity,
	public CComCoClass<CAutoPolyline, &CLSID_Polyline>,
	public IDispatchImpl<IIcadPolyline, &IID_IIcadPolyline, &LIBID_IntelliCAD>
{
private:
	int m_nVertices;
	CComPtr<IIcadPoints> m_Vertices;
	double *m_StartWidth;
	double *m_EndWidth;
	double *m_Bulge;

public:
	CAutoPolyline()
		{
		m_nVertices = 0;
		m_Vertices = NULL;
		m_StartWidth = NULL;
		m_EndWidth = NULL;
		m_Bulge = NULL;
		}
	~CAutoPolyline()
		{
		m_nVertices = 0;
		if (m_Vertices)
			m_Vertices = NULL;
		if (m_StartWidth)
			delete [] m_StartWidth;
		if (m_EndWidth)
			delete [] m_EndWidth;
		if (m_Bulge)
			delete [] m_Bulge;
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoPolyline)
		COM_INTERFACE_ENTRY(IIcadPolyline)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadPolyline)
		COM_INTERFACE_ENTRY_CHAIN(CAutoEntity)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);
	HRESULT UpdatePolylineData ();


public:
	DEFINE_IICADENTITY_METHODS
	DEFINE_IICADDBITEM_METHODS 

	// IIcadPolyline
	STDMETHOD(get_Area)(double * pArea);
	STDMETHOD(get_Length)(double * pLength);
	STDMETHOD(get_Closed)(VARIANT_BOOL * pClosed);
	STDMETHOD(put_Closed)(VARIANT_BOOL pClosed);
	STDMETHOD(get_Coordinates)(IIcadPoints * * ppCoords);
	STDMETHOD(put_Coordinates)(IIcadPoints * ppCoords);
	STDMETHOD(get_Normal)(IIcadVector * * ppExtrusionDirection);
	STDMETHOD(put_Normal)(IIcadVector * ExtrusionDirection);
	STDMETHOD(get_Thickness)(double * pThick);
	STDMETHOD(put_Thickness)(double pThick);
	STDMETHOD(get_Type)(PolylineType * pType);
	STDMETHOD(put_Type)(PolylineType pType);
	STDMETHOD(AppendVertex)(IIcadPoint * Vertex);
	STDMETHOD(Explode)(IIcadSelectionSet * * ppArrayOfEntities);
	STDMETHOD(GetBulge)(int Index, double * pBulge);
	STDMETHOD(GetWidth)(int Index, double * StartWidth, double * EndWidth);
	STDMETHOD(Offset)(double Offset, IIcadSelectionSet * * ppArrayOfEntities);
	STDMETHOD(SetBulge)(int Index, double Bulge);
	STDMETHOD(SetWidth)(int SegmentIndex, double StartWidth, double EndWidth);
};

#endif //__AUTOPOLYLINE_H_


