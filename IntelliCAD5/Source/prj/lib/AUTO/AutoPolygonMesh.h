/* INTELLICAD\PRJ\LIB\AUTO\AUTOPOLYGONMESH.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.3.10.1 $ $Date: 2003/08/29 01:34:02 $ 
 * 
 * Abstract
 * 
 * Declaration of CAutoPolyline - the Automation Polyline - Polygon mesh object
 * 
 */ 

#ifndef __AUTOPOLYGONMESH_H_
#define __AUTOPOLYGONMESH_H_

#include "AutoEntity.h"
#include "AutoBaseDefs.h"
#include "AutoPoints.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoPolygonMesh
class ATL_NO_VTABLE CAutoPolygonMesh : 
	public CAutoEntity,
	public CComCoClass<CAutoPolygonMesh, &CLSID_PolygonMesh>,
	public IDispatchImpl<IIcadPolygonMesh, &IID_IIcadPolygonMesh, &LIBID_IntelliCAD>
{
private:
	int m_nVertices;
	CComPtr<IIcadPoints> m_Vertices;

public:
	//Bugzilla::78559; 07-07-2003 [
	CAutoPolygonMesh()
		{
			m_nVertices = 0;
			m_Vertices = NULL;
		}
	~CAutoPolygonMesh()
		{
		m_nVertices = 0;
		if (m_Vertices)
			m_Vertices = NULL;
		}
	//Bugzilla::78559; 07-07-2003 ]

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoPolygonMesh)
		COM_INTERFACE_ENTRY(IIcadPolygonMesh)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadPolygonMesh)
		COM_INTERFACE_ENTRY_CHAIN(CAutoEntity)
	END_COM_MAP()

	virtual HRESULT Init (CIcadDoc *pDoc, IDispatch *pParent, db_handitem *pDbHandItem);
	HRESULT UpdatePolylineData ();

public:
	DEFINE_IICADENTITY_METHODS
	DEFINE_IICADDBITEM_METHODS 

	// IIcadPolygonMesh
	STDMETHOD(get_Coordinates)(IIcadPoints * * ppCoords);
	STDMETHOD(put_Coordinates)(IIcadPoints * ppCoords);
	STDMETHOD(get_MClose)(VARIANT_BOOL * pClosed);
	STDMETHOD(put_MClose)(VARIANT_BOOL pClosed);
	STDMETHOD(get_MDensity)(long * pDensity);
	STDMETHOD(put_MDensity)(long pDensity);
	STDMETHOD(get_MVertexCount)(long * pCount);
	STDMETHOD(get_NClose)(VARIANT_BOOL * pClosed);
	STDMETHOD(put_NClose)(VARIANT_BOOL pClosed);
	STDMETHOD(get_NDensity)(long * pDensity);
	STDMETHOD(put_NDensity)(long pDensity);
	STDMETHOD(get_NVertexCount)(long * pCount);
	STDMETHOD(get_Type)(PolygonMeshType * pType);
	STDMETHOD(put_Type)(PolygonMeshType pType);
	STDMETHOD(AppendVertex)(IIcadPoints * Vertices);
	STDMETHOD(Explode)(IIcadSelectionSet * * ppObjects);
	
};

#endif //__AUTOPOLYGONMESH_H_


