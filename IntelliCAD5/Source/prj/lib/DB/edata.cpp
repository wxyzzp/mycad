/* D:\ICADDEV\PRJ\LIB\DB\EDATA.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * $Revision: 1.2.8.1 $ $Date: 2003/03/09 00:06:33 $ 
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 


// THIS FILE HAS BEEN GLOBALIZED!

#include "db.h"/*DNT*/

DB_API short db_edata(sds_name ename, short *closedp, short *nvertsp,
    sds_real *lenp, sds_real *areap) {
//
//  This function determines various properties for the entity whose
//  sds_name is in ename.  Only ename needs to be set by the caller; the
//  pointers are used by this function to return values to the caller.
//  Any pointers to unneeded values can be NULL.
//
//  Currently, only LINE, ARC, CIRCLE, TRACE, SOLID, 3DFACE and
//	non-mesh POLYLINE entities are processed by this function.
//
//  *closedp is set to 1 for a CIRCLE, TRACE, SOLID, 3DFACE, and closed
//      POLYLINE; 0 for anything else.
//  *nvertsp is set to the number of VERTEXs in a POLYLINE and the
//      number of vertices in a TRACE, SOLID, and 3dFACE (triangular
//      SOLIDs and 3dFACEs (with the 3rd and 4th vertices coincident)
//      will give 3; all others 4). NOTE: for splined pline, this will
//		include total of control and spline vertices.
//  *lenp is set to the length of a LINE and ARC; the circumference of a
//      CIRCLE; the perimeter of a TRACE, SOLID, and 3DFACE; and the
//      total length of a POLYLINE (open or closed). NOTE: if pline has
//		been splined, this will be the length of the spline excluding
//		control vertices.
//  *areap is set to the area; CIRCLE, TRACE, SOLID, 3DFACE, and CLOSED
//      2D POLYINE only; 0.0 for anything else.  (3DFACEs are considered
//      "folded"/*DNT*/ on the p0p2 diagonal.  (That's how ACAD seems to
//      shade them.)NOTE: if pline has been splined, *areap will be the
//		splined area, excluding all control vertices.
//
//  Returns:
//       0 : OK
//      -1 : Entity retrieval error (sds_entget() or ic_assco()).
//      -2 : Not an entity this function handles.
//

	
    int closed,nverts,starting,finishing,p3d,fi1,fi2,fi3,fi4,rc;
    sds_real len,area,bulge,chordlen,rad,ang,ar[5];
    sds_point begpt,ap[5];
    sds_name fename;
 	db_handitem *hip=(db_handitem *)ename[0];

    rc=0;
    len=area=0.0;
	nverts=closed=0;

    ic_encpy(fename,ename);  /* Don't change ename for the caller */

    if (hip->ret_type()==DB_LINE)
		{
        if (lenp!=NULL) 
			{
			db_line *theline=(db_line *)hip;
            theline->get_10(ap[0]);
            theline->get_11(ap[1]);
            ar[0]=ap[1][0]-ap[0][0];
			ar[1]=ap[1][1]-ap[0][1];
            ar[2]=ap[1][2]-ap[0][2];
            len=sqrt(ar[0]*ar[0]+ar[1]*ar[1]+ar[2]*ar[2]);
			}
	    }
	else if (hip->ret_type()==DB_ARC) 
		{
        if (lenp!=NULL) {
			db_arc *thearc=(db_arc *)hip;
            thearc->get_40(&ar[0]);
            thearc->get_50(&ar[1]);
            thearc->get_51(&ar[2]);
            ic_normang(ar+1,ar+2);
            len=ar[0]*(ar[2]-ar[1]);
			}
	    }
	else if (hip->ret_type()==DB_CIRCLE) 
		{
        closed=1;
		db_circle *thecircle=(db_circle *)hip;
        thecircle->get_40(&ar[0]);
        len=2.0*IC_PI*ar[0];
        area=IC_PI*ar[0]*ar[0];
		}
	else if (hip->ret_type()==DB_POLYLINE) 
		{
		db_polyline *thepline=(db_polyline *)hip;
		thepline->get_70(&fi1);
        /* This function doesn't do meshes (16=polygon mesh; 64=PFACE mesh): */
        if (fi1 & (IC_PLINE_POLYFACEMESH | IC_PLINE_3DMESH)) 
			{
			rc=-2; 
			goto out; 
			}

        closed=fi1 & IC_PLINE_CLOSED;
        p3d=((fi1 & IC_PLINE_3DPLINE)!=0);

        starting=1;
		finishing=0;
		bulge=0.0;
        ap[1][0]=ap[1][1]=ap[1][2]=0.0;
		db_handitem *hipwalk=(db_vertex *)hip; // steps up right away
        db_vertex *thevert;
		
		for (;;) 
			{
            ic_ptcpy(ap[0],ap[1]);

			hipwalk=hipwalk->next;
            if (hipwalk->ret_type()==DB_SEQEND)  /* SEQEND */
				{
                if (nverts<2) 
					{
					rc=-1; 
					goto out; 
					}  /* Actually seen this */
                if (!closed)
					break;
                ic_ptcpy(ap[1],begpt);
                finishing=1;
				}
			else   /* VERTEX */
                {
				thevert=(db_vertex *)hipwalk;
				nverts++;
				if (fi1 & IC_PLINE_SPLINEFIT)
					{
					//if spline control pt, keep going - we've already
					//	incremented nverts
					int vflags;
					thevert->get_70(&vflags);
					if (vflags & IC_VERTEX_SPLINEFRAME)
						continue;
					}
				thevert->get_10(ap[1]);
                if (starting) 
					{
                    ic_ptcpy(begpt,ap[1]);
					thevert->get_42(&bulge);
                    starting=0;
                    continue;
					}
	            }

            /* We now have the 2 pts and the bulge for this segment. */

            if (lenp!=NULL || areap!=NULL) 
				{
                /* Add area increment even for an arc segment; we'll add the */
                /* chord slice later: */
                if (closed && areap!=NULL && !p3d)
                    area+=(ap[1][0]-ap[0][0])*
                        0.5*((ap[0][1]-begpt[1])+(ap[1][1]-begpt[1]));

                /* Get chord length: */
                ar[0]=ap[1][0]-ap[0][0]; ar[1]=ap[1][1]-ap[0][1];
                ar[2]=ap[1][2]-ap[0][2];
                chordlen=sqrt(ar[0]*ar[0]+ar[1]*ar[1]+ar[2]*ar[2]);

                if (icadRealEqual(bulge,0.0))   /* Line segment */
					{
                    if (lenp!=NULL) 
						len+=chordlen;
                    /* Area already incremented above. */
					}
				else   /* Arc segment */
                    {
					rad=chordlen*(bulge+1.0/bulge)/4.0;  /* Neg if CW */
                    ang=4.0*atan(bulge);                 /* Neg if CW */
                    if (lenp!=NULL) len+=rad*ang;

                    /* If area increases for positive dx for the chord, then */
                    /* add CW chord slices and subtract CCW ones: */
                    if (closed && areap!=NULL && !p3d)
                        area-=0.5*rad*rad*(ang-sin(ang));
					}
				}

            if (finishing) break;

            /* Get bulge for NEXT segment from THIS VERTEX: */
			thevert->get_42(&bulge);
			}

		}
	else if (hip->ret_type()==DB_LWPOLYLINE) 
		{
		db_lwpolyline *thepline=(db_lwpolyline *)hip;
		int nverts,i;
		thepline->get_90(&nverts);
		thepline->get_70(&fi1);

        closed=fi1 & IC_PLINE_CLOSED;

        starting=1;
		bulge=0.0;
        ap[1][0]=ap[1][1]=ap[1][2]=0.0;
				
		int numvertstoprocess=nverts;
		if (closed)
			numvertstoprocess++;
		for (i=0; i<numvertstoprocess; i++) 
			{
            ic_ptcpy(ap[0],ap[1]);

			if (i==nverts && closed) // loop to the start, pline was closed
				thepline->get_10(ap[1],0);
			else
				thepline->get_10(ap[1],i);

            if (starting) 
				{
                ic_ptcpy(begpt,ap[1]);
				thepline->get_42(&bulge,i);
                starting=0;
                continue;
				}

            /* We now have the 2 pts and the bulge for this segment. */

            if (lenp!=NULL || areap!=NULL) 
				{
                /* Add area increment even for an arc segment; we'll add the */
                /* chord slice later: */
                if (closed && areap!=NULL)
                    area+=(ap[1][0]-ap[0][0])*
                        0.5*((ap[0][1]-begpt[1])+(ap[1][1]-begpt[1]));

                /* Get chord length: */
                ar[0]=ap[1][0]-ap[0][0]; ar[1]=ap[1][1]-ap[0][1];
                ar[2]=ap[1][2]-ap[0][2];
                chordlen=sqrt(ar[0]*ar[0]+ar[1]*ar[1]+ar[2]*ar[2]);

                if (icadRealEqual(bulge,0.0))   /* Line segment */
					{
                    if (lenp!=NULL) 
						len+=chordlen;
                    /* Area already incremented above. */
					}
				else   /* Arc segment */
                    {
					rad=chordlen*(bulge+1.0/bulge)/4.0;  /* Neg if CW */
                    ang=4.0*atan(bulge);                 /* Neg if CW */
                    if (lenp!=NULL) len+=rad*ang;

                    /* If area increases for positive dx for the chord, then */
                    /* add CW chord slices and subtract CCW ones: */
                    if (closed && areap!=NULL)
                        area-=0.5*rad*rad*(ang-sin(ang));
					}
				}

            /* Get bulge for NEXT segment from THIS VERTEX: */
			thepline->get_42(&bulge,i);
			}

		}
	else if (hip->ret_type()==DB_TRACE ||
			 hip->ret_type()==DB_SOLID ||
             hip->ret_type()==DB_3DFACE)
		{

        closed=1;

        if (nvertsp!=NULL || lenp!=NULL || areap!=NULL) {
			db_trace *thetrace;
            db_solid *thesolid;
			db_3dface *the3dface;
			if (hip->ret_type()==DB_TRACE)
				{
				thetrace=(db_trace *)hip;
				thetrace->get_10(ap[0]);
				thetrace->get_11(ap[1]);
				thetrace->get_12(ap[2]);
				thetrace->get_13(ap[3]);
				}
			else if (hip->ret_type()==DB_SOLID)
				{
				thesolid=(db_solid *)hip;
				thesolid->get_10(ap[0]);
				thesolid->get_11(ap[1]);
				thesolid->get_12(ap[2]);
				thesolid->get_13(ap[3]);
				}
			else if (hip->ret_type()==DB_3DFACE)
				{
				the3dface=(db_3dface *)hip;
				the3dface->get_10(ap[0]);
				the3dface->get_11(ap[1]);
				the3dface->get_12(ap[2]);
				the3dface->get_13(ap[3]);
				}
            nverts=3;
			if (hip->ret_type()==DB_TRACE || !icadPointEqual(ap[2],ap[3]))
				nverts++;

            if (hip->ret_type()!=DB_3DFACE) {
                /* Swap verts 3 and 4 for TRACE and SOLID: */
                ic_ptcpy(ap[4],ap[2]);
				ic_ptcpy(ap[2],ap[3]);
                ic_ptcpy(ap[3],ap[4]);
            }

            if (lenp!=NULL) {
                for (fi1=0; fi1<4; fi1++) {
                    fi2=(fi1<3) ? fi1+1 : 0;
                    ar[0]=ap[fi2][0]-ap[fi1][0]; ar[1]=ap[fi2][1]-ap[fi1][1];
                    ar[2]=ap[fi2][2]-ap[fi1][2];
                    len+=sqrt(ar[0]*ar[0]+ar[1]*ar[1]+ar[2]*ar[2]);
                }
            }

            if (areap!=NULL) {
                if (hip->ret_type()==DB_3DFACE) {
                    /* Assume that 3DFACES are folded along p0p2 and */
                    /* add the 2 triangles.  (Use Heron's Formula */
                    /* since the triangles don't necessarily lie */
                    /* in the XY-plane.): */

                    for (fi4=0; fi4<nverts-2; fi4++) {  /* One or two triangles */
                        /* Do the 0-1-2 triangle: */
                        for (fi1=0; fi1<3; fi1++) {
                            fi2=(fi1<2) ? fi1+1 : 0;
                            for (fi3=0; fi3<3; fi3++)
                                ar[fi1+fi3]=ap[fi2][fi3]-ap[fi1][fi3];
                            ar[fi1]=sqrt(ar[fi1]*ar[fi1]+ar[fi1+1]*ar[fi1+1]+
                                ar[fi1+2]*ar[fi1+2]);
                        }
                        ar[3]=0.5*(ar[0]+ar[1]+ar[2]);
                        area+=sqrt(ar[3]*(ar[3]-ar[0])*(ar[3]-ar[1])*(ar[3]-ar[2]));

                        /* SINCE WE DON'T NEED THE POINTS AFTER THIS, */
                        /* copy 2 to 1 and 3 to 2 and go again: */
                        if (!fi4 && nverts>3)
                            { ic_ptcpy(ap[1],ap[2]); ic_ptcpy(ap[2],ap[3]); }
                    }

                } else {
                    /* Use the standard bow tie area logic */
                    /* for TRACE and SOLID: */
                    area=0.5*fabs((ap[2][0]-ap[0][0])*(ap[3][1]-ap[1][1])-
                             (ap[3][0]-ap[1][0])*(ap[2][1]-ap[0][1]));
                }
            }
        }
    } else {  /* Not an entity this function handles */
        rc=-2; goto out;
    }

out:
    if (closedp!=NULL) *closedp=closed;
    if (nvertsp!=NULL) *nvertsp=nverts;
    if (lenp   !=NULL) *lenp   =len;
    if (areap  !=NULL) *areap  =fabs(area);

    return rc;
}


