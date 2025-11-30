// \source\prj\lib\gr\polycache.cpp

#include "polycache.h"

RC CPolyCache::StartNewPoly()
{
	RC rc = ALLOC_FAIL(m_pCurrentPoly = new CPointArray);
	if(rc)
		QUIT

	if(rc = Add(m_pCurrentPoly))
		delete m_pCurrentPoly;
exit:
	RETURN_RC(CPolyCache::StartNewPoly, rc);
}

void CPolyCache::MakeClosed(double tol)
{
	if(m_pCurrentPoly)
	{  
		int	iLast = m_pCurrentPoly->Size() - 1;
		if(iLast < 1)	/*D.G.*/
			return;
		if(PointDistance((*m_pCurrentPoly)[0], (*m_pCurrentPoly)[iLast]) < tol)
			// force the last point to exactly equal the first point
			(*m_pCurrentPoly)[iLast] = (*m_pCurrentPoly)[0];
	}  
}
