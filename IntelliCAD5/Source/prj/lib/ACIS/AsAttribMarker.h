#pragma once
#ifndef _ASATTRIBMARKER_H_
#define _ASATTRIBMARKER_H_

#include "AsAttrib.h"

extern int CAsAttribMarker_TYPE;

#define CAsAttribMarker_LEVEL (CAsAttrib_LEVEL + 1)

class CAsAttribMarker : public CAsAttrib
{
    // No member data. The presence/absence of this attribute indicates the entity as marked/unmarked.
public:
    CAsAttribMarker(ENTITY* pOwner = 0)
		:CAsAttrib(pOwner) 
	{};

	// The new entity is marked
    virtual void split_owner
	(
	ENTITY *pNew
	);
	// The merged entity is marked
	virtual void  merge_owner (ENTITY *pOther, logical bDeleteOwner);

	virtual void copy_owner
	(
	ENTITY *pNew
	) 
	{
		split_owner (pNew);
	}
    
	ATTRIB_FUNCTIONS (CAsAttribMarker, NONE)

    // Implementation of the CffAttribute interface
    void detach() 
	{
		unhook(); 
		lose();
	}
};

#endif
