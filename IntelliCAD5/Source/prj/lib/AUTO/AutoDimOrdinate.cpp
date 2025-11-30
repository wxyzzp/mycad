// File: <DevDir>\sources\prj\lib\auto\AutoDimOrdinate.cpp
//

#include "stdafx.h"
#include "AutoDimOrdinate.h"


HRESULT
CAutoDimOrdinate::Init(CIcadDoc* pDoc, IDispatch* pParent, db_handitem* pDbHandItem)
{
	HRESULT	hr = CAutoDimension::Init(pDoc, pParent, pDbHandItem);

	if(SUCCEEDED(hr))
		m_pDbHandItem->set_70(6);

	return hr;
}
