// \source\prj\lib\gr\polycache.h

#ifndef _POLYCACHE_H
#define _POLYCACHE_H

#include "geometry.h"
#include "point.h"
#include "theuser.h"
#include "array.h"
#include "ptarray.h"

class CPolyCache : public CCache, public CGeoObjectArray
{
public:
// Constructor/destructor
	CPolyCache() : m_pCurrentPoly(NULL)
	{
	}

	virtual ~CPolyCache()
	{
	}
	
// Inline methods
	BOOL Started()
	{
		return m_pCurrentPoly != NULL && m_pCurrentPoly->Size() > 0;
	}

	RC Accept(
			  const C3Point & P,	// => The point to add
			  double t)				// => Parameter value (ignored here)
	{
		RC rc = m_pCurrentPoly->Add(P);
		RETURN_RC(CPolyCache::Accept, rc);
	}

	PPointArray Poly(
					 int i)			// => Index of the desired polygon
	{
		return (PPointArray)m_pEntries[i];
	}

// Methods
	// start a new polygon
	RC StartNewPoly();

	// adjust any polygon in the cache so that it is exactly closed if the
	// distance from the first point to the last point is less than tol
	void MakeClosed(double tol);

	/*D.G.*/// Free memory.
	void Clear()
	{
		Purge();
		m_pCurrentPoly = NULL;
	}

// Data
protected:
	PPointArray		m_pCurrentPoly;		// The current polygon
};

#endif // _POLYCACHE_H
