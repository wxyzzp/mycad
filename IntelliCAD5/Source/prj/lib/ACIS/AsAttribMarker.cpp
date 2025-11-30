#include "stdafx.h"
#include "AsAttribMarker.h"

// Define macros for this attribute and its parent, to provide the
// information to the definition macro.
#undef THIS
#define THIS()      CAsAttribMarker
#define THIS_LIB    NONE
#define PARENT()    CAsAttrib
#define PARENT_LIB  NONE

// Identifier used externally to identify an particular entity type.
// This is only used within the save/restore system for translating
// to/from external file format, but must be unique amongst attributes
// derived directly from ATTRIB, across all application developers.
#define CAsAttribMarker_NAME  "Marker"

// See at_macro.hxx for the meaning of these macros
ATTCOPY_DEF ("ITC marker attribute")

LOSE_DEF

DTOR_DEF

DEBUG_DEF

SAVE_DEF

RESTORE_DEF

COPY_DEF

SCAN_DEF

FIX_POINTER_DEF

TERMINATE_DEF

// The new entity is marked
void  CAsAttribMarker::split_owner
(
ENTITY* pNew
)
{
    ACIS_NEW CAsAttribMarker(pNew);
}

// The merged entity is always marked
void  CAsAttribMarker::merge_owner
(
ENTITY* pOther,
logical bDeleteOwner
)
{
	// If the owner of this attribute is going to be deleted, and
    // there is no marker attribute attached to the other entity, then we
    // transfer ourself to that other entity.
    if (bDeleteOwner && 0 == find_attrib(pOther, CAsAttrib_TYPE, CAsAttribMarker_TYPE))
        move(pOther);
}
