///////////////////////////////////////////////////////////////////////////////
// Copyright © 2002, Open Design Alliance Inc. ("Open Design") 
// 
// This software is owned by Open Design, and may only be incorporated into 
// application programs owned by members of Open Design subject to a signed 
// Membership Agreement and Supplemental Software License Agreement with 
// Open Design. The structure and organization of this Software are the valuable 
// trade secrets of Open Design and its suppliers. The Software is also protected 
// by copyright law and international treaty provisions. You agree not to 
// modify, adapt, translate, reverse engineer, decompile, disassemble or 
// otherwise attempt to discover the source code of the Software. Application 
// programs incorporating this software must include the following statment 
// with their copyright notices:
//
//      DWGdirect © 2002 by Open Design Alliance Inc. All rights reserved. 
//
// By use of this software, you acknowledge and accept the terms of this 
// agreement.
///////////////////////////////////////////////////////////////////////////////



#ifndef OD_GE_LIB_VERSION
#define OD_GE_LIB_VERSION

#include "DD_PackPush.h"

#define IMAGE_MAJOR_VER          2
#define IMAGE_MINOR_VER          0
#define IMAGE_CORRECTIVE_VER     0
#define IMAGE_INTERNAL_VER       0

/** Description:
    
    {group:OdGe_Classes} 
*/
class GE_TOOLKIT_EXPORT OdGeLibVersion
{
public:
    // Constructor.
    OdGeLibVersion();
    
		// Constructor.
		OdGeLibVersion(const OdGeLibVersion&);

		// Constructor.
    OdGeLibVersion(unsigned char major, unsigned char minor,
                   unsigned char corrective,
                   unsigned char internal_version);

    unsigned char     majorVersion        () const; 
    unsigned char     minorVersion        () const; 
    unsigned char     correctiveVersion   () const; 
    unsigned char     schemaVersion       () const;

    OdGeLibVersion&     setMajorVersion        (unsigned char val); 
    OdGeLibVersion&     setMinorVersion        (unsigned char val); 
    OdGeLibVersion&     setCorrectiveVersion   (unsigned char val); 
    OdGeLibVersion&     setSchemaVersion       (unsigned char val);

    bool operator ==      (const OdGeLibVersion&) const; 
    bool operator !=      (const OdGeLibVersion&) const; 
    bool operator <       (const OdGeLibVersion&) const; 
    bool operator <=      (const OdGeLibVersion&) const; 
    bool operator >       (const OdGeLibVersion&) const; 
    bool operator >=      (const OdGeLibVersion&) const;

    static const OdGeLibVersion kRelease0_95;

    static const OdGeLibVersion kReleaseSed;

    static const OdGeLibVersion kReleaseTah;

private:
    unsigned char   mVersion[10];
};

#include "DD_PackPop.h"

#endif // OD_GE_LIB_VERSION


