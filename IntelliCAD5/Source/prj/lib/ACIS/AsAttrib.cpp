#include "stdafx.h"
#include "AsAttrib.h"

// Define macros for this attribute and its parent, to provide the
// information to the definition macro.
#undef THIS
#define THIS()      CAsAttrib
#define THIS_LIB    NONE
#define PARENT()    ATTRIB
#define PARENT_LIB  KERN

// Identifier used externally to identify an particular entity type.
// This is only used within the save/restore system for translating
// to/from external file format, but must be unique amongst attributes
// derived directly from ATTRIB, across all application developers.
#define CAsAttrib_NAME  "IntelliCADTechnologyConsorcium"

MASTER_ATTRIB_DEFN ("ITC master attribute");
