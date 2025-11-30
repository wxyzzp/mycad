/* D:\ITCDEV\PRJ\ICAD\ICADAPC.H
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.1.1.1.18.1 $ $Date: 2004/10/12 18:57:22 $
 * 
 * Abstract
 * 
 * This file bottlenecks inclusion of licensed VBA headers.
 * 
 */ 

#define _ATL_APARTMENT_THREADED

#define APC_FORCE_RELEASE 1
// define your import mode here
#define APC_IMPORT_FULL  
#define APC_NO_OLE_COLOR // to prevent duplicate symbol

#include <atlctl.h>

#include "Configure.h"	//	VBA configuration option

#ifdef BUILD_WITH_VBA	// Defined in CONFIGURE.H

#include <apcMfc.h>
using namespace MSAPC;

#else

#include "IcadApcMfc.h"
using namespace MSAPC;

#endif

