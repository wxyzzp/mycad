/* INTELLICAD\PRJ\LIB\AUTO\AUTO3DPOLY.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.3 $ $Date: 2001/03/16 21:48:11 $ 
 * 
 * Abstract
 * 
 * Declaration of CAuto3DPolyline - the Automation 3D Polyline object
 * 
 */ 

#ifndef __AUTO3DPOLYLINE_H_
#define __AUTO3DPOLYLINE_H_

#include "AutoEntity.h"
#include "AutoBaseDefs.h"
#include "AutoPoints.h"

/////////////////////////////////////////////////////////////////////////////
// CAuto3DPolyline
class ATL_NO_VTABLE CAuto3DPolyline : 
	public CAutoEntity,
	public CComCoClass<CAuto3DPolyline, &CLSID_Polyline3D>,
	public IDispatchImpl<IIcad3DPoly, &IID_IIcad3DPoly, &LIBID_IntelliCAD>
{
private:
	int m_nVertices;
	CComPtr<IIcadPoints> m_Vertices;

public:
	CAuto3DPolyline()
		{
		m_nVertices = 0;
		m_Vertices = NULL;
		}
	~CAuto3DPolyline()
		{
		m_nVertices = 0;
		if (m_Vertices)
			m_Vertices = NULL;
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAuto3DPolyline)
		COM_INTERFACE_ENTRY(IIcad3DPoly)
		COM_INTERFACE_ENTRY2(IDispatch, IIcad3DPoly)
		COM_INTERFACE_ENTRY_CHAIN(CAutoEntity)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);
	HRESULT UpdatePolylineData ();

public:
	DEFINE_IICADENTITY_METHODS
	DEFINE_IICADDBITEM_METHODS 

	// IIcad3DPoly
	STDMETHOD(get_Closed)(VARIANT_BOOL * pClosed);
	STDMETHOD(put_Closed)(VARIANT_BOOL pClosed);
	STDMETHOD(get_Coordinates)(IIcadPoints * * ppCoords);
	STDMETHOD(put_Coordinates)(IIcadPoints * ppCoords);
	STDMETHOD(get_Length)(double * pLength);
	STDMETHOD(AppendVertex)(IIcadPoint * Vertex);
	STDMETHOD(Explode)(IIcadSelectionSet * * ppArrayOfEntities);
	
};

#endif //__AUTO3DPOLYLINE_H_


