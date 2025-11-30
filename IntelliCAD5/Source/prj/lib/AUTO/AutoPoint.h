/* INTELLICAD\PRJ\LIB\AUTO\AUTOPOINT.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Declaration of the CAutoPoint- the Automation Point object
 * 
 */ 

#ifndef __AUTOPOINT_H_
#define __AUTOPOINT_H_

#include "AutoLibrary.h"
#include "AutoPoints.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoPoint
class AUTO_DECLSPEC ATL_NO_VTABLE CAutoPoint : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAutoPoint, &CLSID_Point>,
	public IDispatchImpl<IIcadPoint, &IID_IIcadPoint, &LIBID_IntelliCAD>
{
private:
	CComPtr<IIcadLibrary> m_pLib;
	IIcadPoints *m_pPoints;
	double m_dX;
	double m_dY;
	double m_dZ;

public:
	CAutoPoint()
		{
		m_pLib = NULL;
		m_pPoints = NULL;
		}
	~CAutoPoint()
		{
		if (m_pLib)
			m_pLib = NULL;
		if (m_pPoints)
			m_pPoints = NULL;
		}

	DECLARE_NO_REGISTRY()
		//we use a dummy registry entry, because we do not want the object to be in the registry, but we do want it
		//to be registered with ARL's CComModule - Point, Points, Vector and Matrix are defined in IcadApp.cpp - the main _Module

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CAutoPoint)
		COM_INTERFACE_ENTRY(IIcadPoint)
		COM_INTERFACE_ENTRY2(IDispatch, IIcadPoint)
	END_COM_MAP()

	HRESULT Init (IIcadLibrary *pLib, IIcadPoints *pPoints, double x, double y, double z);

public:
	// IIcadPoint
	STDMETHOD(get_Application)(IIcadApplication * * ppApp);
	STDMETHOD(get_Parent)(IIcadLibrary * * ppParent);
	STDMETHOD(get_PointsCollection)(IIcadPoints * * ppPoints);
	STDMETHOD(get_x)(double * pX);
	STDMETHOD(put_x)(double pX);
	STDMETHOD(get_y)(double * pY);
	STDMETHOD(put_y)(double pY);
	STDMETHOD(get_z)(double * pZ);
	STDMETHOD(put_z)(double pZ);
};

#endif //__AUTOPOINT_H_


