/* 
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * functions to set up various coordinate systems
 * 
 */ 

#include "gr.h"
#include "gr_view.h"


short gr_setupixfp(gr_view *viewp) {
/*
**  Sets viewp->ixfp[] based on viewp->xfp[].
**
**  xfp[0-3] specify the NCS as viewed by the UCS.
**
**  The ixfp[] elements do the inverse: they specify the UCS system as
**  viewed by the NCS system:
**
**      ixfp[0] : UCS origin      according to the NCS.
**      ixfp[1] : UCS unit X-axis according to the NCS.
**      ixfp[2] : UCS unit Y-axis according to the NCS.
**      ixfp[3] : UCS unit Z-axis according to the NCS.
**
**  The vectors are not necessarily unit vectors --
**  they are the UCS unit vectors TRANSFORMED TO THE NCS.
**
**  With xfp[], gr_ncs2ucs() can transform an NCS pt (or vector)
**  from the NCS to the UCS by treating the pt's coordinates as
**  multipliers of the axis vectors and then performing the
**  vector sum.  ixfp[] can be used in the same manner to transform
**  from UCS to NCS (in gr_ucs2ncs()).
**
**  The equations used in this function to generate ixfp[] from
**  xfp[] come from solving the gr_ncs2ucs() equations for the
**  NCS coordinates, and then using those equations on the UCS origin
**  and axes to see what the UCS looks like to the NCS.
**
**  Returns:
**       0 : OK
**      -1 : At least 1 of the xfp[] axis vectors is the 0-vector,
**             or they are all 3 coplanar; default ixfp[] generated.
*/
	viewp->m_transformationInverse = viewp->m_transformation.invert();
	return 0;
}


