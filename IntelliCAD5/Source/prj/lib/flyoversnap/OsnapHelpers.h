// \PRJ\ICAD\OSNAPHELPERS.H

#include "hiparray.h"

//BugZilla No. 78333 ; 03-03-2003
#define OSNAP_MAX_NUMBER 75
//#define OSNAP_MAX_NUMBER 25

// Helper functions for COsnapManagerImp class (should be replaced with the appropriate
// methods of COsnapManagerImp class in future implementation
// Current implementation of these helper functions (and functions they call) are
// borrowed from sds_osnap.cpp file

int osnap_helper_processSS( int osmode, sds_point inPtrp, sds_point ptAp1rp, sds_point ptAp2rp, sds_name ss );
int osnap_helper_preprocessSS( sds_point ptAp1rp, sds_point ptAp2rp, sds_name ss, CHipArray& hipArray );


