/* C:\DEV\LIB\ICADLIB\SEGXSEG.C
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "icadlib.h"/*DNT*/


short ic_segxseg(sds_point s0p0, sds_point s0p1, short type0, sds_point s1p0,
    sds_point s1p1, short type1, sds_real fuzz, sds_point i0, sds_point i1) {
/*
**  Given two segments (line,arc,circle) defined by s0p0, s0p1, s1p0, and s1p1,
**  this function determines whether they intersect and where (i0 and i1).
**
**  The variable names of the 1st 4 sds_points give the segment and point
**  numbers.  For example, s0p1 is "Segment 0, Point 1".
**
**  type0 and type1 indicate the segment types: 0=line; 1=arc; 2=circle,
**													3=ray, 4=xline..
**
**  For line segs, the two s?p? points are the endpoints.For rays & xlines,
**		s?p1 is NOT a vector, but its endpoint (as offset from s?p0)
**
**  For circles or CCW arcs, the 1st sds_point (s?p0) is the center and the 2nd
**  sds_point (s?p1) is as follows:
**    s?p1[0] : radius
**    s?p1[1] : start angle (radians)	(N/A for circles)
**    s?p1[2] : end   angle (radians)	(N/A for circles)
**
**	For rays & xlines, s?p2=point on ent, NOT unit vector for ent
**
**  If the geometry functions (ic_linexline(), ic_linexarc(), and
**  ic_arcxarc()) find no solution (parallel lines, zero length segs,
**  tangent segs, etc.) the seg endpts are checked for coincidence.
**
**  If an intersection is found but is not technically on both segs,
**  the proximity of the intersection to the two segs is checked and
**  the point is taken if it's close enough to both segs.
**
**  Pass parameter "fuzz" is used for the proximity checks.
**
**  Returns the number of valid intersections:
**      0 : No intersections (neither i0 nor i1 is valid).
**      1 : i0 is valid.
**      2 : i0 and i1 are valid.  (For 2 lines, this only occurs with
**            coincident line segs; the 2 endpts are taken).
*/
    short arctyp,linetyp;
	int	take0,take1,fi1,fi2;
    sds_real ar1;
    sds_real *lnp0,*lnp1,*arcp0,*arcp1;
    sds_point ap[4]={{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}};
	char skipexts=0;

    struct { sds_point ll,ur; } ext[2];  /* Extents */


    take0=take1=0;
	
	if(type0<3 && type1<3){
		/* Get extents of seg 0: */
		if (type0==0) {
			/* Get extents of line seg: */
			if (s0p1[0]>s0p0[0]) { ext[0].ur[0]=s0p1[0]; ext[0].ll[0]=s0p0[0]; }
			else                 { ext[0].ur[0]=s0p0[0]; ext[0].ll[0]=s0p1[0]; }
			if (s0p1[1]>s0p0[1]) { ext[0].ur[1]=s0p1[1]; ext[0].ll[1]=s0p0[1]; }
			else                 { ext[0].ur[1]=s0p0[1]; ext[0].ll[1]=s0p1[1]; }
		} else if(type0==1) {
			/* Get extents of the arc: */
			ic_arcextent(s0p0,s0p1[0],s0p1[1],s0p1[2],ext[0].ll,ext[0].ur);
		} else if(type0==2) { //type==2
			/*Get extents of the circle: */
			ext[0].ur[0]=s0p0[0]+s0p1[0];
			ext[0].ll[0]=s0p0[0]-s0p1[0];
			ext[0].ur[1]=s0p0[1]+s0p1[0];
			ext[0].ll[1]=s0p0[1]-s0p1[0];
		}else skipexts=1;
		/* Get extents of seg 1: */
		if (type1==0) {
			/* Get extents of line seg: */
			if (s1p1[0]>s1p0[0]) { ext[1].ur[0]=s1p1[0]; ext[1].ll[0]=s1p0[0]; }
			else                 { ext[1].ur[0]=s1p0[0]; ext[1].ll[0]=s1p1[0]; }
			if (s1p1[1]>s1p0[1]) { ext[1].ur[1]=s1p1[1]; ext[1].ll[1]=s1p0[1]; }
			else                 { ext[1].ur[1]=s1p0[1]; ext[1].ll[1]=s1p1[1]; }
		} else if(type1==1) {
			/* Get extents of the arc: */
			ic_arcextent(s1p0,s1p1[0],s1p1[1],s1p1[2],ext[1].ll,ext[1].ur);
		} else if(type1==2) { //type==2
			/*Get extents of the circle: */
			ext[1].ur[0]=s1p0[0]+s1p1[0];
			ext[1].ll[0]=s1p0[0]-s1p1[0];
			ext[1].ur[1]=s1p0[1]+s1p1[0];
			ext[1].ll[1]=s1p0[1]-s1p1[0];
		}else
			skipexts=1;


		/* See if extents rule out intersection: */
		if (skipexts==0 &&
		   (ext[0].ur[0]+fuzz<ext[1].ll[0] ||
			ext[1].ur[0]+fuzz<ext[0].ll[0] ||
			ext[0].ur[1]+fuzz<ext[1].ll[1] ||
			ext[1].ur[1]+fuzz<ext[0].ll[1])) return 0;
	}
	
    if (type0!=1 && type0!=2 && type1!=1 && type1!=2) {  /* line-line */

        if ((fi1=ic_linexline(s0p0,s0p1,s1p0,s1p1,i0))<0) {  /* No solution */
            /* Check endpt proximity (parallel or zero-length line segs */
            /* joined at endpts): */

			if(type0==4 && type1==4)return 0;//no fuzz check for xlines
			else if(type0==3){
				fi1=ic_ptlndist(s0p0,s1p0,s1p1,&ar1,ap[0]);
				if(ar1<=fuzz &&
					( 1==fi1 ||
					 (0==fi1 && 4==type1) ||
					 (0==fi1 && 3==type1 && ic_direction_equal(s1p0,s1p1,s1p0,ap[0])))){
					ic_ptcpy(i0,s0p0);
					return 1;
				}
			}else if(type1==3){
				fi1=ic_ptlndist(s1p0,s0p0,s0p1,&ar1,ap[0]);
				if(ar1<=fuzz &&
					( 1==fi1 ||
					 (0==fi1 && 4==type0) ||
					 (0==fi1 && 3==type0 && ic_direction_equal(s0p0,s0p1,s0p0,ap[0])))){
					ic_ptcpy(i0,s1p0);
					return 1;
				}
			}
			if(type0==4){
				fi1=ic_ptlndist(s1p0,s0p0,s0p1,&ar1,ap[0]);
				if(ar1<=fuzz && 
					( 1==fi1 ||
					 (0==fi1 && 3==type1))){
					ic_ptcpy(i0,s1p0);
					return(1);
				}
			}else if(type1==4){
				fi1=ic_ptlndist(s0p0,s1p0,s1p1,&ar1,ap[0]);
				if(ar1<=fuzz && 
					( 1==fi1 ||
					 (0==fi1 && 3==type0))){
					ic_ptcpy(i0,s0p0);
					return(1);
				}
			}
			//if either one is a ray or xline, we should have found the int by now
			if(type0>2 || type1>2)return(0);
		
            fi2=0;
			
            if ((fabs(s0p0[0]-s1p0[0])<=fuzz && fabs(s0p0[1]-s1p0[1])<=fuzz) ||
                (fabs(s0p0[0]-s1p1[0])<=fuzz && fabs(s0p0[1]-s1p1[1])<=fuzz))
                    { ic_ptcpy(i0,s0p0); fi2++; }

            if ((fabs(s0p1[0]-s1p0[0])<=fuzz && fabs(s0p1[1]-s1p0[1])<=fuzz) ||
                (fabs(s0p1[0]-s1p1[0])<=fuzz && fabs(s0p1[1]-s1p1[1])<=fuzz)) {

                if (fi2) ic_ptcpy(i1,s0p1); else ic_ptcpy(i0,s0p1);
                fi2++;
            }

            return shortCast(fi2);
        }

		if(type0==4)fi1|=1;
		if(type1==4)fi1|=2;
		if(type0==3 && ic_direction_equal(s0p0,s0p1,s0p0,i0))fi1|=1;
		if(type1==3 && ic_direction_equal(s1p0,s1p1,s1p0,i0))fi1|=2;

        if (fi1==3) return 1;  /* i0 on both */
        else if (fuzz>0.0) {  /* Check proximity to the segs */
            ic_segdist(i0,s0p0,s0p1,0,&ar1,NULL);
            if (ar1<=fuzz) {
                ic_segdist(i0,s1p0,s1p1,0,&ar1,NULL);
                if (ar1<=fuzz) return 1;  /* Close enough to both */
            }
        }

        return 0;

    } else if (type0>0 && type0<3 && type1>0 && type1<3) {  /* arc/cir - arc/cir */

        fi1=ic_arcxarc(s0p0,s0p1[0],s0p1[1],s0p1[2],
                       s1p0,s1p1[0],s1p1[1],s1p1[2],
                       i0,i1);

        if (fi1<0) {  /* No solution; check for endpt coincidence: */

			if(type0==1){
				/* Get endpts of seg 0: */
				ap[0][0]=s0p0[0]+s0p1[0]*cos(s0p1[1]);
				ap[0][1]=s0p0[1]+s0p1[0]*sin(s0p1[1]);
				ap[1][0]=s0p0[0]+s0p1[0]*cos(s0p1[2]);
				ap[1][1]=s0p0[1]+s0p1[0]*sin(s0p1[2]);
			}

			if(type1==1){
				/* Get endpts of seg 1: */
				ap[2][0]=s1p0[0]+s1p1[0]*cos(s1p1[1]);
				ap[2][1]=s1p0[1]+s1p1[0]*sin(s1p1[1]);
				ap[3][0]=s1p0[0]+s1p1[0]*cos(s1p1[2]);
				ap[3][1]=s1p0[1]+s1p1[0]*sin(s1p1[2]);
			}

            fi2=0;
			if((type0==1)&&(type1==1)){		/*Two Arcs*/
				if ((fabs(ap[0][0]-ap[2][0])<=fuzz &&
					 fabs(ap[0][1]-ap[2][1])<=fuzz) ||
					(fabs(ap[0][0]-ap[3][0])<=fuzz &&
					 fabs(ap[0][1]-ap[3][1])<=fuzz))
						{ ic_ptcpy(i0,ap[0]); fi2++; }

				if ((fabs(ap[1][0]-ap[2][0])<=fuzz &&
					 fabs(ap[1][1]-ap[2][1])<=fuzz) ||
					(fabs(ap[1][0]-ap[3][0])<=fuzz &&
					 fabs(ap[1][1]-ap[3][1])<=fuzz)) {

					 if (fi2) ic_ptcpy(i1,ap[1]); else ic_ptcpy(i0,ap[1]);
					 fi2++;
				}
			} else if((type0==2)&&(type1==2)){	/*Two Circles*/
				if((s0p1[0]<=fuzz)||(s1p1[0]<=fuzz)||	//either rr<fuzz
					(((fabs(s0p0[0]-s1p0[0]))<=fuzz)&&	//coincident cc's
					 ((fabs(s0p0[1]-s1p0[1]))<=fuzz)))return(0);

				ar1=((s0p0[0]-s1p0[0])*(s0p0[0]-s1p0[0]))+	//dx squared +
					((s0p0[1]-s1p0[1])*(s0p0[1]-s1p0[1]));	//dy squared

				if(fabs(ar1-((s0p1[0]+s1p1[0])*(s0p1[0]+s1p1[0])))<fuzz){ //(r1+r2)squared

					i0[0]=s0p0[0]+((s0p1[0]/(s0p1[0]+s1p1[0]))*
							(s1p0[0]-s0p0[0])/(s1p0[1]-s0p0[1]));
					i1[1]=s0p0[1]+((s0p1[0]/(s0p1[0]+s1p1[0]))*
							(s1p0[1]-s0p0[1])/(s1p0[0]-s0p0[0]));
					fi2=1;
				}
			}else if((type0==1)&&(type1==2)){	/*Arc & Circle*/
				//check distance of endpoints of arc from circle center
				//note: easier to check square of distances
				//check arc0 endpoint0
				ar1=((ap[0][0]-s1p0[0])*(ap[0][0]-s1p0[0])+	//dx squared					
				   (ap[0][1]-s1p0[1])*(ap[0][1]-s1p0[1]));	//dy squared
				if(fabs(ar1-s1p1[0]*s1p1[0])<=(fuzz*fuzz)){
					ic_ptcpy(i0,ap[0]);
					fi2++;
				}
				//check arc0 endpoint1
				ar1=((ap[1][0]-s1p0[0])*(ap[1][0]-s1p0[0])+	//dx squared					
				   (ap[1][1]-s1p0[1])*(ap[1][1]-s1p0[1]));	//dy squared
				if(fabs(ar1-s1p1[0]*s1p1[0])<=(fuzz*fuzz)){
					if (fi2) ic_ptcpy(i1,ap[1]); else ic_ptcpy(i0,ap[1]);
					fi2++;
				}
			}else{								/*Circle & Arc*/
				//check arc1 endpoint0
				ar1=(ap[2][0]-s0p0[0])*(ap[2][0]-s0p0[0])+ //dx squared
					(ap[2][1]-s0p0[1])*(ap[2][1]-s0p0[1]);	 //dy squared
				if(fabs(ar1-s0p1[0]*s0p1[0])<=(fuzz*fuzz)){
					ic_ptcpy(i0,ap[2]);
					fi2++;
				}
				//check arc1 endpoint1
				ar1=(ap[3][0]-s0p0[0])*(ap[3][0]-s0p0[0])+ //dx squared
					(ap[3][1]-s0p0[1])*(ap[3][1]-s0p0[1]);	 //dy squared
				if(fabs(ar1-s0p1[0]*s0p1[0])<=(fuzz*fuzz)){
					if (fi2) ic_ptcpy(i1,ap[3]); else ic_ptcpy(i0,ap[3]);
					fi2++;
				}
			}
            return shortCast(fi2);

        }

        /* Found solution(s) on the circles */
		if((type0==1)&&(type1==1)){
			take0=((fi1&3)==3);		/* i0 is on both */
	        take1=((fi1&12)==12);	/* i1 is on both */
		}else if((type0==2)&&(type1==2)){
			//both are circles and there are crossings because
			//arcxarc did not return -1
			take0=take1=1;
		}else if(type0==1){	//only 1st entity is arc, 2nd is circle
			take0=(fi1&1);
			take1=(fi1&4);
		}else{				//only 2nd entity is arc, 1st is circle
			take0=(fi1&2);
			take1=(fi1&8);
		}

        if (fuzz>0.0) {  /* Check proximity to the segs */
            if (!take0) {
                ic_segdist(i0,s0p0,s0p1,type0,&ar1,NULL);
                if (ar1<=fuzz) {
                    ic_segdist(i0,s1p0,s1p1,type1,&ar1,NULL);
                    if (ar1<=fuzz) take0=1;
                }
            }
            if (!take1) {
                ic_segdist(i1,s0p0,s0p1,type0,&ar1,NULL);
                if (ar1<=fuzz) {
                    ic_segdist(i1,s1p0,s1p1,type1,&ar1,NULL);
                    if (ar1<=fuzz) take1=1;
                }
            }
        }

        if ((fi1=take0+take1)==0 || fi1==2) return shortCast(fi1);

        if (take1) ic_ptcpy(i0,i1);
        /* Only i1 should be taken; put it in i0. */

        return 1;

    } else {  /* line-arc or arc-line */

        /* Figure out which is which: */
        if (type1==1 || type1==2) { lnp0=s0p0; lnp1=s0p1; arcp0=s1p0; arcp1=s1p1;arctyp=type1;linetyp=type0;}
        else					  { lnp0=s1p0; lnp1=s1p1; arcp0=s0p0; arcp1=s0p1;arctyp=type0;linetyp=type1;}

        fi1=ic_linexarc(lnp0,lnp1,arcp0,arcp1[0],arcp1[1],arcp1[2],i0,i1);

        if (fi1<0) {  /* No solution; check for endpt coincidence: */

			fi2=0;
			
			fi2=ic_ptlndist(arcp0,lnp0,lnp1,&ar1,ap[0]);
			if(fabs(ar1-arcp1[0])>fuzz)return(0);
			if(arctyp==1){//check if int within subtended arc
				ar1=sds_angle(arcp0,ap[0]);
				ic_normang(&arcp1[1],&arcp1[2]);
				ic_normang(&arcp1[1],&ar1);
				if(ar1>(arcp1[2]+(fuzz/arcp1[0])))return(0);
			}
			//now we have point on subtended arc along line path, but is it
			//on the line segment?
			if(0==fi2 && 0==linetyp){
				if(fabs((*lnp0)-ap[0][0])>fuzz ||
				   fabs(*(lnp0+1)-ap[0][1])>fuzz ||
				   fabs((*lnp1)-ap[0][0])>fuzz ||
				   fabs(*(lnp1+1)-ap[0][1])>fuzz)return(0);
			}else if(0==fi2 && 3==linetyp){
				//short linedim;
				//if((ap[0][linedim]-lnp0[linedim])*(lnp1[linedim]-lnp0[linedim])<0.0)return(0);  //Old code - commented out because of uninit. variable, and senseless code AM 4/20/98
				//In this case, we have determined that the line-arc does not intersect.  For the ray,
				//we need to decide - does the projection of the center lie on the valid half of the ray.
				//Note: a ray is a point & dir (i.e a half inf-line).  Only one half is valid.
				//The following dot product does this check correctly.
				if((ap[0][0]-lnp0[0])*(lnp1[0]-lnp0[0])+
				   (ap[0][1]-lnp0[1])*(lnp1[1]-lnp0[1])+
				   (ap[0][2]-lnp0[2])*(lnp1[2]-lnp0[2])  <fuzz)  return(0);
			}
			//we've eliminated all bogus cases, so the line & arc must intersect
			//	within the given fuzz
			ic_ptcpy(i0,lnp0); return(1);
		}
			/*
			if(arctyp==1){
				//* Get endpts of the arc:
				ap[0][0]=arcp0[0]+arcp1[0]*cos(arcp1[1]);
				ap[0][1]=arcp0[1]+arcp1[0]*sin(arcp1[1]);
				ap[1][0]=arcp0[0]+arcp1[0]*cos(arcp1[2]);
				ap[1][1]=arcp0[1]+arcp1[0]*sin(arcp1[2]);

				if ((fabs(lnp0[0]-ap[0][0])<=fuzz &&
					 fabs(lnp0[1]-ap[0][1])<=fuzz) ||
					(fabs(lnp0[0]-ap[1][0])<=fuzz &&
					 fabs(lnp0[1]-ap[1][1])<=fuzz))
						{ ic_ptcpy(i0,lnp0); fi2++; }

				if ((fabs(lnp1[0]-ap[0][0])<=fuzz &&
					 fabs(lnp1[1]-ap[0][1])<=fuzz) ||
					(fabs(lnp1[0]-ap[1][0])<=fuzz &&
					 fabs(lnp1[1]-ap[1][1])<=fuzz)) {

					 if (fi2) ic_ptcpy(i1,lnp1); else ic_ptcpy(i0,lnp1);
					 fi2++;
				}
			}else{	//*arctyp==2 (a circle) 
				//check if either line endpoint is dist arcp1[0] from center
				//check square of distance
				ar1=((lnp0[0]-arcp0[0])*(lnp0[0]-arcp0[0]))+
					((lnp0[1]-arcp0[1])*(lnp0[1]-arcp0[1]));
				if(fabs(arcp1[0]*arcp1[0]-ar1)<=(fuzz*fuzz))
						{ ic_ptcpy(i0,lnp0); fi2++; }

				ar1=((lnp1[0]-arcp0[0])*(lnp1[0]-arcp0[0]))+
					((lnp1[1]-arcp0[1])*(lnp1[1]-arcp0[1]));
				if(fabs(arcp1[0]*arcp1[0]-ar1)<=(fuzz*fuzz)){

					 if (fi2) ic_ptcpy(i1,lnp1); else ic_ptcpy(i0,lnp1);
					 fi2++;
				}
			}

            return fi2;

        }*/

        //* Found solution(s) on the line and circle 
		if(arctyp==1){	//arc
			take0=((fi1&3)==3);  /* i0 is on both */
			take1=((fi1&12)==12);  /* i1 is on both */
		}else{	//circle
			take0=((fi1&1)==1);		 /* i0 is on line */
			take1=((fi1&4)==4);		 /* i1 is on line */
		}

        if (fuzz>0.0) {  /* Check proximity to the segs */
            if (!take0) {
                ic_segdist(i0,lnp0,lnp1,0,&ar1,NULL);
                if (ar1<=fuzz) {
                    ic_segdist(i0,arcp0,arcp1,arctyp,&ar1,NULL);
                    if (ar1<=fuzz) take0=1;
                }
            }
            if (!take1) {
                ic_segdist(i1,lnp0,lnp1,0,&ar1,NULL);
                if (ar1<=fuzz) {
                    ic_segdist(i1,arcp0,arcp1,arctyp,&ar1,NULL);
                    if (ar1<=fuzz) take1=1;
                }
            }
        }

        if ((fi1=take0+take1)==0 || fi1==2) return shortCast(fi1);

        if (take1) ic_ptcpy(i0,i1);
        /* Only i1 should be taken; put it in i0. */

        return 1;

    }  /* end else line-arc or arc-line */


    return 0;  /* For safety */
}


