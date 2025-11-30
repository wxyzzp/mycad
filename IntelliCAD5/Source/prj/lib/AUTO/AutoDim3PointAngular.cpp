// File: <DevDir>\sources\prj\lib\auto\AutoDim3PointAngular.cpp
//

#include "stdafx.h"
#include "AutoDim3PointAngular.h"
#include "AutoPoint.h"


HRESULT
CAutoDim3PointAngular::Init(CIcadDoc* pDoc, IDispatch* pParent, db_handitem* pDbHandItem)
{
	HRESULT	hr = CAutoDimension::Init(pDoc, pParent, pDbHandItem);

	if(SUCCEEDED(hr))
		m_pDbHandItem->set_70(5);

	return hr;
}


STDMETHODIMP
CAutoDim3PointAngular::get_AngleVertex(IIcadPoint** Point)
{
	RETURN_IF_INVALID((m_pDbHandItem && Point))
	
	sds_point	pt;
	m_pDbHandItem->get_15(pt);
	
	CComObject<CAutoPoint>*	pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if(!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, pt[0], pt[1], pt[2]);
	
	return pPoint->QueryInterface(Point);
}


STDMETHODIMP
CAutoDim3PointAngular::put_AngleVertex(IIcadPoint* Point)
{
	RETURN_IF_INVALID(m_pDbHandItem)

	sds_point	pt = {0.0, 0.0, 0.0};
	if(Point)	//this is an optional arg
	{
		Point->get_x(&pt[0]);
		Point->get_y(&pt[1]);
		Point->get_z(&pt[2]);
	}
	m_pDbHandItem->set_15(pt);
	
	return S_OK;
}


STDMETHODIMP
CAutoDim3PointAngular::get_ExtLine1StartPoint(IIcadPoint** Point)
{	/*DG - 26.12.2001*/// This method must be removed from IIcadDim3PointAngular interface.
	return E_FAIL;
}


STDMETHODIMP
CAutoDim3PointAngular::put_ExtLine1StartPoint(IIcadPoint* Point)
{
	return E_FAIL;
}


STDMETHODIMP
CAutoDim3PointAngular::get_ExtLine2StartPoint(IIcadPoint** Point)
{
	return E_FAIL;
}


STDMETHODIMP
CAutoDim3PointAngular::put_ExtLine2StartPoint(IIcadPoint* Point)
{
	return E_FAIL;
}
