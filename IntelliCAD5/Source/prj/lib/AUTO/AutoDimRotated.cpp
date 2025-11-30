// File: <DevDir>\sources\prj\lib\auto\AutoDimRotated.cpp
//

#include "stdafx.h"
#include "AutoDimRotated.h"


HRESULT
CAutoDimRotated::Init(CIcadDoc* pDoc, IDispatch* pParent, db_handitem* pDbHandItem)
{
	return CAutoDimension::Init(pDoc, pParent, pDbHandItem);
}
