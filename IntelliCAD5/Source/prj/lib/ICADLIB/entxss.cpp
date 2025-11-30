
// THIS FILE HAS BEEN GLOBALIZED!

#include "icadlib.h"/*DNT*/
//NOTE: This really should be called LINXSS, but I didn't wanna change
//	the make file and all calls

int pt_equal(sds_point p1, sds_point p2) 
	{

	if (fabs(p2[0])<=IC_ZRO)
		{
		if (fabs(p1[0])>IC_ZRO)
			return(0);
		}
	else if (fabs((p1[0]/p2[0])-1.0) >= IC_ZRO)
		{
		return(0);
		}

	if (fabs(p2[1])<=IC_ZRO)
		{
		if (fabs(p1[1])>IC_ZRO)
			return(0);
		}
	else if (fabs((p1[1]/p2[1])-1.0) >= IC_ZRO)
		{
		return(0);
		}

	if (fabs(p2[2])<=IC_ZRO)
		{
		if (fabs(p1[2])>IC_ZRO)
			return(0);
		}
	else if (fabs((p1[2]/p2[2])-1.0) >= IC_ZRO)
		{
		return(0);
		}
	return(1);
}

int real_equal(sds_real r1, sds_real r2) 
	{
	if (fabs(r2)<=IC_ZRO)
		{
		if (fabs(r1)>IC_ZRO)
			return(0);
		}
	else if (fabs((r1/r2)-1.0) > IC_ZRO)
		return(0);
	return(1);
}





short ic_entxss(sds_point p1, sds_point p2, sds_name ss, sds_real fuzz,
    struct resbuf **ptlistp, long *nfound,int mode3d) {
	//NOTE: this f'n temporarily excluding plines w/64 or 16 bits set.
	//	this is req'd for icad's CMD_HATCH (cmds5.cpp), which uses ssget
	//	filtering that's not yet implemented.
/*
**  NOTE: THE CALLER MUST FREE LLIST *ptlistp (WITH sds_relrb()) WHEN
**  THIS FUNCTION SETS IT TO NON-NULL.
**
**  Given points p1 & p2 in WCS  which define a line and selection set ss, this function finds all
**  LINE/ARC/CIRCLE/SOLID/RAY/XLINE/3DFACE/TRACE/POLYLINE/TEXT intersections.  That is, the LINEs, 
**	ARCs, CIRCLEs, SOLIDs, RAYs, XLINEs, 3DFACEs, POLYLINEs and TEXT in ss are intersected with 
**	an ent defined by p1 & p2.  The resulting enames & points are put into a linked list of 
**	resbufs (*ptlistp).  Control vertices for splined plines are ignored.  
**	If mode3d&4==4, then all duplicate points from list are eliminated.
**	If mode3d&2==2, the llist is in pairs of resbufs.  The 1st is of restype RTENAME and
**	resval.rlname has the name of the intersection ent.  The 2nd is of restype
**	RT3DPOINT and contains the intersection. If emode is 1 the ptlistp ALWAYS RETURNS 
**	RESBUFS IN PAIRS!!!!!  If mode3d&2==0, we get back llist of single resbufs w/points only.
**	This is for speed in hatching.
**  If mode3d&8, and mode3d&1==0 (2d mode)then don't report crossings tangent to an arc.  This is needed for hatching
**  If mode3d&16 and mode3d&1==0  and mode3d&4==0,
**		then only exclude duplicates which are on the same entity, removing
**		crossing points in PAIRS only (req'd for hatch)
**
**  If MODE3D&1==0, this function produces a llist of "plan view" UCS intersections.  
**  The elevations of ARCs, CIRCLEs SOLIDs TRACEs and POLYLINEs are ignored. 
**	3D LINEs, 3DFACEs and 3D POLYLINEs are accepted, but the Z-values 
**	are ignored.  The resulting intersection points are given p1's 
**	elevation for their Z-coordinates. 
**	REMEMBER: 2D MODE TAKES UCS COORDS
**
**  If MODE3D&1!=0, we require a full 3D WCS intersection.
**  The following entities are ignored when found in the selection sets:
**      All entities except LINEs, ARCs, SOLIDs, CIRCLEs 3DFACEs and 3DPOLYLINEs
**      ARCs & CIRCLEs not parallel to the WCS XY-plane
**      2D POLYLINEs not parallel to the WCS XY-plane
**      POLYFACE MESHes
**	REMEMBER: 3D MODE OPERATES IN WCS COORDS
**
**  Here are the pass parameters:
**
**           P1,P2 : The WCS points defining the line to check
**             ss1 : The selection set.
**            fuzz : A distance tolerance to defeat the "just touching"
**                     problem -- T-intersections and endpoint
**                     intersections.  When fuzz>0.0 and the intersections
**                     don't precisely lie on both segments, they are checked
**                     for proximity to the segments and are accepted
**                     anyway if they are within fuzz of both segments.
**         ptlistp : The ADDRESS of a struct resbuf pointer; the llist of
**                     ents & intersection points will be allocated and *ptlistp
**                     will be pointed at it.
**         nfound  : The address of a long to be set to the number of
**                     intersections found.(Can be NULL if you don't care.)
**  		mode3d : Bit 0 is for 3D, bit 1 is for type of llist returned
  
**
**  Returns:
**       0 : OK; *ptlistp!=NULL if any intersections were found.
**      -1 : ptlistp is NULL (this ones not optional: must be the address
**             of a pointer).
**      -2 : Insufficient memory for the temporary llists of line and arc
**             segments, or for the intersections llist itself.  (*ptlistp
**             set to NULL).
*/
    int closed,done,nints,fi1,fi2;
	short rc;
    long ssmem;
    sds_real bulge,ar[3];
    sds_point begvert,vert[2],ap[3],pt210;
    sds_name ename,etemp;
    resbuf *elist,rb,*ptlist,*rbp1,rbwcs,rbucs,rbent,*trb;

	sds_point* pXPoints = NULL;

    struct seglink
		{ /* Data about each segment */
        char type;        /* 0=line; 1=arc ; 2=circle; 3=ray; 4=xline*/

        sds_point pt[2];  /* For a line segment: */
                          /*   The 2 points in UCS (converted from ECS or WCS)*/
                          /* For an arc segment: */
                          /*   pt[0] is the center. */
                          /*   pt[1][0] is the radius */
                          /*   pt[1][1] is the start angle */
                          /*   pt[1][2] is the end angle */
						  /*   NOTE: these are in ECS coords, & use extru & ename */
		sds_point extru;  /* For an arc segment, the arc's Extrusion Direction*/
		sds_name ename;	  /* For an arc segment, the ename */
        struct seglink *next;
		}
	*segset,*pseg,*lseg,*aseg,*tseg1;
    /* The two llists of segs to intersect with each other */


    rc=0;
    segset=pseg=lseg=aseg=tseg1=NULL;
    ptlist=elist=rbp1=NULL;

	rbwcs.rbnext=rbucs.rbnext=rbent.rbnext=NULL;
	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;rbwcs.resval.rint=0;
	rbent.restype=RTENAME;

    if (nfound!=NULL) 
		*nfound=0L;

    if (ptlistp==NULL) 
		{
		rc=-1; 
		goto out; 
		}

    *ptlistp=NULL;

    if (fuzz<0.0) 
		fuzz=0.0;

	
	/* BUILD THE segset[] LLISTS: */

        for (ssmem=0L; sds_ssname(ss,ssmem,ename)==RTNORM; ssmem++) 
			{
            /* Each entity */

            IC_FREEIT
			ic_encpy(rbent.resval.rlname,ename);

            if ((elist=sds_entget(ename))==NULL ||
                ic_assoc(elist,0)) 
				continue;

            if (strisame(ic_rbassoc->resval.rstring,"LINE"/*DNT*/) ||
				strisame(ic_rbassoc->resval.rstring,"XLINE"/*DNT*/) ||
				strisame(ic_rbassoc->resval.rstring,"RAY"/*DNT*/)) 
				{
				if (strisame(ic_rbassoc->resval.rstring,"LINE"/*DNT*/))
					fi1=0;
				else if (strisame(ic_rbassoc->resval.rstring,"RAY"/*DNT*/))
					fi1=3;
				else
					fi1=4;
                if (ic_assoc(elist,10))
					continue;
                ic_ptcpy(ap[0],ic_rbassoc->resval.rpoint);
                ic_rbassoc=NULL;  /* Avoid Metaware bug */

                if (ic_assoc(elist,11))
					continue;
                ic_ptcpy(ap[1],ic_rbassoc->resval.rpoint);
                ic_rbassoc=NULL;  /* Avoid Metaware bug */
				if (fi1>2)
					{
					ap[1][0]+=ap[0][0];
					ap[1][1]+=ap[0][1];
					ap[1][2]+=ap[0][2];
					}

                if ((tseg1= new struct seglink )==NULL) { rc=-2; goto out; }
				memset(tseg1,0,sizeof(struct seglink));

                tseg1->type=(char)fi1;
                ic_ptcpy(tseg1->pt[0],ap[0]);
                ic_ptcpy(tseg1->pt[1],ap[1]);
				tseg1->extru[2]=1.0;
				ic_encpy(tseg1->ename,ename);

                if (segset==NULL) 
					segset=tseg1;
                else
					pseg->next=tseg1;
                pseg=tseg1;

				}
			else if ((strisame(ic_rbassoc->resval.rstring,"SOLID"/*DNT*/)) ||
					   (strsame(ic_rbassoc->resval.rstring,"TRACE"/*DNT*/)))
				{

				/*edges of ent are treated identical to lines */
				/*make entry from 10 pt to 11 pt */
				if (ic_assoc(elist,210))
					{
					pt210[0]=pt210[1]=0.0;
					pt210[2]=1.0;
					
					}
				else
					ic_ptcpy(pt210,ic_rbassoc->resval.rpoint);
                if (ic_assoc(elist,10))
					continue;
                ic_ptcpy(ap[0],ic_rbassoc->resval.rpoint);
                ic_rbassoc=NULL;  /* Avoid Metaware bug */

                if (ic_assoc(elist,11))
					continue;
                ic_ptcpy(ap[1],ic_rbassoc->resval.rpoint);
                ic_rbassoc=NULL;  /* Avoid Metaware bug */


				sds_trans(ap[0],&rbent,&rbwcs,0,ap[0]);
				sds_trans(ap[1],&rbent,&rbwcs,0,ap[1]);

                if ((tseg1= new struct seglink )==NULL) { rc=-2; goto out; }
				memset(tseg1,0,sizeof(struct seglink));

                tseg1->type=0;
                ic_ptcpy(tseg1->pt[0],ap[0]);
                ic_ptcpy(tseg1->pt[1],ap[1]);
				ic_ptcpy(tseg1->extru,pt210);
				ic_encpy(tseg1->ename,ename);

                if (segset==NULL)
					segset=tseg1;
                else
					pseg->next=tseg1;
                pseg=tseg1;

				/*make entry from 10 pt to 12 pt */
				if (ic_assoc(elist,12))
					continue;
				ic_ptcpy(ap[1],ic_rbassoc->resval.rpoint);

				sds_trans(ap[1],&rbent,&rbwcs,0,ap[1]);

				ic_rbassoc=NULL;

				if ((tseg1= new struct seglink )==NULL) { rc=-2; goto out; }
				memset(tseg1,0,sizeof(struct seglink));

                tseg1->type=0;
                ic_ptcpy(tseg1->pt[1],ap[1]);
				ic_ptcpy(tseg1->pt[0],ap[0]);
				ic_ptcpy(tseg1->extru,pt210);
				ic_encpy(tseg1->ename,ename);
                pseg->next=tseg1;
                pseg=tseg1;
				/*check if it's a 4-pt solid & make 12-13 leg */
				if (!ic_assoc(elist,13))
					{
					ic_ptcpy(ap[0],ic_rbassoc->resval.rpoint);
					sds_trans(ap[0],&rbent,&rbwcs,0,ap[0]);
					}
				else
					ic_ptcpy(ap[0],ap[1]);

				if (!icadPointEqual(ap[0],ap[1]))
					{	//4pt solid
					if ((tseg1= new struct seglink )==NULL) { rc=-2; goto out; }
					memset(tseg1,0,sizeof(struct seglink));

					tseg1->type=0;
					ic_ptcpy(tseg1->pt[1],ap[1]);
					ic_ptcpy(tseg1->pt[0],ap[0]);
					ic_ptcpy(tseg1->extru,pt210);
					ic_encpy(tseg1->ename,ename);
					pseg->next=tseg1;
					pseg=tseg1;
					}
				ic_assoc(elist,11);
				ic_ptcpy(ap[1],ic_rbassoc->resval.rpoint);
				/*NOTE: at this point, for a 3pt solid we have assoc13 in
				** ap[0], which is the same as assoc12.  This means for a
				**3pt solid we're making the 11-12 leg.  For a 4-pt solid
				**we're making the 11-13 leg					*/

				sds_trans(ap[1],&rbent,&rbwcs,0,ap[1]);

				if ((tseg1= new struct seglink )==NULL) { rc=-2; goto out; }
				memset(tseg1,0,sizeof(struct seglink));

                tseg1->type=0;
                ic_ptcpy(tseg1->pt[1],ap[1]);
				ic_ptcpy(tseg1->pt[0],ap[0]);
				ic_ptcpy(tseg1->extru,pt210);
				ic_encpy(tseg1->ename,ename);
                pseg->next=tseg1;
                pseg=tseg1;

				}
				else if (strisame(ic_rbassoc->resval.rstring,"3DFACE"/*DNT*/))
					{
				/*edges of face are treated identical to lines if not */
				/*hidden.  make entry from 10 pt to 11 pt */
				fi1=(!ic_assoc(elist,70)) ? ic_rbassoc->resval.rint : 0;

                if (ic_assoc(elist,10))
					continue;
                ic_ptcpy(ap[0],ic_rbassoc->resval.rpoint);
                ic_rbassoc=NULL;  /* Avoid Metaware bug */

                if (ic_assoc(elist,11))
					continue;
                ic_ptcpy(ap[1],ic_rbassoc->resval.rpoint);
                ic_rbassoc=NULL;  /* Avoid Metaware bug */


				if (!(fi1&1))
					{	//if edge is visible
					if ((tseg1= new struct seglink )==NULL) { rc=-2; goto out; }
					memset(tseg1,0,sizeof(struct seglink));

					tseg1->type=0;
					ic_ptcpy(tseg1->pt[0],ap[0]);
					ic_ptcpy(tseg1->pt[1],ap[1]);
					ic_encpy(tseg1->ename,ename);
					tseg1->extru[2]=1.0;

					if (segset==NULL) segset=tseg1;
					else pseg->next=tseg1;
					pseg=tseg1;
					}

				/*make entry from 11 pt to 12 pt */
                if (ic_assoc(elist,12))
					continue;
                ic_ptcpy(ap[0],ic_rbassoc->resval.rpoint);
                ic_rbassoc=NULL;  /* Avoid Metaware bug */


				if (!(fi1&2))
					{	//if edge is visible
					if ((tseg1= new struct seglink )==NULL) { rc=-2; goto out; }
					memset(tseg1,0,sizeof(struct seglink));

					tseg1->type=0;
					ic_ptcpy(tseg1->pt[0],ap[0]);
					ic_ptcpy(tseg1->pt[1],ap[1]);
					ic_encpy(tseg1->ename,ename);
					tseg1->extru[2]=1.0;

					if (segset==NULL)
						segset=tseg1;
					else 
						pseg->next=tseg1;
					pseg=tseg1;
					}
				/*make entry from 12 pt to 13 pt */
                if (!ic_assoc(elist,13))
					ic_ptcpy(ap[1],ic_rbassoc->resval.rpoint);
				else
					ic_ptcpy(ap[1],ap[0]);
                ic_rbassoc=NULL;  /* Avoid Metaware bug */

				if ((!(fi1&4))&&	//if edge is visible & points not equal
					(!icadPointEqual(ap[0],ap[1])))
					{
					if ((tseg1= new struct seglink )==NULL) { rc=-2; goto out; }
					memset(tseg1,0,sizeof(struct seglink));

					tseg1->type=0;
					ic_ptcpy(tseg1->pt[0],ap[0]);
					ic_ptcpy(tseg1->pt[1],ap[1]);
					ic_encpy(tseg1->ename,ename);
					tseg1->extru[2]=1.0;

					if (segset==NULL)
						segset=tseg1;
					else
						pseg->next=tseg1;
					pseg=tseg1;
					}
				/*make entry from 13 pt to 10 pt */
                if (ic_assoc(elist,10))
					continue;
                ic_ptcpy(ap[0],ic_rbassoc->resval.rpoint);
                ic_rbassoc=NULL;  /* Avoid Metaware bug */

				if (!(fi1&8))
					{	//if edge is visible
					if ((tseg1= new struct seglink )==NULL) { rc=-2; goto out; }
					memset(tseg1,0,sizeof(struct seglink));

					tseg1->type=0;
					ic_ptcpy(tseg1->pt[0],ap[0]);
					ic_ptcpy(tseg1->pt[1],ap[1]);
					ic_encpy(tseg1->ename,ename);
					tseg1->extru[2]=1.0;

					if (segset==NULL)
						segset=tseg1;
					else
						pseg->next=tseg1;
					pseg=tseg1;
					}
				}
			else if (strisame(ic_rbassoc->resval.rstring,"ARC"/*DNT*/)||
					 strisame(ic_rbassoc->resval.rstring,"CIRCLE"/*DNT*/))
				{
				//NOTE:  we'll put the ent name in the seglink along w/extru
                if (ic_assoc(elist,10))
					continue;
                ic_ptcpy(ap[0],ic_rbassoc->resval.rpoint);
                ic_rbassoc=NULL;  /* Avoid Metaware bug */

				ar[0]=ar[1]=pt210[0]=pt210[1]=pt210[2]=0.0;
				ar[2]=IC_PI;
				IC_RELRB(rbp1);
				for (rbp1=elist;rbp1!=NULL;rbp1=rbp1->rbnext)
					{
					if (rbp1->restype==40)
						ar[0]=rbp1->resval.rreal;
					if (rbp1->restype==50)
						ar[1]=rbp1->resval.rreal;
					if (rbp1->restype==51)
						ar[2]=rbp1->resval.rreal;
					if (rbp1->restype==210)
						ic_ptcpy(pt210,rbp1->resval.rpoint);
					}
				rbp1=NULL;

                /* If 2D, no arcs that aren't parallel to the WCS XY-plane: */
                //if (!(mode3d&1) && (fabs(pt210[2]-1.0)>IC_ZRO)) continue;

                if ((tseg1= new struct seglink )==NULL) { rc=-2; goto out; }
				memset(tseg1,0,sizeof(struct seglink));

				ic_assoc(elist,0);
				if (strisame(ic_rbassoc->resval.rstring,"ARC"/*DNT*/))
					tseg1->type=1;	/*arc*/
				else
					tseg1->type=2;	/*circle*/
                ic_ptcpy(tseg1->pt[0],ap[0]);
                tseg1->pt[1][0]=ar[0];
                tseg1->pt[1][1]=ar[1];
                tseg1->pt[1][2]=ar[2];
				ic_encpy(tseg1->ename,ename);
				ic_ptcpy(tseg1->extru,pt210);

                if (segset==NULL)
					segset=tseg1;
                else
					pseg->next=tseg1;
                pseg=tseg1;

				}
			else if (strisame(ic_rbassoc->resval.rstring,"POLYLINE"))
				{
                fi1=(!ic_assoc(elist,70)) ? ic_rbassoc->resval.rint : 0;
                if (fi1 & 64)
					continue;  /* No pface meshes */
                closed=(fi1 & 1);
				ic_encpy(etemp,ename);
				pt210[0]=pt210[1]=0.0;
				pt210[2]=1.0;
				if (!(fi1 & (IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)))
					{
					if (0==ic_assoc(elist,210))
						ic_ptcpy(pt210,ic_rbassoc->resval.rpoint);
					}

				/*if (!(mode3d&1)){
					// No POLYLINEs that aren't parallel to the WCS XY-plane: 
                    if ((fabs(pt210[2]-1.0))>IC_ZRO) continue;
				}*/

                /* Get 1st VERTEX pt and bulge: */
                IC_FREEIT
                if (sds_entnext_noxref(ename,ename)!=RTNORM ||
                    (elist=sds_entget(ename))==NULL ||
                    ic_assoc(elist,0) ||
                    stricmp(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/) ||
                    ic_assoc(elist,10)) 
					continue;
                ic_ptcpy(begvert,ic_rbassoc->resval.rpoint);
                ic_rbassoc=NULL;  /* Avoid Metaware bug */
                ic_ptcpy(vert[1],begvert);


                done=0;
                while (!done)  /* Each VERTEX */
					{
                    ic_ptcpy(vert[0],vert[1]);

                    /* Get bulge before stepping to next subentity: */
                    bulge=(!ic_assoc(elist,42)) ?
                        ic_rbassoc->resval.rreal : 0.0;

                    /* Step to next subentity: */
                    IC_FREEIT
                    if (sds_entnext_noxref(ename,ename)!=RTNORM ||
                        (elist=sds_entget(ename))==NULL ||
                        ic_assoc(elist,0))
						break;

                    if (strisame(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/)) 
						{
                        if (ic_assoc(elist,10))
							break;
                        ic_ptcpy(vert[1],ic_rbassoc->resval.rpoint);
                        ic_rbassoc=NULL;			 /* Avoid Metaware bug */

                        /* Don't get bulge here; use prev vert's bulge. */

						}
					else   /* SEQEND */
                        {
						if (!closed)
							break;
                        ic_ptcpy(vert[1],begvert);  /* Close to 1st vert */
                        done=1;
						}

                    if (icadRealEqual(bulge,0.0))   /* Line seg */
                        {
						doline:

                        if ((tseg1= new struct seglink )==NULL) { rc=-2; goto out; }
						memset(tseg1,0,sizeof(struct seglink));

                        tseg1->type=0;
						ic_encpy(tseg1->ename,etemp);
						ic_ptcpy(tseg1->extru,pt210);

						if (fi1&24) //3D segment, which we've already converted to UCS
							{
							ic_ptcpy(tseg1->pt[0],vert[0]);
							ic_ptcpy(tseg1->pt[1],vert[1]);
							}
						else  //straight 2D segment in ECS coords...
							{
							sds_trans(vert[0],&rbent,&rbwcs,0,ap[0]);
							sds_trans(vert[1],&rbent,&rbwcs,0,ap[1]);
							ic_ptcpy(tseg1->pt[0],ap[0]);
							ic_ptcpy(tseg1->pt[1],ap[1]);
							}
						

	                    }
					else   /* Arc seg */
						{
                        if ((fi1=ic_bulge2arc(vert[0],vert[1],bulge,ap[0],
                            ar,ar+1,ar+2))==1)
							goto doline;
                        if (fi1)
							continue;

                        if ((tseg1= new struct seglink )==NULL) { rc=-2; goto out; }
						memset(tseg1,0,sizeof(struct seglink));

                        tseg1->type=1;
                        ic_ptcpy(tseg1->pt[0],ap[0]);
						ic_encpy(tseg1->ename,etemp);
                        tseg1->pt[1][0]=ar[0];
                        tseg1->pt[1][1]=ar[1];
                        tseg1->pt[1][2]=ar[2];
						ic_ptcpy(tseg1->extru,pt210);
						if (0==(fi1&24))//always the case (3D stuff is never bulged)
							ic_encpy(tseg1->ename,ename);
	                    }

                    /* If got here, made a new link; add it: */
                    if (segset==NULL)
						segset=tseg1;
                    else
						pseg->next=tseg1;
                    pseg=tseg1;

					}  /* End while !done */
				//END REGULAR PLINE SECTION

				//BEGIN POLYGON MESH CHECKING

				//we have to do lines going in the "N" direction!
				IC_FREEIT
				ic_encpy(ename,etemp);//go back to main poly ent
				if ((elist=sds_entget(ename))==NULL)
					continue;
				fi1=(!ic_assoc(elist,70)) ? ic_rbassoc->resval.rint : 0;
				if (fi1&16)
					{
					//We'll walk thru mesh w/rbp1 exactly M vertices
					//ahead of elist.  When we get to end of pline, M
					//will be @ seqend & we're done

					//check N direction
					fi1=(!ic_assoc(elist,72)) ? ic_rbassoc->resval.rint : 0;
					if (fi1<2)
						continue;
					//check M direction
					fi1=(!ic_assoc(elist,71)) ? ic_rbassoc->resval.rint : 0;
					if (fi1<2)
						continue;
					IC_FREEIT
					if (sds_entnext_noxref(ename,ename)!=RTNORM)
						continue;
					if ((elist=sds_entget(ename))==NULL)
						continue;
					ic_encpy(etemp,ename);
					for (fi2=0;fi2<fi1;fi2++)
						{
						if (sds_entnext_noxref(etemp,etemp)!=RTNORM)
							continue;
						FreeResbufIfNotNull(&rbp1);
						if ((rbp1=sds_entget(etemp))==NULL)
							continue;
						ic_assoc(rbp1,0);
						if (!strsame(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/)) 
							continue;
						}
					//now we have elist & rbp1 pointing to where 
					//we want to start.
					done=0;
					while(!done)
						{
						ic_assoc(elist,10);
						ic_ptcpy(vert[0],ic_rbassoc->resval.rpoint);
						ic_assoc(rbp1,10);
						ic_ptcpy(vert[1],ic_rbassoc->resval.rpoint);


                        if ((tseg1= new struct seglink )==NULL) { rc=-2; goto out; }
						memset(tseg1,0,sizeof(struct seglink));

                        tseg1->type=0;
						tseg1->extru[2]=1.0;
						ic_encpy(tseg1->ename,ename);
                        ic_ptcpy(tseg1->pt[0],vert[0]);
                        ic_ptcpy(tseg1->pt[1],vert[1]);
						pseg->next=tseg1;
						pseg=tseg1;
						IC_FREEIT
						FreeResbufIfNotNull(&rbp1);
						if (sds_entnext_noxref(ename,ename)!=RTNORM)
							break;
						if (sds_entnext_noxref(etemp,etemp)!=RTNORM)
							break;
						if ((elist=sds_entget(ename))==NULL)
							break;
						if ((rbp1=sds_entget(ename))==NULL)
							break;
						if (closed)
							{
							ic_assoc(elist,0);
							if (!strsame(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/))
								done=1;
							}
						else
							{
							ic_assoc(rbp1,0);
							if (!strsame(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/))
								done=1;
							}
						}//while loop for stepping through vertices
					}//if polygon mesh
				}  /* End else POLYLINE */
			else if (strisame(ic_rbassoc->resval.rstring,"LWPOLYLINE"/*DNT*/))
				{

                fi1=(!ic_assoc(elist,70)) ? ic_rbassoc->resval.rint : 0;
                closed=(fi1 & 1);
				ic_encpy(etemp,ename);
				pt210[0]=pt210[1]=0.0;
				pt210[2]=1.0;
				if (0==ic_assoc(elist,210))
					ic_ptcpy(pt210,ic_rbassoc->resval.rpoint);

                /* Get 1st VERTEX pt and bulge: */
				trb=ic_ret_assoc(elist,10);
                ic_ptcpy(begvert,trb->resval.rpoint);
                ic_ptcpy(vert[1],begvert);


                done=0;
                while (!done)  /* Each VERTEX */
					{
                    ic_ptcpy(vert[0],vert[1]);

                    /* Get bulge before stepping to next subentity: */
                    trb=ic_ret_assoc(trb,42);
					bulge=trb->resval.rreal;

                    /* Step to next subentity: */
					trb=ic_ret_assoc(trb,10);
					if (trb!=NULL)
						{
                        ic_ptcpy(vert[1],trb->resval.rpoint);
                        /* Don't get bulge here; use prev vert's bulge. */
						}
					else   /* end of the verts */
                        {
						if (!closed)
							break;
                        ic_ptcpy(vert[1],begvert);  /* Close to 1st vert */
                        done=1;
						}

                    if (icadRealEqual(bulge,0.0))   /* Line seg */
                        {
						doline2:

                        if ((tseg1= new struct seglink )==NULL) { rc=-2; goto out; }
						memset(tseg1,0,sizeof(struct seglink));

                        tseg1->type=0;
						ic_encpy(tseg1->ename,etemp);
						ic_ptcpy(tseg1->extru,pt210);

						sds_trans(vert[0],&rbent,&rbwcs,0,ap[0]);
						sds_trans(vert[1],&rbent,&rbwcs,0,ap[1]);
						ic_ptcpy(tseg1->pt[0],ap[0]);
						ic_ptcpy(tseg1->pt[1],ap[1]);
	                    }
					else   /* Arc seg */
						{
                        if ((fi1=ic_bulge2arc(vert[0],vert[1],bulge,ap[0],
                            ar,ar+1,ar+2))==1)
							goto doline2;
                        if (fi1)
							continue;

                        if ((tseg1= new struct seglink )==NULL) { rc=-2; goto out; }
						memset(tseg1,0,sizeof(struct seglink));

                        tseg1->type=1;
                        ic_ptcpy(tseg1->pt[0],ap[0]);
						ic_encpy(tseg1->ename,etemp);
                        tseg1->pt[1][0]=ar[0];
                        tseg1->pt[1][1]=ar[1];
                        tseg1->pt[1][2]=ar[2];
						ic_ptcpy(tseg1->extru,pt210);
						ic_encpy(tseg1->ename,ename);
	                    }

                    /* If got here, made a new link; add it: */
                    if (segset==NULL)
						segset=tseg1;
                    else
						pseg->next=tseg1;
                    pseg=tseg1;

					}  /* End while !done */
				}

			}  /* End for each entity */

    /* SCAN THE segset LLIST, BUILD THE LLIST OF INTERSECTIONS (ptlist): */

	if (0==(mode3d&1))
		{
		//get the current extrusion direction
		sds_getvar("UCSXDIR"/*DNT*/,&rb);
		ic_ptcpy(vert[0],rb.resval.rpoint);
		sds_getvar("UCSYDIR"/*DNT*/,&rb);
		ic_ptcpy(vert[1],rb.resval.rpoint);
		pt210[0]=(vert[0][1]*vert[1][2])-(vert[1][1]*vert[0][2]);
		pt210[1]=(vert[0][2]*vert[1][0])-(vert[1][2]*vert[0][0]);
		pt210[2]=(vert[0][0]*vert[1][1])-(vert[1][0]*vert[0][1]);
		}

	rbent.restype=RT3DPOINT;
	if (0==(mode3d&1))
		{
		ic_ptcpy(ap[2],p2);
		ap[2][2]=p1[2];
		}

    for (tseg1=segset; tseg1!=NULL; tseg1=tseg1->next) 
		{

            /* Get the intersections: */
			if (mode3d&1)
				{
				rb.resval.rpoint[0]=rb.resval.rpoint[1]=0.0;
				rb.resval.rpoint[2]=1.0;
				// use copies of points that may get changed inside ic_3dxseg
				sds_point seg1pt1;
				ic_ptcpy(seg1pt1, tseg1->pt[1]);
				sds_point p2cpy;
				ic_ptcpy(p2cpy, p2);
				nints=ic_3dxseg(tseg1->pt[0],seg1pt1,tseg1->type,tseg1->extru,tseg1->ename,
								p1,p2cpy,0,rb.resval.rpoint,NULL,fuzz,pXPoints);
				}
			else
				{
				//Even though we're not doing a 3d int, we have to put
				//all pts into the same coord system. p1 & p2 are UCS pts.
				//All straight segments are wcs & need to be converted to ucs for xing
				if (tseg1->type==2 || tseg1->type==1)
					{
					if (!pt_equal(tseg1->extru,pt210))
						{
						vert[0][0]=-tseg1->extru[0];
						vert[0][1]=-tseg1->extru[1];
						vert[0][2]=-tseg1->extru[2];
						if (!pt_equal(vert[0],pt210))
							continue;
						else
							{
							//anti-parallel extrusions, so switch start and ending angles
							vert[0][0]=tseg1->pt[1][1];
							tseg1->pt[1][1]=tseg1->pt[1][2];
							tseg1->pt[1][2]=vert[0][0];
							}
						}
					ic_ptcpy(rbent.resval.rpoint,tseg1->extru);
					sds_trans(tseg1->pt[0],&rbent,&rbucs,0,vert[0]);
					vert[0][2]=p1[2];
					//all points are now in UCS!!!
					
					nints=ic_segxseg(vert[0],tseg1->pt[1],tseg1->type,
						p1,ap[2],0,fuzz,ap[0],ap[1]);
					if (8==(mode3d&(8+1)) && nints==2 && 
						fabs(ap[0][0]-ap[1][0])<fuzz &&
						fabs(ap[0][1]-ap[1][1])<fuzz)//ignore tangency
							nints=0;
					if (tseg1->type==2 && 8==(mode3d&(8+1)) && nints==1)
						nints=0;//ignore tangency

					if (nints) 
						{
						pXPoints = new sds_point [nints];
						for (register i = 0; i < nints; i++)
							ic_ptcpy(pXPoints[i], ap[i]);
						}

					}
				else
					{
					sds_trans(tseg1->pt[0],&rbwcs,&rbucs,0,vert[0]);
					sds_trans(tseg1->pt[1],&rbwcs,&rbucs,0,vert[1]);
					vert[0][2]=vert[1][2]=p1[2];
					nints=ic_segxseg(vert[0],vert[1],tseg1->type,
						p1,ap[2],0,fuzz,ap[0],ap[1]);
					if (nints) 
						{
						pXPoints = new sds_point [nints];
						for (register i = 0; i < nints; i++)
							ic_ptcpy(pXPoints[i], ap[i]);
						}
					}
				}

            /* Add the points to the llist (ptlist): */
            for (fi1=0; fi1<nints; fi1++) 
				{

                if (0==(mode3d&1))
					pXPoints[fi1][2]=p1[2];  /* Make sure */
				if (mode3d&(4+16))
					{
					//check for duplicate removal
					if (0==(mode3d&1))
						{ 
						//check 2d only
						for (rbp1=ptlist; rbp1!=NULL && 
							(rbp1->restype==RTENAME ||
							 !real_equal(rbp1->resval.rpoint[0],pXPoints[fi1][0]) ||
							 !real_equal(rbp1->resval.rpoint[1],pXPoints[fi1][1]));
							rbp1=rbp1->rbnext);
						}
					else if (mode3d&1)
						{
						for (rbp1=ptlist; rbp1!=NULL && 
							(rbp1->restype==RTENAME ||
							 !pt_equal(rbp1->resval.rpoint,pXPoints[fi1])); 
							rbp1=rbp1->rbnext);
						}
					if (rbp1!=NULL)  //if we already have this intersection
						{
						if (mode3d&4) 
							continue;  // ignore ALL duplicates
						else
							{
							//only ignore duplicates if the ename is the same
							//If mode3d&16, we have double sets of resbufs w/enames
							//re-search and find the ename for the match
							if (rbp1==ptlist->rbnext)
								{
								if (ptlist->resval.rlname[0]==tseg1->ename[0])
									{
									//if it matches the first element in the pts llist, remove 1st 2 resbufs
									rbp1=rbp1->rbnext;
									ptlist->rbnext->rbnext=NULL;
									sds_relrb(ptlist);
									ptlist=rbp1;
									(*nfound)--;
									continue;
									}
								}
							else
								{
								struct resbuf *rbphold;
								char gotit=0;
								rbphold=rbp1;
								for (rbp1=ptlist; rbp1!=NULL && rbp1->rbnext!=NULL && rbp1->rbnext->rbnext!=NULL && rbp1->rbnext->rbnext->rbnext!=NULL;
																	rbp1=rbp1->rbnext->rbnext)
																		{
									//This double llist stuff makes for some weird for loops!!!
									if (rbp1->rbnext->rbnext->rbnext==rbphold && rbp1->rbnext->rbnext->resval.rlname[0]==tseg1->ename[0])
										{
										rbphold=rbphold->rbnext;
										rbp1->rbnext->rbnext->rbnext=NULL;
										sds_relrb(rbp1->rbnext->rbnext);
										rbp1->rbnext->rbnext=rbphold;
										(*nfound)--;
										gotit=1;
										}
									}
								if (gotit==1)continue;
								}
							}
						}
					}
				if (nfound!=NULL)
					(*nfound)++;
				if (mode3d&(2+16))
					{
					//calloc 2 resbufs.  1st w/ename, 2nd w/point.  
					//NOTE:  Always calloc 2 resbuf if we're doing duplicate ent checking
					if ((rbp1=sds_newrb(RTENAME))==
						NULL) { rc=-2; goto out; }
					if ((rbp1->rbnext=sds_newrb(RT3DPOINT))==
						NULL) { rc=-2; goto out; }
					rbp1->restype=RTENAME;
					ic_encpy(rbp1->resval.rlname,tseg1->ename);
					rbp1->rbnext->restype=RT3DPOINT;
					ic_ptcpy(rbp1->rbnext->resval.rpoint,pXPoints[fi1]);
					if (ptlist==NULL)
						ptlist=rbp1; 
					else
						{
						rbp1->rbnext->rbnext=ptlist;
						ptlist=rbp1;
						}
					}
				else
					{
					//just one resbuf w/point
					if ((rbp1=sds_newrb(RT3DPOINT))==
						NULL) { rc=-2; goto out; }
					rbp1->restype=RT3DPOINT;
					ic_ptcpy(rbp1->resval.rpoint,pXPoints[fi1]);
					if (ptlist==NULL) 
						ptlist=rbp1; 
					else
						{
						rbp1->rbnext=ptlist;
						ptlist=rbp1;
						}
					}
	            }
			if (pXPoints) {
				delete [] pXPoints;
				pXPoints = NULL;
			}
		}  /* End for each seg in the set */

	if (16==(mode3d&(2+16)))
		{
		//we were double-allocating to assure that we could check enames for duplicates,
		//	but we really didn't want the double entries
		struct resbuf *rbphold,*rbplast;
		for (rbp1=ptlist;rbp1!=NULL;)
			{
			if (rbp1->restype==RTENAME)
				{
				rbphold=rbp1;
				if (rbp1==ptlist)
					{
					ptlist=rbp1=ptlist->rbnext;
					rbphold->rbnext=NULL;
					sds_relrb(rbphold);
					}
				else
					{
					rbp1=rbp1->rbnext;
					rbplast->rbnext=rbp1;
					rbphold->rbnext=NULL;
					sds_relrb(rbphold);
					}
				}
			else
				{
				rbplast=rbp1;//keep this as a marker for where we last had a pt
				rbp1=rbp1->rbnext;
				}
			}
		}

out:
    IC_FREEIT

	if (pXPoints) {
		delete [] pXPoints;
		pXPoints = NULL;
	}

	if (segset!=NULL)
		{
		tseg1=segset;
		while(tseg1!=NULL)
			{
		    tseg1=tseg1->next;
			delete segset;
			segset=tseg1;
			}
		}


    if (rc && ptlist!=NULL) 
		{
		sds_relrb(ptlist); 
		ptlist=NULL; 
		}

    if (ptlistp!=NULL)
		*ptlistp=ptlist;

    return rc;
}
