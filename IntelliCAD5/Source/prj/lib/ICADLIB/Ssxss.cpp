#include "icadlib.h"/*DNT*/
#include "gvector.h"
#include "transf.h"
#include "curve.h"
#include "EntConversion.h"
#include "elliparc.h"
#include "nurbs.h"

#ifdef max
#undef max
#endif
#include <limits>
#include <vector>
#include <algorithm>
using namespace icl;
using namespace std;

short
ic_ssxss
(
 sds_name			ss0,
 sds_name			ss1,
 sds_real			fuzz,
 resbuf**			ptlistp,
 int				mode3d,
 long*				nskipped0p,
 long*				nskipped1p,
 EInterExtendOption	interOption   /* = eExtendNone*/,
 bool				bExtendAnyway /* = true*/
)
{
/*
**  NOTE: THE CALLER MUST FREE LLIST *ptlistp (WITH sds_relrb()) WHEN
**  THIS FUNCTION SETS IT TO NON-NULL.
**
**  Given two selection sets ss0 and ss1, this function finds all of the
**  LINE/ARC/RAY/XLINE/CIRCLE/ELLIPSE/SPLINE/SOLID/3DFACE/TRACE/POLYLINE/VIEWPORT
**  intersections.  That is, the LINEs, ARCs, RAYs, XLINEs, CIRCLEs, ELLIPSEs,
**  SPLINEs, SOLIDs 3DFACEs TRACEs VIEWPORTs and POLYLINEs in ss0 are intersected
**  with those in ss1.  The resulting points are put into a linked list of
**	resbufs (*ptlistp) in WCS coordinates.
**
**
**  If MODE3D==0, this function produces a llist of "plan view" intersections.
**  The elevations of ARCs, CIRCLEs, SOLIDs TRACEs and POLYLINEs are ignored.
**	3D LINEs, 3DFACEs and 3D POLYLINEs, SPLINEs are accepted, but the Z-values
**	are ignored.  The resulting intersection points are given the
**	current elevation for their Z-coordinates.
**  If MODE3D!=0, we require a full 3D intersection.
**
**  The following entities are ignored when found in the selection sets:
**	  All entities except LINEs, ARCs, ELLIPSEs, SPLINEs, SOLIDs, CIRCLEs 3DFACEs
**    and POLYLINEs ARCs & CIRCLEs not parallel to the WCS XY-plane
**	  2D POLYLINEs not parallel to the WCS XY-plane
**	  POLYFACE MESHes
**
**  Here are the pass parameters:
**
**			 ss0 : The first selection set.
**			 ss1 : The second selection set.
**			fuzz : A distance tolerance to defeat the "just touching"
**					 problem -- T-intersections and endpoint
**					 intersections.  When fuzz>0.0 and the intersections
**					 don't precisely lie on both segments, they are checked
**					 for proximity to the segments and are accepted
**					 anyway if they are within fuzz of both segments.
**		 ptlistp : The ADDRESS of a struct resbuf pointer; the llist of
**					 intersection points will be allocated and *ptlistp
**					 will be pointed at it.  All points are in UCS coords.
**			mode3d : 0==plan view intersection, non-zero==3D intersections
**	  nskipped0p : The address of a long to be set to the number of
**					 entities in ss0 ignored because they were of invalid
**					 type.  (Can be NULL if you don't care.)
**	  nskipped1p : The address of a long to be set to the number of
**					 entities in ss1 ignored because they were of invalid
**					 type.  (Can be NULL if you don't care.)
**
**  Returns:
**	   0 : OK; *ptlistp!=NULL if any intersections were found.
**	  -1 : ptlistp is NULL (this ones not optional: must be the address
**			 of a pointer).
**	  -2 : Insufficient memory for the temporary llists of line and arc
**			 segments, or for the intersections llist itself.  (*ptlistp
**			 set to NULL).
*/
	struct seglink { /* Data about each segment */
		char type;		/* 0=line; 1=arc ; 2=circle; 3=ray; 4=xline; 5=ellipse; 6=spline*/
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
	};

	int closed,done,ssidx,nints,fi1,fi2;
	RT	rc;
	long ssmem,nused[2],fl1;
	sds_real bulge,elev,ar[3];
	sds_point begvert,vert[2],ap[2],pt210;
	sds_name ename,etemp,whichss;
	resbuf *elist,rb,*ptlist,*pptlist,*rbp1,rbwcs,rbucs,rbent,*trb;
	seglink *segset[2],*pseg,*lseg,*aseg,*tseg1,*tseg2;
	/* The two llists of segs to intersect with each other */

	sds_point* pXPoints = NULL;

	rc=0; elist=NULL;
	segset[0]=segset[1]=pseg=lseg=aseg=tseg1=tseg2=NULL;
	nused[0]=nused[1]=0L;
	ptlist=pptlist=elist=rbp1=NULL;
	rbwcs.rbnext=rbucs.rbnext=rbent.rbnext=NULL;
	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;rbwcs.resval.rint=0;
	rbent.restype=RTENAME;

	if (nskipped0p!=NULL)
		*nskipped0p=0L;
	if (nskipped1p!=NULL)
		*nskipped1p=0L;

	if (ptlistp==NULL) { rc=-1; goto out; }

	*ptlistp=NULL;

	elev=(sds_getvar("ELEVATION"/*DNT*/,&rb)==RTNORM) ? rb.resval.rreal : 0.0;
	if (fuzz<0.0)
		fuzz=0.0;

	/* BUILD THE segset[] LLISTS: */

	for (ssidx=0; ssidx<2; ssidx++)   /* Each set */
		{
		if (ssidx)
			ic_encpy(whichss,ss1);
		else
			ic_encpy(whichss,ss0);

		for (ssmem=0L; sds_ssname(whichss,ssmem,ename)==RTNORM; ssmem++)
			{
			/* Each entity */

			IC_FREEIT
			ic_encpy(rbent.resval.rlname,ename);

			if ((elist=sds_entget(ename))==NULL ||
				ic_assoc(elist,0))
				continue;

			if (strisame(ic_rbassoc->resval.rstring,"LINE"/*DNT*/)||
				strisame(ic_rbassoc->resval.rstring,"XLINE"/*DNT*/)||
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
				if (!mode3d)
					ap[0][2]=ap[1][2]=elev;  /* Plan view; ignore Z */

				if ((tseg1= new struct seglink )==NULL)
					{
					rc=-2;
					goto out;
					}
				memset(tseg1,0,sizeof(struct seglink));

				tseg1->type=(char)fi1;
				ic_ptcpy(tseg1->pt[0],ap[0]);
				ic_ptcpy(tseg1->pt[1],ap[1]);

				if (segset[ssidx]==NULL)
					segset[ssidx]=tseg1;
				else
					pseg->next=tseg1;
				pseg=tseg1;

				nused[ssidx]++;
				}
			else if ((strisame(ic_rbassoc->resval.rstring,"SOLID"/*DNT*/))||
					   (strsame(ic_rbassoc->resval.rstring,"TRACE"/*DNT*/)))
				{

				/*edges of ent are treated identical to lines */
				/*make entry from 10 pt to 11 pt */

				if (ic_assoc(elist,10))
					continue;
				ic_ptcpy(ap[0],ic_rbassoc->resval.rpoint);
				ic_rbassoc=NULL;  /* Avoid Metaware bug */

				if (ic_assoc(elist,11))
					continue;
				ic_ptcpy(ap[1],ic_rbassoc->resval.rpoint);
				ic_rbassoc=NULL;  /* Avoid Metaware bug */

				if (!mode3d)
					ap[0][2]=ap[1][2]=elev;  /* Plan view; ignore Z */

				sds_trans(ap[0],&rbent,&rbwcs,0,ap[0]);
				sds_trans(ap[1],&rbent,&rbwcs,0,ap[1]);

				if ((tseg1= new struct seglink )==NULL)
					{
					rc=-2;
					goto out;
					}
				memset(tseg1,0,sizeof(struct seglink));

				tseg1->type=0;
				ic_ptcpy(tseg1->pt[0],ap[0]);
				ic_ptcpy(tseg1->pt[1],ap[1]);

				if (segset[ssidx]==NULL)
					segset[ssidx]=tseg1;
				else
					pseg->next=tseg1;
				pseg=tseg1;

				/*make entry from 10 pt to 12 pt */
				if (ic_assoc(elist,12))
					continue;
				ic_ptcpy(ap[1],ic_rbassoc->resval.rpoint);
				if (!mode3d)
					ap[1][2]=elev;

				sds_trans(ap[1],&rbent,&rbwcs,0,ap[1]);

				ic_rbassoc=NULL;

				if ((tseg1= new struct seglink )==NULL)
					{
					rc=-2;
					goto out;
					}
				memset(tseg1,0,sizeof(struct seglink));

				tseg1->type=0;
				ic_ptcpy(tseg1->pt[1],ap[1]);
				ic_ptcpy(tseg1->pt[0],ap[0]);
				pseg->next=tseg1;
				pseg=tseg1;
				/*check if it's a 4-pt solid & make 12-13 leg */
				if (!ic_assoc(elist,13))
					{
					ic_ptcpy(ap[0],ic_rbassoc->resval.rpoint);
					if (!mode3d)
						ap[0][2]=elev;
					sds_trans(ap[0],&rbent,&rbwcs,0,ap[0]);
					}
				else
					ic_ptcpy(ap[0],ap[1]);

				if (!icadPointEqual(ap[0],ap[1]))	//4pt solid
					{
					if ((tseg1= new struct seglink )==NULL)
						{
						rc=-2;
						goto out;
						}
					memset(tseg1,0,sizeof(struct seglink));

					tseg1->type=0;
					ic_ptcpy(tseg1->pt[1],ap[1]);
					ic_ptcpy(tseg1->pt[0],ap[0]);
					pseg->next=tseg1;
					pseg=tseg1;
					}
				ic_assoc(elist,11);
				ic_ptcpy(ap[1],ic_rbassoc->resval.rpoint);
				/*NOTE: at this point, for a 3pt solid we have assoc13 in
				** ap[0], which is the same as assoc12.  This means for a
				**3pt solid we're making the 11-12 leg.  For a 4-pt solid
				**we're making the 11-13 leg					*/
				if (!mode3d)
					ap[1][2]=elev;

				sds_trans(ap[1],&rbent,&rbwcs,0,ap[1]);

				if ((tseg1= new struct seglink )==NULL) { rc=-2; goto out; }
				memset(tseg1,0,sizeof(struct seglink));

				tseg1->type=0;
				ic_ptcpy(tseg1->pt[1],ap[1]);
				ic_ptcpy(tseg1->pt[0],ap[0]);
				pseg->next=tseg1;
				pseg=tseg1;

				nused[ssidx]++;

				}
			else if (strisame(ic_rbassoc->resval.rstring,"VIEWPORT"/*DNT*/))
				{
				if (ic_assoc(elist,10))
					continue;
				ic_ptcpy(ap[0],ic_rbassoc->resval.rpoint);
				ic_rbassoc=NULL;  /* Avoid Metaware bug */

				if (ic_assoc(elist,40))
					continue;
				ar[0]=0.5*ic_rbassoc->resval.rreal;//get half-width
				ic_rbassoc=NULL;  /* Avoid Metaware bug */
				if (ic_assoc(elist,41))
					continue;
				ar[1]=0.5*ic_rbassoc->resval.rreal;//get half-height
				ic_rbassoc=NULL;  /* Avoid Metaware bug */

				if ((tseg1= new struct seglink )==NULL) { rc=-2; goto out; }
				memset(tseg1,0,sizeof(struct seglink));

				ap[0][0]+=ar[0];
				ap[0][1]+=ar[1];
				tseg1->type=0;
				ic_ptcpy(tseg1->pt[0],ap[0]);
				ic_ptcpy(tseg1->pt[1],ap[0]);
				tseg1->pt[1][1]-=2.0*ar[1];
				sds_trans(tseg1->pt[0],&rbucs,&rbwcs,0,tseg1->pt[0]);
				sds_trans(tseg1->pt[1],&rbucs,&rbwcs,0,tseg1->pt[1]);

				if (segset[ssidx]==NULL)
					segset[ssidx]=tseg1;
				else
					pseg->next=tseg1;
				pseg=tseg1;

				if ((tseg1= new struct seglink )==NULL) { rc=-2; goto out; }
				memset(tseg1,0,sizeof(struct seglink));

				tseg1->type=0;
				ic_ptcpy(tseg1->pt[0],ap[0]);
				ic_ptcpy(tseg1->pt[1],ap[0]);
				tseg1->pt[1][0]-=2.0*ar[0];
				sds_trans(tseg1->pt[0],&rbucs,&rbwcs,0,tseg1->pt[0]);
				sds_trans(tseg1->pt[1],&rbucs,&rbwcs,0,tseg1->pt[1]);

				pseg->next=tseg1;
				pseg=tseg1;

				ap[0][0]-=2.0*ar[0];
				ap[0][1]-=2.0*ar[1];
				if ((tseg1= new struct seglink )==NULL) { rc=-2; goto out; }
				memset(tseg1,0,sizeof(struct seglink));

				tseg1->type=0;
				ic_ptcpy(tseg1->pt[0],ap[0]);
				ic_ptcpy(tseg1->pt[1],ap[0]);
				tseg1->pt[1][0]+=2.0*ar[0];
				sds_trans(tseg1->pt[0],&rbucs,&rbwcs,0,tseg1->pt[0]);
				sds_trans(tseg1->pt[1],&rbucs,&rbwcs,0,tseg1->pt[1]);

				pseg->next=tseg1;
				pseg=tseg1;

				if ((tseg1= new struct seglink )==NULL) { rc=-2; goto out; }
				memset(tseg1,0,sizeof(struct seglink));

				tseg1->type=0;
				ic_ptcpy(tseg1->pt[0],ap[0]);
				ic_ptcpy(tseg1->pt[1],ap[0]);
				tseg1->pt[1][1]+=2.0*ar[1];
				sds_trans(tseg1->pt[0],&rbucs,&rbwcs,0,tseg1->pt[0]);
				sds_trans(tseg1->pt[1],&rbucs,&rbwcs,0,tseg1->pt[1]);

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

				if (!mode3d)
					ap[0][2]=ap[1][2]=elev;  /* Plan view; ignore Z */

				if (!(fi1&1))	//if edge is visible
					{
					if ((tseg1= new struct seglink )==NULL) { rc=-2; goto out; }
					memset(tseg1,0,sizeof(struct seglink));

					tseg1->type=0;
					ic_ptcpy(tseg1->pt[0],ap[0]);
					ic_ptcpy(tseg1->pt[1],ap[1]);

					if (segset[ssidx]==NULL)
						segset[ssidx]=tseg1;
					else
						pseg->next=tseg1;
					pseg=tseg1;
					}

				/*make entry from 11 pt to 12 pt */
				if (ic_assoc(elist,12))
					continue;
				ic_ptcpy(ap[0],ic_rbassoc->resval.rpoint);
				ic_rbassoc=NULL;  /* Avoid Metaware bug */

				if (!mode3d)
					ap[0][2]=ap[1][2]=elev;  /* Plan view; ignore Z */

				if (!(fi1&2))	//if edge is visible
					{
					if ((tseg1= new struct seglink )==NULL) { rc=-2; goto out; }
					memset(tseg1,0,sizeof(struct seglink));

					tseg1->type=0;
					ic_ptcpy(tseg1->pt[0],ap[0]);
					ic_ptcpy(tseg1->pt[1],ap[1]);

					if (segset[ssidx]==NULL)
						segset[ssidx]=tseg1;
					else
						pseg->next=tseg1;
					pseg=tseg1;
					}
				/*make entry from 12 pt to 13 pt */
				if (!ic_assoc(elist,13))
					{
					ic_ptcpy(ap[1],ic_rbassoc->resval.rpoint);
					if (!mode3d)
						ap[1][2]=elev;
					}
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

					if (segset[ssidx]==NULL)
						segset[ssidx]=tseg1;
					else
						pseg->next=tseg1;
					pseg=tseg1;
					}
				/*make entry from 13 pt to 10 pt */
				if (ic_assoc(elist,10))
					continue;
				ic_ptcpy(ap[0],ic_rbassoc->resval.rpoint);
				ic_rbassoc=NULL;  /* Avoid Metaware bug */

				if (!mode3d)
					ap[0][2]=elev;  /* Plan view; ignore Z */
				if (!(fi1&8))	//if edge is visible
					{
					if ((tseg1= new struct seglink )==NULL) { rc=-2; goto out; }
					memset(tseg1,0,sizeof(struct seglink));

					tseg1->type=0;
					ic_ptcpy(tseg1->pt[0],ap[0]);
					ic_ptcpy(tseg1->pt[1],ap[1]);

					if (segset[ssidx]==NULL)
						segset[ssidx]=tseg1;
					else
						pseg->next=tseg1;
					pseg=tseg1;
					}
				nused[ssidx]++;
				}
			else if ((strisame(ic_rbassoc->resval.rstring,"ELLIPSE"/*DNT*/))||
					 (strisame(ic_rbassoc->resval.rstring,"SPLINE"/*DNT*/)))
				{
				if ((tseg1= new struct seglink )==NULL)
					{
					rc=-2;
					goto out;
					}
				memset(tseg1,0,sizeof(struct seglink));

				if (strisame(ic_rbassoc->resval.rstring,"ELLIPSE"/*DNT*/))
					tseg1->type=5;	/*ellipse*/
				else
					tseg1->type=6;	/*spline*/

				ic_encpy(tseg1->ename,ename);
				ic_ptcpy(tseg1->extru,pt210);

				if (segset[ssidx]==NULL)
					segset[ssidx]=tseg1;
				else
					pseg->next=tseg1;
				pseg=tseg1;

				nused[ssidx]++;
				}
			else if ((strisame(ic_rbassoc->resval.rstring,"ARC"/*DNT*/))||
					 (strisame(ic_rbassoc->resval.rstring,"CIRCLE"/*DNT*/)))
				{
				//NOTE:  we'll put the ent name in the seglink along w/extru
				if (ic_assoc(elist,10))
					continue;
				ic_ptcpy(ap[0],ic_rbassoc->resval.rpoint);
				ic_rbassoc=NULL;  /* Avoid Metaware bug */
				if (!mode3d)
					ap[0][2]=elev;  /* Plan view; ignore Z */

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
				if (!mode3d && (fabs(pt210[2]-1.0)>IC_ZRO))
					continue;

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

				if (segset[ssidx]==NULL)
					segset[ssidx]=tseg1;
				else
					pseg->next=tseg1;
				pseg=tseg1;

				nused[ssidx]++;
				}
			else if (strisame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/))
				{
				fi1=(!ic_assoc(elist,70)) ? ic_rbassoc->resval.rint : 0;
				if (fi1 & 64)
					continue;  /* No pface meshes */
				closed=(fi1 & 1);
				ic_encpy(etemp,ename);

				ic_assoc(elist,210);
				ic_ptcpy(pt210,ic_rbassoc->resval.rpoint);
				if (!mode3d)
					{
					/* No POLYLINEs that aren't parallel to the WCS XY-plane: */
					if ((fabs(pt210[2]-1.0))>IC_ZRO)
						continue;
					}

				/* Get 1st VERTEX pt and bulge: */
				IC_FREEIT
				if (sds_entnext_noxref(ename,ename)!=RTNORM ||
					(elist=sds_entget(ename))==NULL ||
					ic_assoc(elist,0) ||
					stricmp(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/) ||
					ic_assoc(elist,10)) continue;
				ic_ptcpy(begvert,ic_rbassoc->resval.rpoint);
				ic_rbassoc=NULL;  /* Avoid Metaware bug */
				if (!mode3d)
					begvert[2]=elev;  /* Plan view; ignore Z */
				ic_ptcpy(vert[1],begvert);

				nused[ssidx]++;

				done=0;
				while (!done)   /* Each VERTEX */
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
						if (!mode3d)
							vert[1][2]=elev;  /* Plan view; ignore Z */
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
						if (!mode3d)
							ap[0][2]=elev;  /* Plan view; ignore Z */
						else
							ap[0][2]=vert[0][2];
						if ((fi1=ic_bulge2arc(vert[0],vert[1],bulge,ap[0],
							ar,ar+1,ar+2))==1)
							goto doline;
						if (fi1)
							continue;

						if ((tseg1= new struct seglink )==NULL) { rc=-2; goto out; }
						memset(tseg1,0,sizeof(struct seglink));

						tseg1->type=1;
						ic_ptcpy(tseg1->pt[0],ap[0]);
						tseg1->pt[1][0]=ar[0];
						tseg1->pt[1][1]=ar[1];
						tseg1->pt[1][2]=ar[2];
						if (0==(fi1&24)) //always the case (3D stuff is never bulged)
							{
							ic_ptcpy(tseg1->extru,pt210);
							ic_encpy(tseg1->ename,ename);
							}
						}

					/* If got here, made a new link; add it: */
					if (segset[ssidx]==NULL)
						segset[ssidx]=tseg1;
					else
						pseg->next=tseg1;
					pseg=tseg1;

					}  /* End while !done */
				//END REGULAR PLINE SECTION

				//BEGIN POLYGON MESH CHECKING

				//we have to do lines going in the "N" direction!
				IC_FREEIT
				ic_encpy(ename,etemp);//go back to main poly ent
				if ((elist=sds_entget(ename))==NULL) continue;
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

						if (!mode3d)
							vert[0][2]=vert[1][2]=elev;

						if ((tseg1= new struct seglink )==NULL) { rc=-2; goto out; }
						memset(tseg1,0,sizeof(struct seglink));

						tseg1->type=0;
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

				ic_assoc(elist,210);
				ic_ptcpy(pt210,ic_rbassoc->resval.rpoint);
				if (!mode3d)
					{
					/* No LWPOLYLINEs that aren't parallel to the WCS XY-plane: */
					if ((fabs(pt210[2]-1.0))>IC_ZRO)
						continue;
					}

				/* Get 1st VERTEX pt and bulge: */
				trb=ic_ret_assoc(elist,10);
				ic_ptcpy(begvert,trb->resval.rpoint);
				ic_ptcpy(vert[1],begvert);

				nused[ssidx]++;

				done=0;
				while (!done)   /* Each VERTEX */
					{
					ic_ptcpy(vert[0],vert[1]);

					trb=ic_ret_assoc(trb,42);
					bulge=trb->resval.rreal;

					/* Step to next subentity: */
					trb=ic_ret_assoc(trb,10);

					if (trb!=NULL)
						{
						ic_ptcpy(vert[1],trb->resval.rpoint);
						if (!mode3d)
							vert[1][2]=elev;  /* Plan view; ignore Z */
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
						sds_trans(vert[0],&rbent,&rbwcs,0,ap[0]);
						sds_trans(vert[1],&rbent,&rbwcs,0,ap[1]);
						ic_ptcpy(tseg1->pt[0],ap[0]);
						ic_ptcpy(tseg1->pt[1],ap[1]);

						}
					else   /* Arc seg */
						{
						if (!mode3d)
							ap[0][2]=elev;  /* Plan view; ignore Z */
						else
							ap[0][2]=vert[0][2];
						if ((fi1=ic_bulge2arc(vert[0],vert[1],bulge,ap[0],
							ar,ar+1,ar+2))==1)
							goto doline2;
						if (fi1)
							continue;

						if ((tseg1= new struct seglink )==NULL) { rc=-2; goto out; }
						memset(tseg1,0,sizeof(struct seglink));

						tseg1->type=1;
						ic_ptcpy(tseg1->pt[0],ap[0]);
						tseg1->pt[1][0]=ar[0];
						tseg1->pt[1][1]=ar[1];
						tseg1->pt[1][2]=ar[2];
						ic_ptcpy(tseg1->extru,pt210);
						ic_encpy(tseg1->ename,ename);
						}

					/* If got here, made a new link; add it: */
					if (segset[ssidx]==NULL)
						segset[ssidx]=tseg1;
					else
						pseg->next=tseg1;
					pseg=tseg1;

					}  /* End while !done */
				} // end lwpline

			}  /* End for each entity */

		}  /* End for each set */


	/* SCAN THE segset[] LLISTS, BUILD THE LLIST OF INTERSECTIONS (ptlist): */

	/*DG - 31.10.2001*/// Here is the algorithm:
	// 1. if bExtendAnyway == true then extend segments depending on value of interOption;
	// 2. otherwise, 1st, try to intersect without extending and
	// 3. if there are no intersections then try to intersect again assuming extension of segments
	//    according to interOption.
	if(bExtendAnyway && (interOption == eExtendBoth || interOption == eExtendFirst))
		for(tseg1 = segset[0]; tseg1; tseg1 = tseg1->next)
			tseg1->type = tseg1->type == 1 ? 2 : 4;

	if(bExtendAnyway && (interOption == eExtendBoth || interOption == eExtendSecond))
		for(tseg1 = segset[1]; tseg1; tseg1 = tseg1->next)
			tseg1->type = tseg1->type == 1 ? 2 : 4;

	for (tseg1=segset[0]; tseg1!=NULL; tseg1=tseg1->next)
		{
		for (tseg2=segset[1]; tseg2!=NULL; tseg2=tseg2->next)
			{
			/* Get the intersections: */
			if (mode3d)
				{
				// use copies of points that may get changed inside ic_3dxseg (eg for circles)
				sds_point	seg1pt1, seg2pt1;
				ic_ptcpy(seg1pt1, tseg1->pt[1]);
				ic_ptcpy(seg2pt1, tseg2->pt[1]);

				nints = ic_3dxseg(tseg1->pt[0], seg1pt1, tseg1->type, tseg1->extru, tseg1->ename,
								  tseg2->pt[0], seg2pt1, tseg2->type, tseg2->extru, tseg2->ename,
								  fuzz, pXPoints);

				if(!nints && !bExtendAnyway && interOption != eExtendNone)
					{
					char	type1 = interOption == eExtendSecond ? tseg1->type : tseg1->type == 1 ? 2 : 4,
							type2 = interOption == eExtendFirst ? tseg2->type : tseg2->type == 1 ? 2 : 4;

					ic_ptcpy(seg1pt1, tseg1->pt[1]);
					ic_ptcpy(seg2pt1, tseg2->pt[1]);

					nints = ic_3dxseg(tseg1->pt[0], seg1pt1, type1, tseg1->extru, tseg1->ename,
									  tseg2->pt[0], seg2pt1, type2, tseg2->extru, tseg2->ename,
									  fuzz, pXPoints);
					}
				}
			else
				{
				fi1=0;//flag for coord sys of ints.  0=wcs
				if (tseg1->type==1 || tseg1->type==2)
					{
					if (tseg2->type==1 || tseg2->type==2)
						{
						//check to make sure extrusions are equal
						if (fabs(tseg1->extru[0]-tseg2->extru[0])>IC_ZRO ||
						   fabs(tseg1->extru[1]-tseg2->extru[1])>IC_ZRO ||
						   fabs(tseg1->extru[2]-tseg2->extru[2])>IC_ZRO)
						   continue;
						else
							fi1=1;//ints are ecs, pts are already in ecs
						}
					else
						{
						ic_ptcpy(rbent.resval.rpoint,tseg1->extru);
						rbent.restype=RT3DPOINT;
						sds_trans(tseg2->pt[0],&rbwcs,&rbent,0,tseg2->pt[0]);
						sds_trans(tseg2->pt[1],&rbwcs,&rbent,0,tseg2->pt[1]);
						fi1=1;
						}
					}
				else if (tseg2->type==1 || tseg2->type==2)
					{
					ic_ptcpy(rbent.resval.rpoint,tseg2->extru);
					rbent.restype=RT3DPOINT;
					sds_trans(tseg1->pt[0],&rbwcs,&rbent,0,tseg1->pt[0]);
					sds_trans(tseg1->pt[1],&rbwcs,&rbent,0,tseg1->pt[1]);
					fi1=1;
					}
				nints=ic_segxseg(tseg1->pt[0],tseg1->pt[1],tseg1->type,
					tseg2->pt[0],tseg2->pt[1],tseg2->type,fuzz,ap[0],ap[1]);

				if(!nints && !bExtendAnyway && interOption != eExtendNone)
					{
					char	type1 = interOption == eExtendSecond ? tseg1->type : tseg1->type == 1 ? 2 : 4,
							type2 = interOption == eExtendFirst ? tseg2->type : tseg2->type == 1 ? 2 : 4;

					nints = ic_segxseg(tseg1->pt[0], tseg1->pt[1], type1,
									   tseg2->pt[0], tseg2->pt[1], type2,
									   fuzz, ap[0], ap[1]);
					}

				if (fi1)
					{
					sds_trans(ap[0],&rbent,&rbwcs,0,ap[0]);
					sds_trans(ap[1],&rbent,&rbwcs,0,ap[1]);
					}

				if (nints) {
					pXPoints = new sds_point [nints];
					for (register i = 0; i < nints; i++)
						ic_ptcpy(pXPoints[i], ap[i]);
				}
				}

			/* Add the points to the llist (ptlist): */
			for (fi1=nints; --fi1 >= 0;)
				{
				if (!mode3d)pXPoints[fi1][2]=elev;  /* Make sure */

				/* See if we already have it (we don't want duplicates): */
				for (rbp1=ptlist; rbp1!=NULL; rbp1=rbp1->rbnext)
				{
					//<alm>
					if (fabs(rbp1->resval.rpoint[0]-pXPoints[fi1][0])<fuzz &&
						fabs(rbp1->resval.rpoint[1]-pXPoints[fi1][1])<fuzz &&
						fabs(rbp1->resval.rpoint[2]-pXPoints[fi1][2])<fuzz)
						break;
				}
				if (rbp1!=NULL)
					continue;  /* Already have it */

				if ((rbp1=sds_newrb(RT3DPOINT))==
					NULL) { rc=-2; goto out; }
				rbp1->restype=RT3DPOINT;
				ic_ptcpy(rbp1->resval.rpoint,pXPoints[fi1]);

				if (ptlist==NULL)
					ptlist=rbp1;
				else
					pptlist->rbnext=rbp1;
				pptlist=rbp1;
				}

			}  /* End for each seg in the 2nd set */
			if (pXPoints) {
				delete [] pXPoints;
				pXPoints = NULL;
			}
		}  /* End for each seg in the 1st set */


out:
	IC_FREEIT

	if (pXPoints) {
		delete [] pXPoints;
		pXPoints = NULL;
	}
	for (ssidx=0; ssidx<2; ssidx++)
		{
		for (tseg1=segset[ssidx]; tseg1!=NULL; tseg1=tseg2)
			{
			tseg2=tseg1->next;
			delete tseg1;
			}
		segset[ssidx]=NULL;
		}

	if (nskipped0p!=NULL && sds_sslength(ss0,&fl1)==RTNORM)
		*nskipped0p=fl1-nused[0];
	if (nskipped1p!=NULL && sds_sslength(ss1,&fl1)==RTNORM)
		*nskipped1p=fl1-nused[1];

	if (rc && ptlist!=NULL)
		{
		sds_relrb(ptlist);
		ptlist=pptlist=NULL;
		}

	if (ptlistp!=NULL)
		*ptlistp=ptlist;

	return rc;
}

struct segment
{
	enum
	{
		eLine = 0,
		eArc = 1,
		eCircle = 2,
		eRay = 3,
		eXline = 4,
		eEllipse = 5,
		eSpline = 6
	};
	char type;		/* 0=line; 1=arc ; 2=circle; 3=ray; 4=xline*; 5=ellipse; 6=spline; */

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

	double distance;
	bool initialized;

	segment()
	:initialized(false)
	{}
};

static
double distanceToSegment
(
segment& seg,
const point& seed
);

struct CompareSegmentsByDistanceToSeed
{
	point m_seed;
	CompareSegmentsByDistanceToSeed(const point& _s)
		:m_seed(_s){}

	bool operator () (segment* s1, segment* s2);
};

inline
bool CompareSegmentsByDistanceToSeed::operator ()
(
segment* s1,
segment* s2
)
{
	if (!s1->initialized)
	{
		s1->distance = distanceToSegment(*s1, m_seed);
		s1->initialized = true;
	}
	if (!s2->initialized)
	{
		s2->distance = distanceToSegment(*s2, m_seed);
		s2->initialized = true;
	}

	return s1->distance < s2->distance;
}


// version of ic_ssxss for intersection snapping
// it returns intersection closest to 'seed' point
short
ic_ssxss2
(
 sds_name			ss0,
 sds_name			ss1,
 sds_real			fuzz,
 sds_point			_seed,
 resbuf**			ptlistp,
 int				mode3d,
 long*				nskipped0p,
 long*				nskipped1p,
 EInterExtendOption	interOption   /* = eExtendNone*/,
 bool				bExtendAnyway /* = true*/
)
{
/*
**  NOTE: THE CALLER MUST FREE LLIST *ptlistp (WITH sds_relrb()) WHEN
**  THIS FUNCTION SETS IT TO NON-NULL.
**
**  Given two selection sets ss0 and ss1, this function finds all of the
**  LINE/ARC/RAY/XLINE/CIRCLE/ELLIPSE/SPLINE/SOLID/3DFACE/TRACE/POLYLINE/VIEWPORT
**  intersections.  That is, the LINEs, ARCs, RAYs, XLINEs, CIRCLEs, ELLIPSEs,
**  SPLINEs, SOLIDs 3DFACEs TRACEs VIEWPORTs and POLYLINEs in ss0 are intersected
**  with those in ss1.  The resulting points are put into a linked list of
**	resbufs (*ptlistp) in WCS coordinates.
**
**
**  If MODE3D==0, this function produces a llist of "plan view" intersections.
**  The elevations of ARCs, CIRCLEs, ELLIPSEs, SPLINEs, SOLIDs TRACEs and POLYLINEs
**  are ignored. 3D LINEs, 3DFACEs and 3D POLYLINEs are accepted, but the Z-values
**	are ignored.  The resulting intersection points are given the
**	current elevation for their Z-coordinates.
**  If MODE3D!=0, we require a full 3D intersection.
**
**  The following entities are ignored when found in the selection sets:
**	  All entities except LINEs, ARCs, SOLIDs, CIRCLEs, ELLIPSEs, SPLINEs,
**    3DFACEs and POLYLINEs, ARCs & CIRCLEs not parallel to the WCS XY-plane
**	  2D POLYLINEs not parallel to the WCS XY-plane, POLYFACE MESHes
**
**  Here are the pass parameters:
**
**			 ss0 : The first selection set.
**			 ss1 : The second selection set.
**			fuzz : A distance tolerance to defeat the "just touching"
**					 problem -- T-intersections and endpoint
**					 intersections.  When fuzz>0.0 and the intersections
**					 don't precisely lie on both segments, they are checked
**					 for proximity to the segments and are accepted
**					 anyway if they are within fuzz of both segments.
**			seed : Point in ucs, we search intersections closest to this point
**		 ptlistp : The ADDRESS of a struct resbuf pointer; the llist of
**					 intersection points will be allocated and *ptlistp
**					 will be pointed at it.  All points are in UCS coords.
**			mode3d : 0==plan view intersection, non-zero==3D intersections
**	  nskipped0p : The address of a long to be set to the number of
**					 entities in ss0 ignored because they were of invalid
**					 type.  (Can be NULL if you don't care.)
**	  nskipped1p : The address of a long to be set to the number of
**					 entities in ss1 ignored because they were of invalid
**					 type.  (Can be NULL if you don't care.)
**
**  Returns:
**	   0 : OK; *ptlistp!=NULL if any intersections were found.
**	  -1 : ptlistp is NULL (this ones not optional: must be the address
**			 of a pointer).
**	  -2 : Insufficient memory for the temporary llists of line and arc
**			 segments, or for the intersections llist itself.  (*ptlistp
**			 set to NULL).
*/
	int closed,done,ssidx,nints,fi1,fi2;
	RT	rc;
	long ssmem,nused[2],fl1;
	sds_real bulge,elev,ar[3];
	sds_point begvert,vert[2],ap[2],pt210;
	sds_name ename,etemp,whichss;
	resbuf *elist,rb,*ptlist,*pptlist,*rbp1,rbwcs,rbucs,rbent,*trb;
	std::vector<segment*> segset[2];
	segment* tseg = NULL;

	int numPoints = 0;
	sds_point* pXPoints = NULL;

	/* The two llists of segs to intersect with each other */

	double dist = std::numeric_limits<double>::max();
	point seed(_seed);

	rc=0; elist=NULL;
	nused[0]=nused[1]=0L;
	ptlist=pptlist=elist=rbp1=NULL;
	rbwcs.rbnext=rbucs.rbnext=rbent.rbnext=NULL;
	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;rbwcs.resval.rint=0;
	rbent.restype=RTENAME;

	if (nskipped0p!=NULL)
		*nskipped0p=0L;
	if (nskipped1p!=NULL)
		*nskipped1p=0L;

	if (ptlistp==NULL) { rc=-1; goto out; }

	*ptlistp=NULL;

	elev=(sds_getvar("ELEVATION"/*DNT*/,&rb)==RTNORM) ? rb.resval.rreal : 0.0;
	if (fuzz<0.0)
		fuzz=0.0;

	/* BUILD THE segset[] LLISTS: */

	for (ssidx=0; ssidx<2; ssidx++)   /* Each set */
		{
		if (ssidx)
			ic_encpy(whichss,ss1);
		else
			ic_encpy(whichss,ss0);

		for (ssmem=0L; sds_ssname(whichss,ssmem,ename)==RTNORM; ssmem++)
			{
			/* Each entity */

			IC_FREEIT
			ic_encpy(rbent.resval.rlname,ename);

			if ((elist=sds_entget(ename))==NULL ||
				ic_assoc(elist,0))
				continue;

			if (strisame(ic_rbassoc->resval.rstring,"LINE"/*DNT*/)||
				strisame(ic_rbassoc->resval.rstring,"XLINE"/*DNT*/)||
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
				if (!mode3d)
					ap[0][2]=ap[1][2]=elev;  /* Plan view; ignore Z */

				if ((tseg=new segment)==NULL)
					{
					rc=-2;
					goto out;
					}

				tseg->type=(char)fi1;
				ic_ptcpy(tseg->pt[0],ap[0]);
				ic_ptcpy(tseg->pt[1],ap[1]);

				segset[ssidx].push_back(tseg);

				nused[ssidx]++;
				}
			else if ((strisame(ic_rbassoc->resval.rstring,"SOLID"/*DNT*/))||
					   (strsame(ic_rbassoc->resval.rstring,"TRACE"/*DNT*/)))
				{

				/*edges of ent are treated identical to lines */
				/*make entry from 10 pt to 11 pt */

				if (ic_assoc(elist,10))
					continue;
				ic_ptcpy(ap[0],ic_rbassoc->resval.rpoint);
				ic_rbassoc=NULL;  /* Avoid Metaware bug */

				if (ic_assoc(elist,11))
					continue;
				ic_ptcpy(ap[1],ic_rbassoc->resval.rpoint);
				ic_rbassoc=NULL;  /* Avoid Metaware bug */

				if (!mode3d)
					ap[0][2]=ap[1][2]=elev;  /* Plan view; ignore Z */

				sds_trans(ap[0],&rbent,&rbwcs,0,ap[0]);
				sds_trans(ap[1],&rbent,&rbwcs,0,ap[1]);

				if ((tseg=new segment)==NULL)
					{
					rc=-2;
					goto out;
					}

				tseg->type=0;
				ic_ptcpy(tseg->pt[0],ap[0]);
				ic_ptcpy(tseg->pt[1],ap[1]);

				segset[ssidx].push_back(tseg);

				/*make entry from 10 pt to 12 pt */
				if (ic_assoc(elist,12))
					continue;
				ic_ptcpy(ap[1],ic_rbassoc->resval.rpoint);
				if (!mode3d)
					ap[1][2]=elev;

				sds_trans(ap[1],&rbent,&rbwcs,0,ap[1]);

				ic_rbassoc=NULL;

				if ((tseg=new segment)==NULL)
					{
					rc=-2;
					goto out;
					}

				tseg->type=0;
				ic_ptcpy(tseg->pt[1],ap[1]);
				ic_ptcpy(tseg->pt[0],ap[0]);
				segset[ssidx].push_back(tseg);

				/*check if it's a 4-pt solid & make 12-13 leg */
				if (!ic_assoc(elist,13))
					{
					ic_ptcpy(ap[0],ic_rbassoc->resval.rpoint);
					if (!mode3d)
						ap[0][2]=elev;
					sds_trans(ap[0],&rbent,&rbwcs,0,ap[0]);
					}
				else
					ic_ptcpy(ap[0],ap[1]);

				if (!icadPointEqual(ap[0],ap[1]))	//4pt solid
					{
					if ((tseg= new segment)==NULL)
						{
						rc=-2;
						goto out;
						}

					tseg->type=0;
					ic_ptcpy(tseg->pt[1],ap[1]);
					ic_ptcpy(tseg->pt[0],ap[0]);
					segset[ssidx].push_back(tseg);
					}
				ic_assoc(elist,11);
				ic_ptcpy(ap[1],ic_rbassoc->resval.rpoint);
				/*NOTE: at this point, for a 3pt solid we have assoc13 in
				** ap[0], which is the same as assoc12.  This means for a
				**3pt solid we're making the 11-12 leg.  For a 4-pt solid
				**we're making the 11-13 leg					*/
				if (!mode3d)
					ap[1][2]=elev;

				sds_trans(ap[1],&rbent,&rbwcs,0,ap[1]);

				if ((tseg=new segment)==NULL)
				{
					rc=-2; goto out;
				}

				tseg->type=0;
				ic_ptcpy(tseg->pt[1],ap[1]);
				ic_ptcpy(tseg->pt[0],ap[0]);

				segset[ssidx].push_back(tseg);

				nused[ssidx]++;

				}
			else if (strisame(ic_rbassoc->resval.rstring,"VIEWPORT"/*DNT*/))
				{
				if (ic_assoc(elist,10))
					continue;
				ic_ptcpy(ap[0],ic_rbassoc->resval.rpoint);
				ic_rbassoc=NULL;  /* Avoid Metaware bug */

				if (ic_assoc(elist,40))
					continue;
				ar[0]=0.5*ic_rbassoc->resval.rreal;//get half-width
				ic_rbassoc=NULL;  /* Avoid Metaware bug */
				if (ic_assoc(elist,41))
					continue;
				ar[1]=0.5*ic_rbassoc->resval.rreal;//get half-height
				ic_rbassoc=NULL;  /* Avoid Metaware bug */

				if ((tseg= new segment)==NULL)
				{
					rc=-2; goto out;
				}

				ap[0][0]+=ar[0];
				ap[0][1]+=ar[1];
				tseg->type=0;
				ic_ptcpy(tseg->pt[0],ap[0]);
				ic_ptcpy(tseg->pt[1],ap[0]);
				tseg->pt[1][1]-=2.0*ar[1];
				sds_trans(tseg->pt[0],&rbucs,&rbwcs,0,tseg->pt[0]);
				sds_trans(tseg->pt[1],&rbucs,&rbwcs,0,tseg->pt[1]);

				segset[ssidx].push_back(tseg);

				if ((tseg=new segment)==NULL)
				{
					rc=-2; goto out;
				}

				tseg->type=0;
				ic_ptcpy(tseg->pt[0],ap[0]);
				ic_ptcpy(tseg->pt[1],ap[0]);
				tseg->pt[1][0]-=2.0*ar[0];
				sds_trans(tseg->pt[0],&rbucs,&rbwcs,0,tseg->pt[0]);
				sds_trans(tseg->pt[1],&rbucs,&rbwcs,0,tseg->pt[1]);

				segset[ssidx].push_back(tseg);

				ap[0][0]-=2.0*ar[0];
				ap[0][1]-=2.0*ar[1];

				if ((tseg=new segment)==NULL)
				{
					rc=-2;
					goto out;
				}

				tseg->type=0;
				ic_ptcpy(tseg->pt[0],ap[0]);
				ic_ptcpy(tseg->pt[1],ap[0]);
				tseg->pt[1][0]+=2.0*ar[0];
				sds_trans(tseg->pt[0],&rbucs,&rbwcs,0,tseg->pt[0]);
				sds_trans(tseg->pt[1],&rbucs,&rbwcs,0,tseg->pt[1]);

				segset[ssidx].push_back(tseg);

				if ((tseg=new segment)==NULL)
				{
					rc=-2;
					goto out;
				}

				tseg->type=0;
				ic_ptcpy(tseg->pt[0],ap[0]);
				ic_ptcpy(tseg->pt[1],ap[0]);
				tseg->pt[1][1]+=2.0*ar[1];
				sds_trans(tseg->pt[0],&rbucs,&rbwcs,0,tseg->pt[0]);
				sds_trans(tseg->pt[1],&rbucs,&rbwcs,0,tseg->pt[1]);

				segset[ssidx].push_back(tseg);
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

				if (!mode3d)
					ap[0][2]=ap[1][2]=elev;  /* Plan view; ignore Z */

				if (!(fi1&1))	//if edge is visible
					{
					if ((tseg=new segment)==NULL) { rc=-2; goto out; }

					tseg->type=0;
					ic_ptcpy(tseg->pt[0],ap[0]);
					ic_ptcpy(tseg->pt[1],ap[1]);

					segset[ssidx].push_back(tseg);
					}

				/*make entry from 11 pt to 12 pt */
				if (ic_assoc(elist,12))
					continue;
				ic_ptcpy(ap[0],ic_rbassoc->resval.rpoint);
				ic_rbassoc=NULL;  /* Avoid Metaware bug */

				if (!mode3d)
					ap[0][2]=ap[1][2]=elev;  /* Plan view; ignore Z */

				if (!(fi1&2))	//if edge is visible
					{
					if ((tseg=new segment)==NULL) { rc=-2; goto out; }

					tseg->type=0;
					ic_ptcpy(tseg->pt[0],ap[0]);
					ic_ptcpy(tseg->pt[1],ap[1]);

					segset[ssidx].push_back(tseg);
					}
				/*make entry from 12 pt to 13 pt */
				if (!ic_assoc(elist,13))
					{
					ic_ptcpy(ap[1],ic_rbassoc->resval.rpoint);
					if (!mode3d)
						ap[1][2]=elev;
					}
				else
					ic_ptcpy(ap[1],ap[0]);
				ic_rbassoc=NULL;  /* Avoid Metaware bug */

				if ((!(fi1&4))&&	//if edge is visible & points not equal
					(!icadPointEqual(ap[0],ap[1])))
					{
					if ((tseg=new segment)==NULL) { rc=-2; goto out; }

					tseg->type=0;
					ic_ptcpy(tseg->pt[0],ap[0]);
					ic_ptcpy(tseg->pt[1],ap[1]);

					segset[ssidx].push_back(tseg);
					}
				/*make entry from 13 pt to 10 pt */
				if (ic_assoc(elist,10))
					continue;
				ic_ptcpy(ap[0],ic_rbassoc->resval.rpoint);
				ic_rbassoc=NULL;  /* Avoid Metaware bug */

				if (!mode3d)
					ap[0][2]=elev;  /* Plan view; ignore Z */
				if (!(fi1&8))	//if edge is visible
					{
					if ((tseg=new segment)==NULL) { rc=-2; goto out; }

					tseg->type=0;
					ic_ptcpy(tseg->pt[0],ap[0]);
					ic_ptcpy(tseg->pt[1],ap[1]);

					segset[ssidx].push_back(tseg);
					}
				nused[ssidx]++;
				}
			else if ((strisame(ic_rbassoc->resval.rstring,"ELLIPSE"/*DNT*/))||
					 (strisame(ic_rbassoc->resval.rstring,"SPLINE"/*DNT*/)))
				{
				if ((tseg=new segment)==NULL)
					{
					rc=-2;
					goto out;
					}

				if (strisame(ic_rbassoc->resval.rstring,"ELLIPSE"/*DNT*/))
					tseg->type=5;	/*ellipse*/
				else
					tseg->type=6;	/*spline*/

				ic_encpy(tseg->ename,ename);
				ic_ptcpy(tseg->extru,pt210);

				segset[ssidx].push_back(tseg);

				nused[ssidx]++;
				}
			else if ((strisame(ic_rbassoc->resval.rstring,"ARC"/*DNT*/))||
					 (strisame(ic_rbassoc->resval.rstring,"CIRCLE"/*DNT*/)))
				{
				//NOTE:  we'll put the ent name in the seglink along w/extru
				if (ic_assoc(elist,10))
					continue;
				ic_ptcpy(ap[0],ic_rbassoc->resval.rpoint);
				ic_rbassoc=NULL;  /* Avoid Metaware bug */
				if (!mode3d)
					ap[0][2]=elev;  /* Plan view; ignore Z */

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
				if (!mode3d && (fabs(pt210[2]-1.0)>IC_ZRO))
					continue;

				if ((tseg=new segment)==NULL) { rc=-2; goto out; }

				ic_assoc(elist,0);
				if (strisame(ic_rbassoc->resval.rstring,"ARC"/*DNT*/))
					tseg->type=1;	/*arc*/
				else
					tseg->type=2;	/*circle*/
				ic_ptcpy(tseg->pt[0],ap[0]);
				tseg->pt[1][0]=ar[0];
				tseg->pt[1][1]=ar[1];
				tseg->pt[1][2]=ar[2];
				ic_encpy(tseg->ename,ename);
				ic_ptcpy(tseg->extru,pt210);

				segset[ssidx].push_back(tseg);

				nused[ssidx]++;
				}
			else if (strisame(ic_rbassoc->resval.rstring,"POLYLINE"/*DNT*/))
				{
				fi1=(!ic_assoc(elist,70)) ? ic_rbassoc->resval.rint : 0;
				if (fi1 & 64)
					continue;  /* No pface meshes */
				closed=(fi1 & 1);
				ic_encpy(etemp,ename);

				ic_assoc(elist,210);
				ic_ptcpy(pt210,ic_rbassoc->resval.rpoint);
				if (!mode3d)
					{
					/* No POLYLINEs that aren't parallel to the WCS XY-plane: */
					if ((fabs(pt210[2]-1.0))>IC_ZRO)
						continue;
					}

				/* Get 1st VERTEX pt and bulge: */
				IC_FREEIT
				if (sds_entnext_noxref(ename,ename)!=RTNORM ||
					(elist=sds_entget(ename))==NULL ||
					ic_assoc(elist,0) ||
					stricmp(ic_rbassoc->resval.rstring,"VERTEX"/*DNT*/) ||
					ic_assoc(elist,10)) continue;
				ic_ptcpy(begvert,ic_rbassoc->resval.rpoint);
				ic_rbassoc=NULL;  /* Avoid Metaware bug */
				if (!mode3d)
					begvert[2]=elev;  /* Plan view; ignore Z */
				ic_ptcpy(vert[1],begvert);

				nused[ssidx]++;

				done=0;
				while (!done)   /* Each VERTEX */
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
						if (!mode3d)
							vert[1][2]=elev;  /* Plan view; ignore Z */
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

						if ((tseg=new segment)==NULL)
								{ rc=-2; goto out; }

						tseg->type=0;
						if (fi1&24) //3D segment, which we've already converted to UCS
							{
							ic_ptcpy(tseg->pt[0],vert[0]);
							ic_ptcpy(tseg->pt[1],vert[1]);
							}
						else  //straight 2D segment in ECS coords...
							{
							sds_trans(vert[0],&rbent,&rbwcs,0,ap[0]);
							sds_trans(vert[1],&rbent,&rbwcs,0,ap[1]);
							ic_ptcpy(tseg->pt[0],ap[0]);
							ic_ptcpy(tseg->pt[1],ap[1]);
							}

						}
					else   /* Arc seg */
						{
						if (!mode3d)
							ap[0][2]=elev;  /* Plan view; ignore Z */
						else
							ap[0][2]=vert[0][2];
						if ((fi1=ic_bulge2arc(vert[0],vert[1],bulge,ap[0],
							ar,ar+1,ar+2))==1)
							goto doline;
						if (fi1)
							continue;

						if ((tseg=new segment)==NULL)
								{ rc=-2; goto out; }

						tseg->type=1;
						ic_ptcpy(tseg->pt[0],ap[0]);
						tseg->pt[1][0]=ar[0];
						tseg->pt[1][1]=ar[1];
						tseg->pt[1][2]=ar[2];
						if (0==(fi1&24)) //always the case (3D stuff is never bulged)
							{
							ic_ptcpy(tseg->extru,pt210);
							ic_encpy(tseg->ename,ename);
							}
						}

					/* If got here, made a new link; add it: */
					segset[ssidx].push_back(tseg);

					}  /* End while !done */
				//END REGULAR PLINE SECTION

				//BEGIN POLYGON MESH CHECKING

				//we have to do lines going in the "N" direction!
				IC_FREEIT
				ic_encpy(ename,etemp);//go back to main poly ent
				if ((elist=sds_entget(ename))==NULL) continue;
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

						if (!mode3d)
							vert[0][2]=vert[1][2]=elev;

						if ((tseg=new segment)==NULL)
								{ rc=-2; goto out; }

						tseg->type=0;
						ic_ptcpy(tseg->pt[0],vert[0]);
						ic_ptcpy(tseg->pt[1],vert[1]);
						segset[ssidx].push_back(tseg);

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

				ic_assoc(elist,210);
				ic_ptcpy(pt210,ic_rbassoc->resval.rpoint);
				if (!mode3d)
					{
					/* No LWPOLYLINEs that aren't parallel to the WCS XY-plane: */
					if ((fabs(pt210[2]-1.0))>IC_ZRO)
						continue;
					}

				/* Get 1st VERTEX pt and bulge: */
				trb=ic_ret_assoc(elist,10);
				ic_ptcpy(begvert,trb->resval.rpoint);
				ic_ptcpy(vert[1],begvert);

				nused[ssidx]++;

				done=0;
				while (!done)   /* Each VERTEX */
					{
					ic_ptcpy(vert[0],vert[1]);

					trb=ic_ret_assoc(trb,42);
					bulge=trb->resval.rreal;

					/* Step to next subentity: */
					trb=ic_ret_assoc(trb,10);

					if (trb!=NULL)
						{
						ic_ptcpy(vert[1],trb->resval.rpoint);
						if (!mode3d)
							vert[1][2]=elev;  /* Plan view; ignore Z */
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

						if ((tseg=new segment)==NULL)
								{ rc=-2; goto out; }

						tseg->type=0;
						sds_trans(vert[0],&rbent,&rbwcs,0,ap[0]);
						sds_trans(vert[1],&rbent,&rbwcs,0,ap[1]);
						ic_ptcpy(tseg->pt[0],ap[0]);
						ic_ptcpy(tseg->pt[1],ap[1]);

						}
					else   /* Arc seg */
						{
						if (!mode3d)
							ap[0][2]=elev;  /* Plan view; ignore Z */
						else
							ap[0][2]=vert[0][2];
						if ((fi1=ic_bulge2arc(vert[0],vert[1],bulge,ap[0],
							ar,ar+1,ar+2))==1)
							goto doline2;
						if (fi1)
							continue;

						if ((tseg=new segment)==NULL)
								{ rc=-2; goto out; }

						tseg->type=1;
						ic_ptcpy(tseg->pt[0],ap[0]);
						tseg->pt[1][0]=ar[0];
						tseg->pt[1][1]=ar[1];
						tseg->pt[1][2]=ar[2];
						ic_ptcpy(tseg->extru,pt210);
						ic_encpy(tseg->ename,ename);
						}

					/* If got here, made a new link; add it: */
					segset[ssidx].push_back(tseg);

					}  /* End while !done */
				} // end lwpline

			}  /* End for each entity */

		}  /* End for each set */


	/* SCAN THE segset[] LLISTS, BUILD THE LLIST OF INTERSECTIONS (ptlist): */

	/*DG - 31.10.2001*/// Here is the algorithm:
	// 1. if bExtendAnyway == true then extend segments depending on value of interOption;
	// 2. otherwise, 1st, try to intersect without extending and
	// 3. if there are no intersections then try to intersect again assuming extension of segments
	//    according to interOption.
	if(bExtendAnyway && (interOption == eExtendBoth || interOption == eExtendFirst))
		for(int iseg = 0, nseg = segset[0].size(); iseg < nseg; iseg++)
		{
			segment* s = segset[0][iseg];
			s->type = (s->type == 1)? 2 : 4;
		}

	if(bExtendAnyway && (interOption == eExtendBoth || interOption == eExtendSecond))
		for(int iseg = 0, nseg = segset[1].size(); iseg < nseg; iseg++)
		{
			segment* s = segset[1][iseg];
			s->type = (s->type == 1)? 2 : 4;
		}

	//<alm>
	//sort segments in increasing order from seed point
	{CompareSegmentsByDistanceToSeed cmp(seed);
	sort(segset[0].begin(), segset[0].end(), cmp);
	sort(segset[1].begin(), segset[1].end(), cmp);}
	//</alm>

	{for (int iseg1 = 0, nseg1 = segset[0].size(); iseg1 < nseg1; iseg1++)
	{
		segment* tseg1 = segset[0][iseg1];
		for (int iseg2 = 0, nseg2 = segset[1].size(); iseg2 < nseg2; iseg2++)
		{
			segment* tseg2 = segset[1][iseg2];
			//<alm>
			{
				//calculate distance from segments to seed
				if (gt(distanceToSegment(*tseg1, seed), dist) ||
					gt(distanceToSegment(*tseg2, seed), dist))
					continue;
			}
			//</alm>
			/* Get the intersections: */
			if (mode3d)
			{
				// use copies of points that may get changed inside ic_3dxseg (eg for circles)
				sds_point	seg1pt1, seg2pt1;
				ic_ptcpy(seg1pt1, tseg1->pt[1]);
				ic_ptcpy(seg2pt1, tseg2->pt[1]);

				nints = ic_3dxseg(tseg1->pt[0], seg1pt1, tseg1->type, tseg1->extru, tseg1->ename,
								  tseg2->pt[0], seg2pt1, tseg2->type, tseg2->extru, tseg2->ename,
								  fuzz, pXPoints);

				if(!nints && !bExtendAnyway && interOption != eExtendNone)
				{
					char	type1 = interOption == eExtendSecond ? tseg1->type : tseg1->type == 1 ? 2 : 4,
							type2 = interOption == eExtendFirst ? tseg2->type : tseg2->type == 1 ? 2 : 4;

					ic_ptcpy(seg1pt1, tseg1->pt[1]);
					ic_ptcpy(seg2pt1, tseg2->pt[1]);

					nints = ic_3dxseg(tseg1->pt[0], seg1pt1, type1, tseg1->extru, tseg1->ename,
									  tseg2->pt[0], seg2pt1, type2, tseg2->extru, tseg2->ename,
									  fuzz, pXPoints);
				}
			}
			else
			{
				fi1=0;//flag for coord sys of ints.  0=wcs
				if (tseg1->type==1 || tseg1->type==2)
				{
					if (tseg2->type==1 || tseg2->type==2)
					{
						//check to make sure extrusions are equal
						if (fabs(tseg1->extru[0]-tseg2->extru[0])>IC_ZRO ||
						   fabs(tseg1->extru[1]-tseg2->extru[1])>IC_ZRO ||
						   fabs(tseg1->extru[2]-tseg2->extru[2])>IC_ZRO)
						   continue;
						else
							fi1=1;//ints are ecs, pts are already in ecs
					}
					else
					{
						ic_ptcpy(rbent.resval.rpoint,tseg1->extru);
						rbent.restype=RT3DPOINT;
						sds_trans(tseg2->pt[0],&rbwcs,&rbent,0,tseg2->pt[0]);
						sds_trans(tseg2->pt[1],&rbwcs,&rbent,0,tseg2->pt[1]);
						fi1=1;
					}
				}
				else if (tseg2->type==1 || tseg2->type==2)
				{
					ic_ptcpy(rbent.resval.rpoint,tseg2->extru);
					rbent.restype=RT3DPOINT;
					sds_trans(tseg1->pt[0],&rbwcs,&rbent,0,tseg1->pt[0]);
					sds_trans(tseg1->pt[1],&rbwcs,&rbent,0,tseg1->pt[1]);
					fi1=1;
				}
				nints=ic_segxseg(tseg1->pt[0],tseg1->pt[1],tseg1->type,
					tseg2->pt[0],tseg2->pt[1],tseg2->type,fuzz,ap[0],ap[1]);

				if(!nints && !bExtendAnyway && interOption != eExtendNone)
				{
					char	type1 = interOption == eExtendSecond ? tseg1->type : tseg1->type == 1 ? 2 : 4,
							type2 = interOption == eExtendFirst ? tseg2->type : tseg2->type == 1 ? 2 : 4;

					nints = ic_segxseg(tseg1->pt[0], tseg1->pt[1], type1,
									   tseg2->pt[0], tseg2->pt[1], type2,
									   fuzz, ap[0], ap[1]);
				}

				if (fi1)
				{
					sds_trans(ap[0],&rbent,&rbwcs,0,ap[0]);
					sds_trans(ap[1],&rbent,&rbwcs,0,ap[1]);
				}

				if (nints) {
					pXPoints = new sds_point [nints];
					for (register i = 0; i < nints; i++)
						ic_ptcpy(pXPoints[i], ap[i]);
				}
			}

			/* Add the points to the llist (ptlist): */
			for (fi1=nints; --fi1 >= 0;)
			{
				if (!mode3d)pXPoints[fi1][2]=elev;  /* Make sure */

				double _dist = (point(pXPoints[fi1]) - seed).norm();
				if (gt(_dist, dist))
					continue;
				dist = _dist;

				/* See if we already have it (we don't want duplicates): */
				for (rbp1=ptlist; rbp1!=NULL; rbp1=rbp1->rbnext)
				{
					if (fabs(rbp1->resval.rpoint[0]-pXPoints[fi1][0])<fuzz &&
						fabs(rbp1->resval.rpoint[1]-pXPoints[fi1][1])<fuzz &&
						fabs(rbp1->resval.rpoint[2]-pXPoints[fi1][2])<fuzz)
						break;
				}
				if (rbp1!=NULL)
					continue;  /* Already have it */

				if ((rbp1=sds_newrb(RT3DPOINT))==
					NULL) { rc=-2; goto out; }
				rbp1->restype=RT3DPOINT;
				ic_ptcpy(rbp1->resval.rpoint,pXPoints[fi1]);

				if (ptlist==NULL)
					ptlist=rbp1;
				else
					pptlist->rbnext=rbp1;
				pptlist=rbp1;
			}

		}  /* End for each seg in the 2nd set */
		if (pXPoints) {
			delete [] pXPoints;
			pXPoints = NULL;
		}
	}}  /* End for each seg in the 1st set */


out:
	IC_FREEIT

	for (ssidx=0; ssidx<2; ssidx++)
	{
		vector<segment*>& sgs = segset[ssidx];
		for (int iseg = 0, nseg = sgs.size(); iseg < nseg; iseg++)
		{
			delete sgs[iseg];
		}
	}

	if (pXPoints) {
		delete [] pXPoints;
		pXPoints = NULL;
	}

	if (nskipped0p!=NULL && sds_sslength(ss0,&fl1)==RTNORM)
		*nskipped0p=fl1-nused[0];
	if (nskipped1p!=NULL && sds_sslength(ss1,&fl1)==RTNORM)
		*nskipped1p=fl1-nused[1];

	if (rc && ptlist!=NULL)
	{
		sds_relrb(ptlist);
		ptlist=pptlist=NULL;
	}

	if (ptlistp!=NULL)
		*ptlistp=ptlist;

	return rc;
}

static
point getClosestPointOnLine
(
const segment& seg,
const point& seed
)
{
	const point start(seg.pt[0]), end(seg.pt[1]);
	gvector disp = end - start;
	if (disp > resabs())
	{
		disp.normalize();
		return start + disp*((seed-start)%disp);
	}
	else
	{
		return start;
	}
}

static
point getClosestPointOnCircle
(
const segment& seg,
const point& seed
)
{
	const gvector normal(seg.extru);
	const point root(0.,0.,0.);
	const transf u2e(coordinate(root, normal));
	const point seed_ecs = seed*u2e;
	const point seed_ecs_pr = seed_ecs*icl::projection(root, normal);
	const gvector radius_dir = (seed_ecs_pr - root).normalized();

	const double radius = seg.pt[1][0];
	return root + radius*radius_dir;
}

static inline
point getPointOnArc
(
double param,
const gvector& xaxis,
const gvector& yaxis,
const point& center
)
{
	return xaxis*cos(param) + yaxis*sin(param) + center;
}


static
bool isInRange(double p, double s, double e)
{
	while (lt(p, s, resnorm()))
		p += IC_TWOPI;
	while (gt(p, e, resnorm()))
		p -= IC_TWOPI;

	if (lt(p, s, resnorm()) ||
		gt(p, e, resnorm()))
		return false;
	return true;
}

static
point getClosestPointOnArc
(
const segment& seg,
const point& seed
)
{
	const point on_circle = getClosestPointOnCircle(seg, seed);

	const point root(0., 0., 0.);
	const gvector to_on_circle = on_circle - root;

	const radius = seg.pt[1][0];
	const gvector xaxis(radius, 0., 0.);
	const gvector yaxis(0., radius, 0.);

	const double y = to_on_circle%yaxis;
	const double x = to_on_circle%xaxis;

	const double param = ic_atan2(y, x);

	const double start_param = seg.pt[1][1];
	const double end_param = seg.pt[1][2];
	if (isInRange(param, start_param, end_param))
		return on_circle;

	const point center = seg.pt[0];
	const point start_pt = getPointOnArc(start_param, xaxis, yaxis, center);
	const point end_pt = getPointOnArc(end_param, xaxis, yaxis, center);

	double dist_to_start = (seed - start_pt).norm();
	double dist_to_end = (seed - end_pt).norm();

	return (dist_to_start < dist_to_end)? start_pt: end_pt;
}

static
point getClosestPointOnEllipse
(
const segment& seg,
const point& seed
)
{
	CEllipArc* pCurve = asGeoEllipArc((db_ellipse*)seg.ename[0]);

	C3Point ptNearest;
	double rAt, dis;
	pCurve->NearestPoint(C3Point(seed.x(), seed.y(), seed.z()), ptNearest, rAt, dis);

	return point(ptNearest.X(), ptNearest.Y(), ptNearest.Z());
}

static
point getClosestPointOnSpline
(
const segment& seg,
const point& seed
)
{
	CNURBS* pCurve = asGeoSpline((db_spline*)seg.ename[0]);

	C3Point ptNearest;
	double rAt, dis;
	pCurve->NearestPoint(C3Point(seed.x(), seed.y(), seed.z()), ptNearest, rAt, dis);

	return point(ptNearest.X(), ptNearest.Y(), ptNearest.Z());
}

static
double distanceToSegment
(
segment& seg,
const point& seed
)
{
	if (seg.initialized)
		return seg.distance;

	point pnt;
	switch (seg.type)
	{
	case segment::eLine:
	case segment::eRay:
	case segment::eXline:

		pnt = getClosestPointOnLine(seg, seed);
		break;

	case segment::eCircle:

		pnt = getClosestPointOnCircle(seg, seed);
		break;

	case segment::eArc:

		pnt = getClosestPointOnArc(seg, seed);
		break;

	case segment::eEllipse:

		pnt = getClosestPointOnEllipse(seg, seed);
		break;

	case segment::eSpline:

		pnt = getClosestPointOnSpline(seg, seed);
		break;

	default:
		assert(false);
		return -std::numeric_limits<double>::max();
	}

	seg.distance = (seed - pnt).norm();
	seg.initialized = true;
	return seg.distance;
}

//</alm>
