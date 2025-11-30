// File: <DevDir>\sources\prj\lib\auto\AutoDimAligned.cpp
//

#include "stdafx.h"
#include "AutoDimAligned.h"
#include "AutoPoint.h"


HRESULT
CAutoDimAligned::Init(CIcadDoc* pDoc, IDispatch* pParent, db_handitem* pDbHandItem)
{
	HRESULT	hr = CAutoDimension::Init(pDoc, pParent, pDbHandItem);

	if(SUCCEEDED(hr))
		m_pDbHandItem->set_70(1);

	return hr;
}


STDMETHODIMP
CAutoDimAligned::get_ExtLine1Point(IIcadPoint** Point)
{
	RETURN_IF_INVALID((m_pDbHandItem && Point))
	
	sds_point	pt;
	m_pDbHandItem->get_13(pt);
	
	CComObject<CAutoPoint>*	pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if(!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, pt[0], pt[1], pt[2]);
	
	return pPoint->QueryInterface(Point);
}


STDMETHODIMP
CAutoDimAligned::put_ExtLine1Point(IIcadPoint* Point)
{
	RETURN_IF_INVALID(m_pDbHandItem)

	sds_point	pt = {0.0, 0.0, 0.0};
	if(Point)	//this is an optional arg
	{
		Point->get_x(&pt[0]);
		Point->get_y(&pt[1]);
		Point->get_z(&pt[2]);
	}
	m_pDbHandItem->set_13(pt);
	
	return S_OK;
}


STDMETHODIMP
CAutoDimAligned::get_ExtLine2Point(IIcadPoint** Point)
{
	RETURN_IF_INVALID((m_pDbHandItem && Point))
	
	sds_point	pt;
	m_pDbHandItem->get_14(pt);
	
	CComObject<CAutoPoint>*	pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if(!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, pt[0], pt[1], pt[2]);
	
	return pPoint->QueryInterface(Point);
}


STDMETHODIMP
CAutoDimAligned::put_ExtLine2Point(IIcadPoint* Point)
{
	RETURN_IF_INVALID(m_pDbHandItem)

	sds_point	pt = {0.0, 0.0, 0.0};
	if(Point)	//this is an optional arg
	{
		Point->get_x(&pt[0]);
		Point->get_y(&pt[1]);
		Point->get_z(&pt[2]);
	}
	m_pDbHandItem->set_14(pt);
	
	return S_OK;
}
