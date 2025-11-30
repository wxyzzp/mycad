/* INTELLICAD\PRJ\LIB\AUTO\AUTOPOLYFACEMESH.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.3 $ $Date: 2001/03/16 21:48:12 $ 
 * 
 * Abstract
 * 
 * Declaration of CAutoPolyfaceMesh - the Automation Polyline - Polyface mesh object
 * 
 */ 

#ifndef __AUTOPOLYFACEMESH_H_
#define __AUTOPOLYFACEMESH_H_

#include "AutoEntity.h"
#include "AutoBaseDefs.h"
#include "AutoPoints.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoPolyfaceMesh
class ATL_NO_VTABLE CAutoPolyfaceMesh : 
	public CAutoEntity,
	public CComCoClass<CAutoPolyfaceMesh, &CLSID_PolyfaceMesh>,
	public IDispatchImpl<IIcadPolyfaceMesh, &IID_IIcadPolyfaceMesh, &LIBID_IntelliCAD>
{
	int m_nVertices;
	CComPtr<IIcadPoints> m_Vertices;

public:
	CAutoPolyfaceMesh()
		{
		m_nVertices = 0;
		m_Vertices = NULL;
		}
	~CAutoPolyfaceMesh()
		{
		m_nVertices = 0;
		if (m_Vertices)
			m_Vertices = NULL;
		}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoPolyfaceMesh)
		COM_INTERFACE_ENTRY(IIcadPolyfaceMesh)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadPolyfaceMesh)
		COM_INTERFACE_ENTRY_CHAIN(CAutoEntity)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);
	HRESULT UpdatePolylineData ();
	HRESULT PutFaces (VARIANT Faces);

public:
	DEFINE_IICADENTITY_METHODS
	DEFINE_IICADDBITEM_METHODS 

	// IIcadPolyfaceMesh
	STDMETHOD(get_Coordinates)(IIcadPoints * * ppCoords);
	STDMETHOD(put_Coordinates)(IIcadPoints * ppCoords);
	STDMETHOD(get_NumberOfVertices)(long * pCount);
	STDMETHOD(get_NumberOfFaces)(long * pCount);
	
};

#endif //__AUTOPOLYFACEMESH_H_


