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
// programs incorporating this software must include the following statement 
// with their copyright notices:
//
//      DWGdirect © 2002 by Open Design Alliance Inc. All rights reserved. 
//
// By use of this software, you acknowledge and accept the terms of this 
// agreement.
///////////////////////////////////////////////////////////////////////////////



#ifndef _ODDBDATE_INCLUDED_
#define _ODDBDATE_INCLUDED_

#include "DD_PackPush.h"

#include "OdTimeStamp.h"

class OdDbDwgFiler;

/** Description:
    Represents date and time information within an OdDbDatabase.

    {group:OdDb_Classes}
*/
class TOOLKIT_EXPORT OdDbDate : public OdTimeStamp
{
public:
	/** Constructor.
  */
  OdDbDate();

  OdDbDate(const OdTimeStamp& odtime);

	/** Constructor.
	    @param i Initial value, one of:
	    - kInitZero - Initial value is 0.
	    - kInitLocalTime - Initial value is set to local time.
	    - kInitUniversalTime - Initial value is set to Universal or Greenwich Mean Time.
  */
  OdDbDate(InitialValue i);

  OdDbDate operator=(const OdTimeStamp& odtime);


  /** Description:
      Reads in the DWG data for this object.

      Arguments:
      inFiler Filer object from which data is read.
  */
  void dwgIn(OdDbDwgFiler* inFiler);

  /** Description:
      Writes out the DWG data for this object.

      Arguments:
      outFiler Filer object to which data is written.
  */
  void dwgOut(OdDbDwgFiler* outFiler) const;
};

#include "DD_PackPop.h"

#endif // _ODDBDATE_INCLUDED_


