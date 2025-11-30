// File: <DevDir>\sources\prj\lib\auto\AutoDimDiametric.cpp
//

#include "stdafx.h"
#include "AutoDimDiametric.h"


HRESULT
CAutoDimDiametric::Init(CIcadDoc* pDoc, IDispatch* pParent, db_handitem* pDbHandItem)
{
	HRESULT	hr = CAutoDimension::Init(pDoc, pParent, pDbHandItem);

	if(SUCCEEDED(hr))
		m_pDbHandItem->set_70(3);

	return hr;
}
