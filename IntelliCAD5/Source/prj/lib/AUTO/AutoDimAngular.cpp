// File: <DevDir>\sources\prj\lib\auto\AutoDimAngular.cpp
//

#include "stdafx.h"
#include "AutoDimAngular.h"
#include "AutoPoint.h"


HRESULT
CAutoDimAngular::Init(CIcadDoc* pDoc, IDispatch* pParent, db_handitem* pDbHandItem)
{
	HRESULT	hr = CAutoDimension::Init(pDoc, pParent, pDbHandItem);

	if(SUCCEEDED(hr))
		m_pDbHandItem->set_70(2);

	return hr;
}


HRESULT
CAutoDimAngular::get_ExtLine1StartPoint(IIcadPoint** Point)
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


HRESULT
CAutoDimAngular::put_ExtLine1StartPoint(IIcadPoint* Point)
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


HRESULT
CAutoDimAngular::get_ExtLine2StartPoint(IIcadPoint** Point)
{
	RETURN_IF_INVALID((m_pDbHandItem && Point))
	
	sds_point	pt;
	m_pDbHandItem->get_10(pt);
	
	CComObject<CAutoPoint>*	pPoint;
	CComObject<CAutoPoint>::CreateInstance(&pPoint);
	if(!pPoint)
		return E_FAIL;
	
	pPoint->Init(NULL, NULL, pt[0], pt[1], pt[2]);
	
	return pPoint->QueryInterface(Point);
}


HRESULT
CAutoDimAngular::put_ExtLine2StartPoint(IIcadPoint* Point)
{
	RETURN_IF_INVALID(m_pDbHandItem)

	sds_point	pt = {0.0, 0.0, 0.0};
	if(Point)	//this is an optional arg
	{
		Point->get_x(&pt[0]);
		Point->get_y(&pt[1]);
		Point->get_z(&pt[2]);
	}
	m_pDbHandItem->set_10(pt);
	
	return S_OK;
}
