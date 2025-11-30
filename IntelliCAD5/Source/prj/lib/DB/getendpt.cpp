/* D:\ICADDEV\PRJ\LIB\DB\GETENDPT.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.1.1.1.12.1 $ $Date: 2004/05/08 14:47:10 $ 
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 


// THIS FILE HAS BEEN GLOBALIZED!

#include "db.h"
#include "icadlib.h"/*DNT*/
#include "EntConversion.h"/*DNT*/
#include "nurbs.h"

#include "curve.h"

short db_getendpts(sds_name ename, sds_point pt[2])
	{

    sds_real fr2;
    sds_point cp;

    if (IC_TYPE_FROM_ENAME(ename)==DB_LINE)
		{
		db_line *theline=(db_line *)ename[0];
        theline->get_10(pt[0]);
        theline->get_11(pt[1]);
		return 0;
		}
    else if (IC_TYPE_FROM_ENAME(ename)==DB_ARC) 
		{
		db_arc *thearc=(db_arc *)ename[0];
		sds_real stang, endang;
		thearc->get_40(&fr2);
		thearc->get_10(cp);
		thearc->get_50(&stang);
		ic_polar(cp,stang,fr2,pt[0]);
		thearc->get_51(&endang);
		ic_polar(cp,endang,fr2,pt[1]);
		return 0;
		}
    else if (IC_TYPE_FROM_ENAME(ename)==DB_POLYLINE)
		{
		db_vertex *thevert=(db_vertex *)(((db_handitem *)ename[0])->next);
		thevert->get_10(pt[0]);
		for (;;)
			{
			thevert=(db_vertex *)thevert->next;
			if (((db_handitem *)thevert)->ret_type()!=DB_VERTEX)
				break;
			thevert->get_10(pt[1]);
			}
		return 0;
		}
    else if (IC_TYPE_FROM_ENAME(ename)==DB_LWPOLYLINE)
		{
		db_lwpolyline *thelwpline=(db_lwpolyline *)ename[0];
        int nverts;
		thelwpline->get_90(&nverts);
		thelwpline->get_10(pt[0],0);
		thelwpline->get_10(pt[1],nverts-1);
		return 0;
		}
	else if (IC_TYPE_FROM_ENAME(ename)==DB_ELLIPSE || IC_TYPE_FROM_ENAME(ename)==DB_SPLINE) {
		CCurve* pCurve = (IC_TYPE_FROM_ENAME(ename)==DB_ELLIPSE) ?
			             (CCurve*)asGeoEllipArc((db_ellipse*)ename[0]) :
				         (CCurve*)asGeoSpline((db_spline*)ename[0]);

		C3Point start, end;
		if (!pCurve->GetStartPoint(start) && !pCurve->GetEndPoint(end)) {
			pt[0][0] = start.X(); pt[0][1] = start.Y();	pt[0][2] = start.Z();
			pt[1][0] = end.X();	pt[1][1] = end.Y();	pt[1][2] = end.Z();
			delete pCurve;
			return 0;
		}
		delete pCurve;
	}

    return(-1);
}


