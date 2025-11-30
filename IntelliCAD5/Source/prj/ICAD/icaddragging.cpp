		/* C:\DEV\PRJ\ICAD\ICADDRAGGING.CPP
		 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
		 *
		 * Abstract
		 *
		 *
		 */
		// ** Includes
		#include "Icad.h"/*DNT*/
		#include "sdsapplication.h" /*DNT*/
		#include "cmds.h"/*DNT*/
		#include "IcadView.h"
		#include "TextStyleTableInfo.h"/*DNT*/
		#include "sdsthreadexception.h"

	#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
	#endif

//	// Modified CyberAge VSB 20/05/2001
//	#include "IcadDoc.h"
		int			SDS_GetZScale;
		int 			SDS_PointDisplayMode;
		sds_point SDS_PointDisplay;
		bool			SDS_ApplyNoOrtho;

//4M Spiros Item:27->
sds_point 		 SDS_GripLast;
extern bool		SDS_GripPlaced;
//4M Spiros Item:27<-

		extern int		SDS_IsDragging;
		extern bool 	SDS_AtAngDist;
		extern int		SDS_BoxModeCursor;
		extern int		SDS_SkipLastOrthoSet;
		extern char 	SDS_cmdcharbuf[IC_ACADBUF];
		extern int	SDS_AtEntGet;

		extern void SDS_SetOsnapCursor(char *snapstr,int *osnapmode,bool readvar);
//	// Modified CyberAge VSB 20/05/2001 [
//	extern SNAPSTRUCT SnapStructLastSnap;	//Stores the snapped points
//	extern Snap_DrawInOneWindow;			//Stores wether to draw in all views
//	extern int SDS_BoxModeCursor;	//declared in icadapi.cpp, used to where a "selecting rectangle box" is being drawn
//	extern BOOL bGetPoint ;			// if waiting for point.
//	extern int SDS_PointDisplayMode;// if waiting for point.
//	// Modified CyberAge VSB 20/05/2001 ]

//Modified Cybage AW 03-12-01 [
//Reason : Global variables "bNotAPoint" and "strAngleScale" are not used anymore.
/*
//Modified Cybage AW 02-07-01 [
//Reason : The dragmode OFF or ON affect the commands like rotate/Scale
BOOL	bNotAPoint = FALSE;
char strAngleScale[IC_ACADBUF];
//Modified Cybage AW 02-07-01 ]
*/
//Modified Cybage AW 03-12-01 ]
		/**************************************************************************/
		/****************** Dragging Functions ************************************/
		/**************************************************************************/


		void SDS_DrawArc(int color, int hl, int mode, sds_real viewht,
				sds_real r0,sds_real r1,sds_real r2,sds_real r3,sds_real r4,
				sds_real elev) {
		/*
		**	Draws a temporary arc or circle (using sds_grdraw()).  Use -1 for
		**	color if you want ResourceString(IDC_ICADDRAGGING_XOR_INK_0, "XOR ink" ).  Variable hl specifies whether to use
		**	highlighting linetype or not.  Variable viewht must specify the height
		**	of the screen in WCS units (affects resolution).
		**
		**	The five sds_real r? variables describe the arc or circle; variable
		**	ResourceString(IDC_ICADDRAGGING_MODE_1, "mode" ) tells how:
		**
		**	mode		 r0 				r1				r2				r3				r4			 description
		**	----	--------	 -------- 	------	 ---------	-------  ----------------
		**		0 	center X	 center Y 	radius	 start ang	end ang  (CCW arc/circle)
		**		1 		pt 0 X		 pt 0 Y 	pt 1 X	 pt 1 Y 		bulge 	 (Arc)
		**		2 	center X	 center Y 	radius			 -				 -		 (Circle)
		**
		**	Angles are in radians.
		**
		**	The coordinates are expected to be ECS for the ARC, CIRCLE, or
		**	2D POLYLINE involved.  Variable elev is the elevation of the
		**	arc entity in its ECS.	Variables arcextru and ixp are for the
		**	calls to gis_ascend2wcs(); arcextru is the entity's own extrusion
		**	vector; ixp is a llist of INSERT transformation data.  Both
		**	arcextru and ixp can be NULL if the coordinates are intended to
		**	be WCS with no INSERTs involved.
		*/
				int nchords,justadot,fi1;
				sds_real dang,sn,cs,dx,dy,ar1;
				sds_point ecspt[2],ap[3];

				if (viewht<IC_ZRO) return;

				ecspt[0][2]=ecspt[1][2]=elev;
				justadot=0;

				/* Convert to CCW arc definition: */
				if (mode==1) {
						ap[0][0]=r0; ap[0][1]=r1;
						ap[1][0]=r2; ap[1][1]=r3;
						if (ic_bulge2arc(ap[0],ap[1],r4,ap[2],&r2,&r3,&r4)) r2=0.0;
						else { r0=ap[2][0]; r1=ap[2][1]; }
				} else if (mode==2) { r3=0.0; r4=IC_TWOPI; }

				if (r2<0.0) r2=-r2;

				/* Check for zero radius and zero arc-length: */
				if (r2<IC_ZRO || fabs(r3-r4)<IC_ZRO) {
						justadot=1; ecspt[0][0]=r0; ecspt[0][1]=r1;
						if (fabs(r3-r4)<IC_ZRO)
								{ ecspt[0][0]+=r2*cos(r3); ecspt[0][1]+=r2*sin(r3); }
				}
				if (justadot) {
						sds_grdraw(ecspt[0],ecspt[0],color,0);
						return;
				}

				if (mode!=2) ic_normang(&r3,&r4);  /* Don't adjust the forced angles */

				/* Determine number of chords: */
				if ((ar1=r2/viewht*100.0)>100.0) ar1=100.0;
				if (ar1<8.0) ar1=8.0; 	/* 8-100 for full circle */
				/* Adjust for partial circle (but never less than 8) */
				if ((nchords=(int)(ar1*(r4-r3)/IC_TWOPI))<8) nchords=8;

				/* Determine start pt: */
				ecspt[0][0]=r0+r2*cos(r3); ecspt[0][1]=r1+r2*sin(r3);

				dang=(r4-r3)/nchords; cs=cos(dang); sn=sin(dang);

				/* Determine end of 1st chord: */
				ecspt[1][0]=r0+r2*cos(r3+dang); ecspt[1][1]=r1+r2*sin(r3+dang);

				for (fi1=0; fi1<nchords; fi1++) {  /* Draw each chord */

						SDS_DrawLine(ecspt[0],ecspt[1],color,0,0,SDS_CURDWG,0);

						dx=ecspt[1][0]-ecspt[0][0]; dy=ecspt[1][1]-ecspt[0][1];
						ecspt[0][0]=ecspt[1][0]; ecspt[0][1]=ecspt[1][1];  /* Next "from" pt */
						/* Determine next "to" pt: */
						ecspt[1][0]+=dx*cs-dy*sn; ecspt[1][1]+=dy*cs+dx*sn;
				}
		}

		// ** Functions
//	// Modified CyberAge VSB 20/05/2001
//	BOOL bGetPoint = FALSE ; //Is waiting for a point.

		// We should set (pragma optimize "off") here to enable exception hadling at this level
		#pragma optimize( "", off )
		int SDS_dragobject(int mode, int dragit, const sds_point pt1,
							 const sds_point pt2, const sds_real ang,
							 const char *prompt, struct resbuf **retrb,
							 sds_point retpt, char *kword) {

				/*	This function simulates the dragging of objects.	It also passes			 */
				/*	back the last points calculated in a linked list (retrb).  If retrb 	 */
				/*	is NULL the linked list is not passed back.  The linked list will be	 */
				/*	passed back in entget DXF form for LINE, ARC, and CIRCLE.  Note that	 */
				/*	the DXF code 0, layer, linetype, color and EED is not included in 		 */
				/*	the list.  These values must be linked in by the caller.	Warning:		 */
				/*	the linked list for POLYGON is all the points around the POLYGON, 		 */
				/*	the caller must break this list up into VERTEXs before using it with	 */
				/*	entmake.																															 */
				/*																																				 */
				/*	If an invalid object is drawn retrb will be NULL. 										 */
				/*																																				 */
				/*	The dragit parm will cause this function to "drag" the object if its	 */
				/*	value is 1.  If its value is 0 only the rubber band line will be			 */
				/*	drawn.	This parm is for use with the setvar DRAGMODE.								 */
				/*																																				 */
				/*	The prompt parm is the prompt that is displayed while dragging. 			 */
				/*																																				 */
				/*	The parms pt1, pt2 and ang must be filled in depending on the mode		 */
				/*	in which you are dragging.	The mode and required parms are as				 */
				/*	follows:																															 */
				/*																														 Parms			 */
				/*	mode	 Description																		pt1 pt2 ang 		 */
				/*	----	 -------------------------------------------- 	--- --- --- 		 */
				/*		-1	 Special mode to act like internalGetpoint()							 */
			/*					 ** pt1 = sds_name W/elp and flp set						 */
			/*										pt2 = *sds_point to be changed on drag				 */
			/*						ang = thickness if dragging point on extruded edge (0.0 if direct update)*/
				/*		-2	 Special mode to act like sds_getdist()							 */
			/*					 ** pt1 = sds_name W/elp and flp set						 */
			/*										pt2 = *sds_real to be changed on drag					 */
				/*		-3	 Special mode to act like sds_getangle()							 */
			/*					 ** pt1 = sds_name W/elp and flp set						 */
			/*										pt2 = *sds_real to be changed on drag						 */
			/*										retrb = The insertion point of the block				 */
				/*		-4	 Special mode for dragging blocks											 */
			/*					 ** pt1 = sds_name W/elp and flp set						 */
			/*										pt2 = *sds_point to be changed on drag				 */
			/*										retrb = The insertion point of the block				 */
				/*		-5	 Special mode for dynamicly entering text							 */
			/*					 ** pt1 = sds_name W/elp and flp set						 */
				/*		-6	 Real LINE		first/[second]											X 						 */
				/*		-7	 Special mode to act like internalGetpoint()/Direction vector				 */
			/*					 ** pt1 = sds_name W/elp and flp set						 */
			/*										pt2 = *sds_point to be changed on drag				 */
				/*		-8	 Special mode to act like internalGetpoint()/BiSect-Direction vector */
			/*					 ** pt1 = sds_name W/elp and flp set						 */
			/*										pt2 = *sds_point to be changed on drag				 */
				/*		-9	 Special mode for ellipse axisratio 								 */
			/*					 ** pt1 = sds_name W/elp and flp set						 */
			/*										pt2 = *sds_real to be axisratio changed on drag		 */
				/*		-10  Special mode to act like sds_getangle() special for ellipse		 */
			/*					 ** pt1 = sds_name W/elp and flp set						 */
			/*										pt2 = *sds_real to be changed on drag						 */
			/*										retrb = The insertion point of the block				 */
				/*		0 	 ARC			 start/second/[end] 										X 	X 	X(**)  */
				/*		1 	 CIRCLE 	 center [radius]												X 			X		 */
				/*		2 	 LINE 		 first/angle/[length] 									X 			X 		 */
				/*		3 	 CIRCLE 	 first/[second] 												X 						 */
				/*		4 	 CIRCLE 	 first/second/[third] 									X 	X 				 */
				/*		5 	 ARC			 center/start/[angle] 									X 	X 				 */
				/*		6 	 ARC			 center/start/[length of chord] 				X 	X 				 */
				/*		7 	 ARC			 center/start/[end] 										X 	X 				 */
				/*		8 	 ARC			 start/center/[angle] 									X 	X 				 */
				/*		9 	 ARC			 start/center/[end] 										X 	X 				 */
				/*		10	 ARC			 start/center/[length of chord] 				X 	X 				 */
				/*		11	 ARC			 start/end/[angle]											X 	X 				 */
				/*		12	 ARC			 start/end/[direction]									X 	X 				 */
				/*		13	 ARC			 start/end/[radius] 										X 	X 				 */
				/*		14	 ARC			 start/end/[center] 										X 	X 				 */
				/*		15	 ARC			 start/direction/[end]									X 			X 		 */
				/*		16	 POLYGON	 center/n sides/[radius] (inscribed)		X 			X 		 */
				/*		17	 POLYGON	 center/n sides/[radius] (cirscribed) 	X 			X 		 */
				/*		18	 LINE 		 first/[second] 												X 						 */
				/*		19	 POLYGON	 edge/n sides/[side length] 						X 			X 		 */
				/*		20	 CIRCLE 	 center [diameter]											X 						 */
				/*		21	 RECTANG	 first corner [other corner]						X 						 */
				/*		22	 DONUT		 inside dia/outside dia/[cenpt] 				X 						 */
				/*		23	 ARC			 start/radius/[end] 										X 			X 		 */
				/*		24	 ARC			 start/radius/angle/[dir-of-chord]			X  *X 	X 		 */
				/*		25	 ARC			 start/angle/[end]											X 			X 		 */
				/*		26	 ARC			 start/angle/[cenpt]										X 			X 		 */
				/*		27	 LINE 		 start/length/[angle] 									X 			X 		 */
				/*		28	 ARC			 start/end/[second] 										X 	X 	X(**)  */
				/*		29	 ARC			 end/second/[start] 										X 	X 	X(**)  */
				/*		30	 LINE 		 start/end/[mid]												X 	X 				 */
				/*		31	 ARC			 end/center/[end] 											X 	X 				 */
				/*		32	 GETPOINT  first/[second] 	(w/o db_handitem) 		X 						 */
				/*		33	 GETCORNER first corner [other corner]						X 						 */
				/*		34	 ZOOM WIN  first corner [other corner]						X 						 */
				/*		35	 GET POLY  first pt/second pt [third pt]					X 	X 				 */
				/*		36	 VIEWPORT  first pt/second pt [edge] (left/right) X 	X 				 */
				/*		37	 VIEWPORT  first pt/second pt [edge] (top/bottom) X 	X 				 */
				/*		38	 LINE 		 first/second/[length] for LENGTHEN 		X 			X 		 */
				/*																																				 */
				/*									* Set pt2[0]=angle/length 														 */
				/*									(**) set to thickness if dragging extruded edge 																											*/
				/*	RETURN values: RTNORM, RTCAN, or RTERROR. 														 */
				/*																																				 */
				/*	return values can be: RTNONE, RTKWORD, or RTSTR.											 */


			bool bWasException = false;
			SDSThreadException exception;

			sds_point pt3;
				sds_real snapang;
				int ret,bitsets,i,j,xfmode;
				char fs1[IC_ACADBUF];
				int ortho,snapstyl;
				struct resbuf vb,vb2;
				struct SDS_dragvars *tvars;
			db_handitem *hip=NULL;
		pt3[0] = pt3[1] = pt3[2] = 0.;

				tvars=SDS_CMainWindow->m_pDragVarsCur;				
		if((SDS_CMainWindow->m_pDragVarsCur= new struct SDS_dragvars )==NULL) return(RTERROR);
		memset(SDS_CMainWindow->m_pDragVarsCur,0,sizeof(struct SDS_dragvars));
				if(retpt && mode>=0) ic_ptcpy(retpt,pt1);
				SDS_CMainWindow->m_pDragVarsCur->mode=mode;
			ic_ptcpy(SDS_CMainWindow->m_pDragVarsCur->pt1,pt1);

				if(pt2) ic_ptcpy(SDS_CMainWindow->m_pDragVarsCur->pt2,pt2);
				SDS_CMainWindow->m_pDragVarsCur->ang=ang;
				SDS_CMainWindow->m_pDragVarsCur->it=dragit;
				SDS_CMainWindow->m_pDragVarsCur->rblst=NULL;
				SDS_CMainWindow->m_pDragVarsCur->firstdrag=0;
				SDS_CMainWindow->m_pDragVarsCur->breakout=0;
				SDS_CMainWindow->m_pDragVarsCur->cursor=0;
				SDS_CMainWindow->m_pDragVarsCur->numok=0;
				SDS_CMainWindow->m_pDragVarsCur->scnf=SDS_dragobj_cb;

				for(i=0; i<=3; i++)
						for(j=0; j<=3; j++)
								SDS_CMainWindow->m_pDragVarsCur->matx[i][j]=0.0;
				for(i=0; i<=3; i++)
						SDS_CMainWindow->m_pDragVarsCur->matx[i][i]=1.0;

			if(SDS_ApplyNoOrtho) {
					SDS_CMainWindow->m_pDragVarsCur->applyortho=0;
			} else {
				switch(SDS_CMainWindow->m_pDragVarsCur->mode) {
					case 0:
					case 4:
					case 5:
					case 14:
					//case 19://NOTE: ORTHO valid for dragging a square rectang!
					case 21:
					case 22:
					case 28:
					case 29:
					case 33:
					case 34:
						SDS_CMainWindow->m_pDragVarsCur->applyortho=0; break;
					default:
						SDS_CMainWindow->m_pDragVarsCur->applyortho=1;
						break;
				}
			}

				switch(SDS_CMainWindow->m_pDragVarsCur->mode) {
						case -3:
						case -10:
						case 5:
						case 8:
						case 11:
						case 12:
						case 24:
						case 27:
						case 31:
								SDS_CMainWindow->m_pDragVarsCur->enternub=1; break;  // Angles OK
						case -2:
						case -4:
						case -9:
						case 1:
						case 2:
						case 6:
						case 10:
						case 13:
						case 16:
						case 17:
						case 19:
						case 20:
						case 38:
								SDS_CMainWindow->m_pDragVarsCur->enternub=2; break;  // Dist OK
						case -5:
								SDS_CMainWindow->m_pDragVarsCur->enternub=3; break;  // DTEXT entry
						default:
								SDS_CMainWindow->m_pDragVarsCur->enternub=0; break;
				}

			SDS_CMainWindow->m_pDragVarsCur->flp=SDS_CURDWG;

				if(SDS_CMainWindow->m_pDragVarsCur->mode<0 && pt1 && pt2) {

				if(retrb) {
					ic_ptcpy(SDS_CMainWindow->m_pDragVarsCur->pt1,((double *)retrb));
						if(SDS_CMainWindow->m_pDragVarsCur->mode==-6) {
						struct resbuf wcs,ucs;

						/* Set up resbufs for sds_trans(): */
						wcs.rbnext=ucs.rbnext=NULL;
						wcs.restype=RTSHORT; wcs.resval.rint=0;
						ucs.restype=RTSHORT; ucs.resval.rint=1;

						sds_trans(SDS_CMainWindow->m_pDragVarsCur->pt1,&wcs,&ucs,0,SDS_CMainWindow->m_pDragVarsCur->pt1);
					}
				}

				SDS_CMainWindow->m_pDragVarsCur->elp=
					(db_handitem *)(((long *) pt1)[0]);
				SDS_CMainWindow->m_pDragVarsCur->flp=
					(db_drawing *)(((long *) pt1)[1]);

				SDS_CMainWindow->m_pDragVarsCur->elpt=(double *)pt2;
					if(SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_DIMENSION) {
					SDS_CMainWindow->m_pDragVarsCur->applyortho=0;
				}

			}

			if(pt1) {
				if(!SDS_AtAngDist) SDS_CMainWindow->m_pDragVarsCur->numok=1;

				if(mode>=0)
					ic_ptcpy(SDS_PointDisplay,pt1);
				if(SDS_CMainWindow->m_pDragVarsCur->enternub==1)
					SDS_PointDisplayMode=1;
				if(SDS_CMainWindow->m_pDragVarsCur->enternub==2)
					if(SDS_GetZScale) SDS_PointDisplayMode=4;
					else							SDS_PointDisplayMode=2;
				if(SDS_CMainWindow->m_pDragVarsCur->mode==-4) {
					SDS_PointDisplayMode=3;
					if(SDS_CMainWindow->m_pDragVarsCur->elp && SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_INSERT){
						SDS_CMainWindow->m_pDragVarsCur->elp->get_10(SDS_PointDisplay);
					}
				}
				if(SDS_CMainWindow->m_pDragVarsCur->mode==-6 && SDS_CMainWindow->m_pDragVarsCur->elp && SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_LINE){
					SDS_CMainWindow->m_pDragVarsCur->elp->get_10(SDS_PointDisplay);
					SDS_PointDisplayMode=5;
				}

				//Modified SAU 31/05/2000 											 [
				//Reason:Bugfixed(GETPOINT Function, F6 will not toggle to Relative Co-ordinates)
				if(SDS_CMainWindow->m_pDragVarsCur->mode==32)
					SDS_PointDisplayMode=5;
					 	//Modified SAU 31/05/2000						 ]

				if(SDS_CMainWindow->m_pDragVarsCur->mode==-9 && SDS_CMainWindow->m_pDragVarsCur->elp && SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_ELLIPSE){
					SDS_CMainWindow->m_pDragVarsCur->elp->get_10(SDS_PointDisplay);
				}
			}

			if(mode==21 && pt1) {
				sds_polar(pt1,IC_PI/4.0,1.0,SDS_CMainWindow->m_pDragVarsCur->LastDrag);
			}

			// Set up the ortho point in whatever coords entity is in.	We'll transform it
			//	later because ic_ortho needs a ucs point to compare to when dragging in ucs.
			if(mode==30){
						//sds_polar(pt1,sds_angle(pt1,pt2),sds_distance(pt1,pt2)/2.0,SDS_CMainWindow->m_pDragVarsCur->OrthoPt);
				SDS_CMainWindow->m_pDragVarsCur->OrthoPt[0]=0.5*(pt1[0]+pt2[0]);
				SDS_CMainWindow->m_pDragVarsCur->OrthoPt[1]=0.5*(pt1[1]+pt2[1]);
				SDS_CMainWindow->m_pDragVarsCur->OrthoPt[2]=0.5*(pt1[2]+pt2[2]);
			}else if(mode>=0) {
				ic_ptcpy(SDS_CMainWindow->m_pDragVarsCur->OrthoPt,pt1);
			}else if(SDS_CMainWindow->m_pDragVarsCur->mode==-1 &&
					SDS_CMainWindow->m_pDragVarsCur->elp &&
					SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_LINE) {
		//		if(SDS_CMainWindow->m_fIsCtrlDown) {
		//			if((double *)SDS_CMainWindow->m_pDragVarsCur->elp->retp_10()==(double *)(SDS_CMainWindow->m_pDragVarsCur->elpt))
		//				SDS_CMainWindow->m_pDragVarsCur->elp->get_11(SDS_CMainWindow->m_pDragVarsCur->OrthoPt);
		//			else
		//				SDS_CMainWindow->m_pDragVarsCur->elp->get_10(SDS_CMainWindow->m_pDragVarsCur->OrthoPt);
		//		} else {
					ic_ptcpy(SDS_CMainWindow->m_pDragVarsCur->OrthoPt,(double *)(SDS_CMainWindow->m_pDragVarsCur->elpt));
		//		}
			}else if((SDS_CMainWindow->m_pDragVarsCur->mode==-1 || SDS_CMainWindow->m_pDragVarsCur->mode==-7) &&
					SDS_CMainWindow->m_pDragVarsCur->elp &&
					(SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_RAY || SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_XLINE)) {
				//if you're dragging a ray or xline, put the 10pt in the ortho point, not the point you got passed (the 11)
				SDS_CMainWindow->m_pDragVarsCur->elp->get_10(SDS_CMainWindow->m_pDragVarsCur->OrthoPt);
			} else if(SDS_CMainWindow->m_pDragVarsCur->mode==-7 && SDS_CMainWindow->m_pDragVarsCur->elp){
				SDS_CMainWindow->m_pDragVarsCur->elp->get_10(SDS_CMainWindow->m_pDragVarsCur->OrthoPt);
			} else if(SDS_CMainWindow->m_pDragVarsCur->mode<0) {
					if(retrb) ic_ptcpy(SDS_CMainWindow->m_pDragVarsCur->OrthoPt,((double *)retrb));
				else ic_ptcpy(SDS_CMainWindow->m_pDragVarsCur->OrthoPt,((double *)SDS_CMainWindow->m_pDragVarsCur->elpt));

			} else
				ic_ptcpy(SDS_CMainWindow->m_pDragVarsCur->OrthoPt,pt2);

			// added by rdf as a remedy for bug 1-58954
			if (SDS_CMainWindow->m_pDragVarsCur->mode==9)
				{
				ic_ptcpy(SDS_CMainWindow->m_pDragVarsCur->OrthoPt,pt2);
				}

			//if dragging extruded edge, modify ortho point by thickness vector & scale
			//	because ortho pt is in native coords for entity, we don't need the extrusion vector
			//	for entities except LINE and POINT
			if(SDS_CMainWindow->m_pDragVarsCur->mode==-1 && fabs(ang)>IC_ZRO && SDS_CMainWindow->m_pDragVarsCur->elp){
				if(SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_LINE){
					sds_point extru;
					SDS_CMainWindow->m_pDragVarsCur->elp->get_210(extru);
					SDS_CMainWindow->m_pDragVarsCur->OrthoPt[0]+=extru[0]*ang;
					SDS_CMainWindow->m_pDragVarsCur->OrthoPt[1]+=extru[1]*ang;
					SDS_CMainWindow->m_pDragVarsCur->OrthoPt[2]+=extru[2]*ang;
				}else SDS_CMainWindow->m_pDragVarsCur->OrthoPt[2]+=ang;
			}


			// Set up the UCS to native ent coords.
			xfmode=1;
				switch(SDS_CMainWindow->m_pDragVarsCur->mode) {
				case -1:
				case -6:
					if(SDS_CMainWindow->m_pDragVarsCur->elp &&
			 				 (SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_LINE ||
			 					SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_POINT ||
			 					SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_XLINE ||
			 					SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_RAY ||
			 					SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_ELLIPSE ||
			 					SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_TOLERANCE ||
			 					SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_LEADER ||
			 					SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_MLINE ||
			 					SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_SPLINE ||
			 					SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_MTEXT ||
			 					SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_VIEWPORT ||
			 					SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_DIMENSION ||
						SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_3DFACE))
						xfmode=0;
					else if(SDS_CMainWindow->m_pDragVarsCur->elp && SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_POLYLINE){
						SDS_CMainWindow->m_pDragVarsCur->elp->get_70(&xfmode);
						if(0==(xfmode&(IC_PLINE_3DPLINE | IC_PLINE_3DMESH | IC_PLINE_POLYFACEMESH)))xfmode=1;//2d poly
						else xfmode=0;//3d poly
					}
					break;
				case -7:
				case 2:
						case 18:
						case 27:
						case 30:
				case 36://viewports ALWAYS wcs
				case 37:
				case 38:
								xfmode=0; break;
				}
			if(SDS_CMainWindow->m_pDragVarsCur->mode<0 && SDS_CMainWindow->m_pDragVarsCur->elp) {
				SDS_CMainWindow->m_pDragVarsCur->elp->get_210(SDS_CMainWindow->m_pDragVarsCur->extru);
			} else {
				SDS_CMainWindow->m_pDragVarsCur->extru[0]=0.0;
				SDS_CMainWindow->m_pDragVarsCur->extru[1]=0.0;
				SDS_CMainWindow->m_pDragVarsCur->extru[2]=0.0;
			}
			SDS_CURDWG->initucs2ent(SDS_CMainWindow->m_pDragVarsCur->xfa,
								 SDS_CMainWindow->m_pDragVarsCur->extru,
											 &SDS_CMainWindow->m_pDragVarsCur->dang,xfmode);

			//Setup resbufs to transform ortho point for use in dragging.  ic_ortho needs
			//	ucs point as comparison basis
			vb.restype=RTSHORT;
			vb.resval.rint=1;
			if(0!=xfmode){
				vb2.restype=RT3DPOINT;
				ic_ptcpy(vb2.resval.rpoint,SDS_CMainWindow->m_pDragVarsCur->extru);
			}else{
				vb2.restype=RTSHORT;
				vb2.resval.rint=0;
			}
		// EBATECH(Futagami.Y)-[17/05/2002 for draw POLYGON with Edge option.
			//if(mode!=1 && mode!=32 && mode!=12 && mode!=35 && mode!=16)
			if(mode!=1 && mode!=32 && mode!=12 && mode!=35 && mode!=16 && mode!=17 && mode!=19)
		// 17/05/2002]-EBATECH(Futagami.Y)
				sds_trans(SDS_CMainWindow->m_pDragVarsCur->OrthoPt,&vb2,&vb,0,SDS_CMainWindow->m_pDragVarsCur->OrthoPt);


			// Create an elp for dragging.
				if(!( SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_DASH )) {
				db_handitem *elp;
				SDS_getvar(NULL,DB_QTHICKNESS,&vb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				switch(SDS_CMainWindow->m_pDragVarsCur->mode) {
					case 2 :
					case 18:
					case 27:
					case 30:
						// LINE
						SDS_CMainWindow->m_pDragVarsCur->elp=new db_line(SDS_CURDWG);
						SDS_CMainWindow->m_pDragVarsCur->elp->set_210(SDS_CMainWindow->m_pDragVarsCur->extru);
						SDS_CMainWindow->m_pDragVarsCur->elp->set_10((double *)pt1);
						SDS_CMainWindow->m_pDragVarsCur->elp->set_11((double *)pt1);
						elp=SDS_CMainWindow->m_pDragVarsCur->elp;
						break;
					case 1 :
					case 3 :
					case 4 :
					case 20:
						// CIRCLE
						SDS_CMainWindow->m_pDragVarsCur->elp=new db_circle(SDS_CURDWG);
						SDS_CMainWindow->m_pDragVarsCur->elp->set_210(SDS_CMainWindow->m_pDragVarsCur->extru);
						SDS_CMainWindow->m_pDragVarsCur->elp->set_10((double *)pt1);
							SDS_getvar(NULL,DB_QCIRCLERAD,&vb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						SDS_CMainWindow->m_pDragVarsCur->elp->set_40(vb.resval.rreal);
						elp=SDS_CMainWindow->m_pDragVarsCur->elp;
						break;
					case 0 :
					case 5 :
					case 6 :
					case 7 :
					case 8 :
					case 9 :
					case 10:
					case 11:
					case 12:
					case 13:
					case 14:
					case 15:
					case 23:
					case 24:
					case 25:
					case 26:
					case 28:
					case 29:
					case 31:
						// ARC
						SDS_CMainWindow->m_pDragVarsCur->elp=new db_arc(SDS_CURDWG);
						SDS_CMainWindow->m_pDragVarsCur->elp->set_210(SDS_CMainWindow->m_pDragVarsCur->extru);
						SDS_CMainWindow->m_pDragVarsCur->elp->set_10((double *)pt1);
						elp=SDS_CMainWindow->m_pDragVarsCur->elp;
						break;
					case 16: // POLYGON
					case 17: // POLYGON
					case 19: // POLYGON
					case 21: // RECTANG
					case 22: // DONUT
						SDS_getvar(NULL,DB_QCECOLOR,&vb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						SDS_CMainWindow->m_pDragVarsCur->color=ic_colornum(vb.resval.rstring);
						IC_FREE(vb.resval.rstring);
						if(SDS_CMainWindow->m_pDragVarsCur->color==DB_BLCOLOR)	{
							hip=SDS_CURDWG->ret_currenttabrec(DB_QCLAYER);
							hip->get_62(&SDS_CMainWindow->m_pDragVarsCur->color);
						}
						break;
				}
			}
				if( SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_DASH) {
						vb.restype=RTSHORT;
						vb.resval.rint=1;
						sds_setvar("HLDOTTED"/*DNT*/,&vb);
				}

				SDS_getvar(NULL,DB_QVIEWSIZE,&vb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				SDS_CMainWindow->m_pDragVarsCur->scrht=vb.resval.rreal;

			int iTempBits = SDSApplication::GetActiveApplication()->GetInputControlBits();
				SDSApplication::GetActiveApplication()->SetInputControlBits( iTempBits | SDS_RSG_NODOCCHG );

				if(SDS_CMainWindow->m_pDragVarsCur->enternub || SDS_CMainWindow->m_pDragVarsCur->numok) {
						bitsets=SDSApplication::GetActiveApplication()->GetInputControlBits();
						SDSApplication::GetActiveApplication()->SetInputControlBits( bitsets | SDS_RSG_OTHER );
				}
				SDS_CMainWindow->m_pDragVarsCur->gotnub=0;
				if(SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_DASH)
				SDS_CMainWindow->m_pDragVarsCur->dashmode=1;

			moredrag: ;

			int savebits=SDSApplication::GetActiveApplication()->GetInputControlBits();
			char savkwords[IC_ACADBUF];
			strncpy(savkwords,SDSApplication::GetActiveApplication()->GetKeywords(),sizeof(savkwords)-1);

				SDS_IsDragging=1;

//4M Spiros Item:27->
	 if (!SDS_GripPlaced)
	 {
//4M Spiros Item:27<-
				// I added this try-catch block to avoid keeping allocated memory for m_pDragVarsCur and
			// to clear SDS_IsDragging flag
			try
			{
//			// Modified CyberAge VSB 20/05/2001
//			bGetPoint = TRUE;	//Used for Flyover Snapping
				ret=SDS_AskForPoint( NULL,prompt,pt3 );
//			bGetPoint = FALSE;	//Used for Flyover Snapping
			}
			catch( SDSThreadException& theException )
			{
//			bGetPoint = FALSE; //Used for Flyover Snapping
				bWasException = true;
				exception = theException;
			}


//4M Spiros Item:27->
	 ic_ptcpy(SDS_GripLast,pt3);
	 }
	 else
	 {
		 ret=RTNORM;
		 ic_ptcpy(pt3,SDS_GripLast);
	 }
//	 DoSnap=0; <-Item:97
//4M Spiros Item:27<-
				SDS_IsDragging=0;

	//Modified Cybage AW 03-12-01 [
	//Reason : Global variables "bNotAPoint" and "strAngleScale" are not used anymore.
	/*
	//Modified Cybage AW 02-07-01 [
	//Reason : The dragmode OFF or ON affect the commands like rotate/Scale
	bNotAPoint = FALSE;
	strAngleScale[0] = '\0';
	*/
	*fs1 = 0; // EBATECH(ARA)
	//Modified Cybage AW 02-07-01 ]
	//Modified Cybage AW 03-12-01 ]
				if(kword && (ret==RTKWORD || ret==RTSTR)) if(sds_getinput(kword)!=RTNORM) *kword=0;
				if(SDS_CMainWindow->m_pDragVarsCur->enternub || SDS_CMainWindow->m_pDragVarsCur->numok) {
						if(ret==RTSTR || ret==RTKWORD) {
						if(SDS_CMainWindow->m_pDragVarsCur->mode==-5) {
						ret=RTNORM;
					} else {
						sds_getinput(fs1);
				//Modified Cybage AW 03-12-01 [
				//Reason : Global variables "bNotAPoint" and "strAngleScale" are not used anymore.
				/*
				//Modified Cybage AW 02-07-01 [
				//Reason : The dragmode OFF or ON affect the commands like rotate/Scale
				bNotAPoint = TRUE;
				strcpy(strAngleScale, fs1);
				//Modified Cybage AW 02-07-01 ]
				*/
				//Modified Cybage AW 03-12-01 ]

						// Fix commented out by PP 09/22/2000. Reason: This fix is incorrect.
						/* Modified PK 25/04/2000
						if(ret==RTKWORD && bitsets&SDS_RSG_OTHER) // Reason: Fix for bug no. 57514
							return ret; */
						if(*fs1==0 &&
							bitsets&SDS_RSG_NONULL) {
							SDS_CMainWindow->m_pDragVarsCur->gotnub=0;
							SDSApplication::GetActiveApplication()->SetInputControlBits( savebits );
												SDSApplication::GetActiveApplication()->SetKeywords(savkwords);
		//MHB					strncpy(SDSApplication::GetActiveApplication()->keywords,savkwords,sizeof(SDSApplication::GetActiveApplication()->keywords)-1);
							cmd_ErrorPrompt(CMD_ERR_NUMERIC,0);
												goto moredrag;
										}

						if(*fs1=='-' || *fs1=='.' || *fs1=='<' || sds_isdigit((unsigned char) *fs1)) {

							if(SDS_CMainWindow->m_pDragVarsCur->enternub==2) {
								SDS_CMainWindow->m_pDragVarsCur->gotnub=1;
								sds_distof(fs1,-1,&SDS_CMainWindow->m_pDragVarsCur->nub);
							}
									if(SDS_CMainWindow->m_pDragVarsCur->enternub==1) {
								SDS_CMainWindow->m_pDragVarsCur->gotnub=1;
								char *fcp1;
								int angct;
								//*** 					< 	- Use Current angular units and Current angle base and direction.
														//*** 					<<	- Use Degrees angular units and Default (east,ccw) angle base
														//*** 								and direction.
														//*** 					<<< - Use Current angular units and Default (east,ccw) angle base
														//*** 								and direction.

														if(*fs1=='<') {
																for(fcp1=fs1,angct=0; *fcp1=='<'; fcp1++,angct++);
																strncpy(fs1,fcp1,sizeof(fs1)-1);
									if(angct==1)
										sds_angtof(fs1,-1,&SDS_CMainWindow->m_pDragVarsCur->nub);
									else if(angct==2)
										sds_angtof(fs1,0,&SDS_CMainWindow->m_pDragVarsCur->nub);
									else if(angct==3) {
										sds_angtof(fs1,-1,&SDS_CMainWindow->m_pDragVarsCur->nub);
										
									SDS_getvar(NULL,DB_QANGBASE,&vb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
								#ifndef OLD_ANGBASE
									{ // support CW direction was added
									resbuf _rb; 
									SDS_getvar(NULL,DB_QANGDIR,&_rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
									ASSERT(vb.resval.rreal >= 0);
									if(_rb.resval.rint) 
										vb.resval.rreal = IC_TWOPI - vb.resval.rreal;
									}
								#endif

										*&SDS_CMainWindow->m_pDragVarsCur->nub+=vb.resval.rreal;
									}
														} else {
									sds_angtof(fs1,-1,&SDS_CMainWindow->m_pDragVarsCur->nub);
								}
								if(*fs1=='-') SDS_CMainWindow->m_pDragVarsCur->nub-=IC_TWOPI;
							}
					if(icadRealEqual(SDS_CMainWindow->m_pDragVarsCur->nub,0.0) && bitsets&SDS_RSG_NOZERO) {
								cmd_ErrorPrompt(CMD_ERR_NONZERO,0);
								SDS_CMainWindow->m_pDragVarsCur->gotnub=0;
								SDSApplication::GetActiveApplication()->SetInputControlBits( savebits );
														SDSApplication::GetActiveApplication()->SetKeywords(savkwords);
		//MHB						strncpy(SDSApplication::GetActiveApplication()->keywords,savkwords,sizeof(SDSApplication::GetActiveApplication()->keywords)-1);
														goto moredrag;
												}
												if(SDS_CMainWindow->m_pDragVarsCur->nub<0.0 && bitsets&SDS_RSG_NONEG) {
								if(bitsets&SDS_RSG_NOZERO){
																cmd_ErrorPrompt(CMD_ERR_POSITIVE,0);
														}else{
																cmd_ErrorPrompt(CMD_ERR_POSORZERO,0);
														}
								SDS_CMainWindow->m_pDragVarsCur->gotnub=0;
								SDSApplication::GetActiveApplication()->SetInputControlBits( savebits );
														SDSApplication::GetActiveApplication()->SetKeywords(savkwords);
		//MHB						strncpy(SDSApplication::GetActiveApplication()->keywords,savkwords,sizeof(SDSApplication::GetActiveApplication()->keywords)-1);
														goto moredrag;
												}
												// Bug fix #5003 and #5789. Removed negation of second argument to allow direct distance input.
												// Note direct distance input was broken in version 1 and will be in FCS_GOLD.
					if(SDS_CMainWindow->m_pDragVarsCur->enternub==0 && SDS_CMainWindow->m_pDragVarsCur->numok && SDS_CMainWindow->m_pDragVarsCur->enternub!=1)
					{
						sds_distof(fs1,-1,&SDS_CMainWindow->m_pDragVarsCur->nub);
						if(!icadRealEqual(SDS_CMainWindow->m_pDragVarsCur->nub,0.0))
						{
							if(SDS_CMainWindow->m_pDragVarsCur->firstdrag)
							{
								ExecuteUIAction( ICAD_WNDACTION_LASTDRAG );
								SDS_BitBlt2Scr(1);
								SDS_CMainWindow->m_pDragVarsCur->firstdrag=0;
							}

							// DDE - Direct Distance Entering
							/*D.G.*/// Use fly-over highlighted point instead of the actual cursor position.
							((CEngineApplication*)SDSApplication::GetEngineApplication())->getOsnapManager()->setPoint();
							((CEngineApplication*)SDSApplication::GetEngineApplication())->getOsnapManager()->getPoint(SDS_CMainWindow->m_pDragVarsCur->LastDrag);
							sds_point	p2;
							double		fr2;
							sds_point	midPt;
							/*D.G.*/// Using SDS_CMainWindow->m_pDragVarsCur->pt2 in the case of mode == -1.
							// Use (pt1+pt2)/2 in case of mode == 30.
							if(mode == -1)
							{
								p2[0] = SDS_CMainWindow->m_pDragVarsCur->LastDrag[0] - SDS_CMainWindow->m_pDragVarsCur->pt2[0];
								p2[1] = SDS_CMainWindow->m_pDragVarsCur->LastDrag[1] - SDS_CMainWindow->m_pDragVarsCur->pt2[1];
								p2[2] = SDS_CMainWindow->m_pDragVarsCur->LastDrag[2] - SDS_CMainWindow->m_pDragVarsCur->pt2[2];
							}
							else
								if(mode == 30)
								{
									midPt[0] = (SDS_CMainWindow->m_pDragVarsCur->pt1[0] + SDS_CMainWindow->m_pDragVarsCur->pt2[0]) * 0.5;
									midPt[1] = (SDS_CMainWindow->m_pDragVarsCur->pt1[1] + SDS_CMainWindow->m_pDragVarsCur->pt2[1]) * 0.5;
									midPt[2] = (SDS_CMainWindow->m_pDragVarsCur->pt1[2] + SDS_CMainWindow->m_pDragVarsCur->pt2[2]) * 0.5;

									p2[0] = SDS_CMainWindow->m_pDragVarsCur->LastDrag[0] - midPt[0];
									p2[1] = SDS_CMainWindow->m_pDragVarsCur->LastDrag[1] - midPt[1];
									p2[2] = SDS_CMainWindow->m_pDragVarsCur->LastDrag[2] - midPt[2];
								}
								else
								{
									p2[0] = SDS_CMainWindow->m_pDragVarsCur->LastDrag[0] - SDS_CMainWindow->m_pDragVarsCur->pt1[0];
									p2[1] = SDS_CMainWindow->m_pDragVarsCur->LastDrag[1] - SDS_CMainWindow->m_pDragVarsCur->pt1[1];
									p2[2] = SDS_CMainWindow->m_pDragVarsCur->LastDrag[2] - SDS_CMainWindow->m_pDragVarsCur->pt1[2];
								}
							fr2 = sqrt(p2[0] * p2[0] + p2[1] * p2[1] + p2[2] * p2[2]);
							if(icadRealEqual(fr2,0.0))
							{
								p2[0] = 0.0;
								p2[1] = 1.0;
								p2[2] = 0.0;
								fr2 = 1.0;
							}
							if(mode == -1)
							{
								pt3[0] = SDS_CMainWindow->m_pDragVarsCur->pt2[0] + SDS_CMainWindow->m_pDragVarsCur->nub * p2[0] / fr2;
								pt3[1] = SDS_CMainWindow->m_pDragVarsCur->pt2[1] + SDS_CMainWindow->m_pDragVarsCur->nub * p2[1] / fr2;
								pt3[2] = SDS_CMainWindow->m_pDragVarsCur->pt2[2] + SDS_CMainWindow->m_pDragVarsCur->nub * p2[2] / fr2;
							}
							else
								if(mode == 30)
								{
									pt3[0] = midPt[0] + SDS_CMainWindow->m_pDragVarsCur->nub * p2[0] / fr2;
									pt3[1] = midPt[1] + SDS_CMainWindow->m_pDragVarsCur->nub * p2[1] / fr2;
									pt3[2] = midPt[2] + SDS_CMainWindow->m_pDragVarsCur->nub * p2[2] / fr2;
								}
								else
								{
									pt3[0] = SDS_CMainWindow->m_pDragVarsCur->pt1[0] + SDS_CMainWindow->m_pDragVarsCur->nub * p2[0] / fr2;
									pt3[1] = SDS_CMainWindow->m_pDragVarsCur->pt1[1] + SDS_CMainWindow->m_pDragVarsCur->nub * p2[1] / fr2;
									pt3[2] = SDS_CMainWindow->m_pDragVarsCur->pt1[2] + SDS_CMainWindow->m_pDragVarsCur->nub * p2[2] / fr2;
								}
							ret = RTNORM;
						}
					}

						ret=RTNORM;
				}
			}
		}
				SDSApplication::GetActiveApplication()->SetInputControlBits( bitsets );
			}

			SDS_getvar(NULL,DB_QORTHOMODE,&vb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				ortho=vb.resval.rint;
			SDS_getvar(NULL,DB_QSNAPSTYL,&vb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				if(0==vb.resval.rint)
				snapstyl=-1;
			else{
				SDS_getvar(NULL,DB_QSNAPISOPAIR,&vb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				snapstyl=vb.resval.rint;
			}

			SDS_getvar(NULL,DB_QSNAPANG,&vb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				snapang=vb.resval.rreal;


				if(SDS_CMainWindow->m_pDragVarsCur->firstdrag) {
				ExecuteUIAction( ICAD_WNDACTION_LASTDRAG );
				SDS_BitBlt2Scr(1);
						if(retpt) {
					if(ret==RTNORM) {
						ic_ptcpy(retpt,pt3);
					} else {
						ic_ptcpy(retpt,SDS_CMainWindow->m_pDragVarsCur->pt3);
					}
				}
				} else {
						if(retpt && ret==RTNORM) ic_ptcpy(retpt,pt3);
				}

				//EBATECH(FUTA)
				//if(!SDS_SkipLastOrthoSet && retpt && ((ortho && !SDS_CMainWindow->m_fIsShiftDown) || (!ortho && SDS_CMainWindow->m_fIsShiftDown)) && SDS_CMainWindow->m_pDragVarsCur->applyortho) {
				if(*fs1==0 && !SDS_SkipLastOrthoSet && retpt && ((ortho && !SDS_CMainWindow->m_fIsShiftDown) || (!ortho && SDS_CMainWindow->m_fIsShiftDown)) && SDS_CMainWindow->m_pDragVarsCur->applyortho) { // Modify 2001/11/9
		 		//** 2/25/98	changing z to retpt's z so osnap won't adjust it
//4M Item:58->
/*
				if( mode != -6 ) // Suppress changing Ortho point in case of drawing line BRX CR1071
*/
// When Ortho is On sds_getpoint returns a point that has lost its elevation relative to its
// base point
		if ((mode != -6) && (mode!=32))
//<-4M Item:58
					SDS_CMainWindow->m_pDragVarsCur->OrthoPt[2]=retpt[2];
						ic_ortho(SDS_CMainWindow->m_pDragVarsCur->OrthoPt,snapang,retpt,retpt,snapstyl);
			 	vb.restype=RT3DPOINT;
				ic_ptcpy(vb.resval.rpoint,retpt);
				SDS_setvar(NULL,DB_QLASTPOINT,&vb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
			}

			SDS_SkipLastOrthoSet=0;

				if(retrb && retpt && SDS_CMainWindow->m_pDragVarsCur->mode>=0) {
						SDS_dragone(retpt,2,dragit,ortho,snapang,snapstyl);
				if(icadRealEqual(SDS_CMainWindow->m_pDragVarsCur->extru[0],0.0) &&
						icadRealEqual(SDS_CMainWindow->m_pDragVarsCur->extru[1],0.0) &&
						 icadRealEqual(SDS_CMainWindow->m_pDragVarsCur->extru[2],1.0)) {
					*retrb=SDS_CMainWindow->m_pDragVarsCur->rblst;
				} else {
					*retrb=sds_buildlist(210,SDS_CMainWindow->m_pDragVarsCur->extru,0);
					(*retrb)->rbnext=SDS_CMainWindow->m_pDragVarsCur->rblst;
				}
					SDS_BitBlt2Scr(1);
				}

				if((SDS_CMainWindow->m_pDragVarsCur->mode==16 || SDS_CMainWindow->m_pDragVarsCur->mode==17) && retpt && SDS_CMainWindow->m_pDragVarsCur->gotnub)	{
						sds_polar(SDS_CMainWindow->m_pDragVarsCur->pt1,0.0,SDS_CMainWindow->m_pDragVarsCur->nub,retpt);
				}

				if(SDS_CMainWindow->m_pDragVarsCur->mode==-1)  {
				ic_ptcpy(SDS_CMainWindow->m_pDragVarsCur->elpt,retpt ? retpt : pt3);
				SDS_CURDWG->ucs2ent(SDS_CMainWindow->m_pDragVarsCur->elpt,
										SDS_CMainWindow->m_pDragVarsCur->elpt,
													SDS_CMainWindow->m_pDragVarsCur->xfa);
				}

				if((SDS_CMainWindow->m_pDragVarsCur->mode==-3 || SDS_CMainWindow->m_pDragVarsCur->mode==-10) && retpt)	{
				if(SDS_CMainWindow->m_pDragVarsCur->gotnub)	{
					retpt[0]=SDS_CMainWindow->m_pDragVarsCur->nub;
				}else{
					retpt[0]=sds_angle(SDS_CMainWindow->m_pDragVarsCur->pt1,retpt);
				}
				}

				if(SDS_CMainWindow->m_pDragVarsCur->mode==-2 && retpt)	{
				if(SDS_CMainWindow->m_pDragVarsCur->gotnub) {
					retpt[0]=SDS_CMainWindow->m_pDragVarsCur->nub;
				} else {
					retpt[0]=sds_distance(SDS_CMainWindow->m_pDragVarsCur->pt1,retpt);
				}
				if(SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_ELLIPSE){
					//divide dist by semimajor axis vector's length to get new ratio
					sds_point smavect;
					SDS_CMainWindow->m_pDragVarsCur->elp->get_11(smavect);
					retpt[0]/=sqrt((smavect[0]*smavect[0])+(smavect[1]*smavect[1])+(smavect[2]*smavect[2]));
				}
			}
				if(SDS_CMainWindow->m_pDragVarsCur->mode==-9 && retpt && SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_ELLIPSE) {
				sds_point smavect;
				SDS_CMainWindow->m_pDragVarsCur->elp->get_11(smavect);

				if(SDS_CMainWindow->m_pDragVarsCur->gotnub) {
					retpt[0]=SDS_CMainWindow->m_pDragVarsCur->nub/
								sqrt(smavect[0]*smavect[0]+
										 smavect[1]*smavect[1]+
										 smavect[2]*smavect[2]);
				} else {
					retpt[0]=sds_distance(SDS_CMainWindow->m_pDragVarsCur->pt1,retpt)/
								sqrt(smavect[0]*smavect[0]+
										 smavect[1]*smavect[1]+
										 smavect[2]*smavect[2]);
				}
				}

				if(SDS_CMainWindow->m_pDragVarsCur->mode==-4 && retpt)	{
				if(SDS_CMainWindow->m_pDragVarsCur->gotnub) {
					retpt[0]=SDS_CMainWindow->m_pDragVarsCur->nub;
					retpt[1]=SDS_CMainWindow->m_pDragVarsCur->nub;
					retpt[2]=SDS_CMainWindow->m_pDragVarsCur->nub;
					ret=RTSTR;
				} else {
					retpt[0]=retpt[0]-SDS_CMainWindow->m_pDragVarsCur->pt1[0];
					retpt[1]=retpt[1]-SDS_CMainWindow->m_pDragVarsCur->pt1[1];
					retpt[2]=retpt[2]-SDS_CMainWindow->m_pDragVarsCur->pt1[2];
				}
			}

				if(SDS_CMainWindow->m_pDragVarsCur->mode>=0 && SDS_CMainWindow->m_pDragVarsCur->elp)	{
				delete SDS_CMainWindow->m_pDragVarsCur->elp;
				SDS_CMainWindow->m_pDragVarsCur->elp=NULL;
			}

			IC_FREE(SDS_CMainWindow->m_pDragVarsCur);
				SDS_CMainWindow->m_pDragVarsCur=tvars;

				if(SDSApplication::GetActiveApplication()->GetInputControlBits() & SDS_RSG_DASH) {
						vb.restype=RTSHORT;
						vb.resval.rint=0;
						sds_setvar("HLDOTTED"/*DNT*/,&vb);
				}
			SDS_PointDisplayMode=0;

				// Pass the exception forward after finishing current job
			if( bWasException )
				throw exception;

			return(ret);
		}
		#pragma optimize( "", on )
//	#include "framebuffer.h"/*DNT*/
		int SDS_dragobj_cb(sds_point pt3, sds_matrix mt) {

				static sds_real snapang;
				static int ortho,snapstyl;
				sds_point opt;
				int fi1,ret,ret2;
				static int transp=1;
			struct resbuf rb;

				ret=RTNORM;

				if(!SDS_CMainWindow->m_pDragVarsCur->firstdrag || !mt) {
			 		SDS_getvar(NULL,DB_QORTHOMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				ortho=rb.resval.rint;
			 		SDS_getvar(NULL,DB_QSNAPANG,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						snapang=rb.resval.rreal;
			 		SDS_getvar(NULL,DB_QSNAPSTYL,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				if(0==rb.resval.rint)
					snapstyl=-1;
				else{
				 		SDS_getvar(NULL,DB_QSNAPISOPAIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					snapstyl=rb.resval.rint;
				}
				}

				if(!SDS_CMainWindow->m_pDragVarsCur->firstdrag &&
				 SDS_CMainWindow->m_pDragVarsCur->mode!=-5) {
						if(((ortho && !SDS_CMainWindow->m_fIsShiftDown) || (!ortho && SDS_CMainWindow->m_fIsShiftDown))
					 && SDS_CMainWindow->m_pDragVarsCur->applyortho)
								ic_ortho(SDS_CMainWindow->m_pDragVarsCur->OrthoPt,snapang,pt3,pt3,snapstyl);

						ic_ptcpy(SDS_CMainWindow->m_pDragVarsCur->pt3,pt3);
						SDS_dragone(pt3,0,SDS_CMainWindow->m_pDragVarsCur->it,ortho,snapang,snapstyl);
				SDS_BitBlt2Scr(1);
						SDS_CMainWindow->m_pDragVarsCur->firstdrag=1;
						return(RTNORM);
				}

				// This is a mode that will only be used by our transparent draggen.
				if(!mt) {
						if(transp) transp=0; else transp=1;
						ret2=SDS_dragone(pt3,0,SDS_CMainWindow->m_pDragVarsCur->it,ortho,snapang,snapstyl);
				SDS_BitBlt2Scr(1);
				return(ret2);
				}

				for(fi1=0; fi1<2; ++fi1) {
				ret=SDS_dragone(pt3,fi1,SDS_CMainWindow->m_pDragVarsCur->it,ortho,snapang,snapstyl);
				if(SDS_CMainWindow->m_pDragVarsCur->mode==-5) break;
			}
			SDS_BitBlt2Scr(1);
//		// Modified CyberAge VSB 04/06/2001 [
//		// Checking whether to show snap or not..
//		// Bug 77792 from BugZilla
//		sds_getvar( "OSMODE", &rb );
//		int osmode = rb.resval.rint;
//
//		sds_getvar( "AUTOSNAP", &rb ) ;
//		int autosnap	 = rb.resval.rint ;
//
//		if( ( ( bGetPoint || SDS_PointDisplayMode )&& !SDS_BoxModeCursor && !(osmode & IC_OSMODE_OFF ) && autosnap) )
//		{
//			// Modified CyberAge VSB 20/05/2001 [
//			// Draws snap while dragging.
//			CIcadView *pView = SDS_CURVIEW;
//			CIcadDoc	*pDoc	 = SDS_CURDOC;
//
//			POSITION pos=pDoc->GetFirstViewPosition();
//			while (pos!=NULL)
//			{
//				if(Snap_DrawInOneWindow)
//				{
//					pView=SDS_CURVIEW;
//				}
//				else
//				{
//					pView=(CIcadView *)pDoc->GetNextView(pos);
//				}
//
//				if(pView==NULL)
//				{
//					return 0;
//				}
//
//
//				ASSERT_KINDOF(CIcadView, pView);
//
//				ASSERT( !pView->IsPrinting() );
//
//				pView->SnapObject.m_gr_view  =pView->m_pCurDwgView;
//				if ( SnapStructLastSnap.bValidSnap )
//				{
//					pView->m_iFlyOverSnapX= pView->SnapObject.m_LastSnapPtPixX;
//					pView->m_iFlyOverSnapY= pView->SnapObject.m_LastSnapPtPixY;
//
//				}
//				if ( SnapStructLastSnap.bValidSnap )
//				{
//					int nWidth = 30;
//					pView->m_RectBitBlt = 1;
//					pView->m_RectBitBltPt[0].x = pView->m_iFlyOverSnapX - nWidth;
//					pView->m_RectBitBltPt[0].y = pView->m_iFlyOverSnapY - nWidth;
//					pView->m_RectBitBltPt[1].x = pView->m_iFlyOverSnapX + nWidth;
//					pView->m_RectBitBltPt[1].y = pView->m_iFlyOverSnapY + nWidth;
//
//					pView->SnapObject.pDC =  pView->GetDC();
//					int	rop = ::SetROP2( pView->SnapObject.pDC->m_hDC,R2_XORPEN);
//					pView->SnapObject.DrawSnap(SnapStructLastSnap,1);
//					pView->m_nFlyOverSnapDrawn	= 1;
//					pView->m_iFlyOverSnapX = pView->SnapObject.m_LastSnapPtPixX;
//					pView->m_iFlyOverSnapY = pView->SnapObject.m_LastSnapPtPixY;
//					::SetROP2( pView->SnapObject.pDC->m_hDC,rop);
//					pView->ReleaseDC( pView->SnapObject.pDC );
//
//				}
//				else
//				{
//					if ( pView->m_nFlyOverSnapDrawn )
//					{
//						int nWidth = 30;
//						pView->m_RectBitBlt = 1;
//						pView->m_RectBitBltPt[0].x = pView->m_iFlyOverSnapX - nWidth;
//						pView->m_RectBitBltPt[0].y = pView->m_iFlyOverSnapY - nWidth;
//						pView->m_RectBitBltPt[1].x = pView->m_iFlyOverSnapX + nWidth;
//						pView->m_RectBitBltPt[1].y = pView->m_iFlyOverSnapY + nWidth;
//						pView->m_nFlyOverSnapDrawn = 0;
//					}
//
//				}
//
//				if(Snap_DrawInOneWindow)
//					break;
//			}
//			// Modified CyberAge VSB 20/05/2001 ]
//
//		}
//		// Modified CyberAge VSB 04/06/2001 ]

				if(((ortho && !SDS_CMainWindow->m_fIsShiftDown) || (!ortho && SDS_CMainWindow->m_fIsShiftDown))
				 && SDS_CMainWindow->m_pDragVarsCur->applyortho)
				ic_ortho(SDS_CMainWindow->m_pDragVarsCur->OrthoPt,snapang,pt3,opt,snapstyl);
				else			ic_ptcpy(opt,pt3);

				ic_ptcpy(SDS_CMainWindow->m_pDragVarsCur->pt3,opt);
				return(ret);
		}

		int SDS_dragone(sds_point pt3, int mode, int dragit, int ortho, sds_real snapang, int snapstyl) {

				sds_real rad,sa,ea,fr1,fr2,angbase,tang1,tang2,cdir,iang,bulge;
		sds_real realAng;
				sds_point cp,lp,opt,tpt1,tpt2,p1,p2,p3,p4;
				int ct, dashmode=0;
			sds_name ename;
				struct resbuf *rbb,*rbc,rb;
			static sds_real lastthick;
			struct gr_view *view=SDS_CURGRVW;
			CDC *dc = NULL;

			db_handitem*	Entity = SDS_CMainWindow->m_pDragVarsCur->elp;
			int				EntType = Entity ? Entity->ret_type() : -1;

			//NOTE: if mode==-2 and we're directly dragging a distance (e.g. updating a circle radius), we
			//			need to correct for elevation difference on the drag.  This is dependent upon viewdir.

				dashmode=SDS_CMainWindow->m_pDragVarsCur->dashmode;

				if(((ortho && !SDS_CMainWindow->m_fIsShiftDown) || (!ortho && SDS_CMainWindow->m_fIsShiftDown)) && mode && SDS_CMainWindow->m_pDragVarsCur->applyortho)
						ic_ortho(SDS_CMainWindow->m_pDragVarsCur->OrthoPt, snapang,pt3,opt,snapstyl);
				else
						if(!mode) {
					ic_ptcpy(opt,SDS_CMainWindow->m_pDragVarsCur->pt3);
				} else {
					ic_ptcpy(opt,pt3);
				}

			ic_ptcpy(SDS_CMainWindow->m_pDragVarsCur->LastDrag,opt);

				switch(SDS_CMainWindow->m_pDragVarsCur->mode) {
						case -10:	 // Update a angle special for ellipse
					{
					if(SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()!=DB_ELLIPSE) break;
					struct resbuf rbucs,rbwcs;
					rbucs.restype=rbwcs.restype=RTSHORT;
					rbucs.resval.rint=1;
					rbwcs.resval.rint=0;

					SDS_CMainWindow->m_pDragVarsCur->elp->get_11(cp);
					sds_trans(cp,&rbwcs,&rbucs,1,cp);
					fr1=sds_angle(SDS_CMainWindow->m_pDragVarsCur->pt1,opt);
					fr1-=atan2(cp[1],cp[0]);
					fr2=sqrt(cp[0]*cp[0]+cp[1]*cp[1]+cp[2]*cp[2]);
					SDS_CMainWindow->m_pDragVarsCur->elp->get_40(&rad);
					ic_angpar(fr1,&fr1,fr2,fr2*rad,0);
					//if we're dragging for starting parameter, be sure to normalize start & end
					if(SDS_CMainWindow->m_pDragVarsCur->elp->retp_41()==SDS_CMainWindow->m_pDragVarsCur->elpt){
						SDS_CMainWindow->m_pDragVarsCur->elp->get_42(&fr2);
						ic_normang(&fr1,&fr2);
						SDS_CMainWindow->m_pDragVarsCur->elp->set_42(fr2);
					}
					*SDS_CMainWindow->m_pDragVarsCur->elpt=fr1;

					ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
					ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
					SDS_UpdateEntDisp(ename,0);
					sds_redraw(ename,IC_REDRAW_XOR);

								SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt1,opt,-1,dashmode,0,SDS_CURDWG,0);
					}
					break;
						case -9:	 // Update axisratio
					{
					if(SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()!=DB_ELLIPSE) break;
					SDS_CMainWindow->m_pDragVarsCur->elp->get_11(cp);

					*SDS_CMainWindow->m_pDragVarsCur->elpt=
						sds_distance(SDS_CMainWindow->m_pDragVarsCur->pt1,opt)/
						 sqrt(cp[0]*cp[0]+cp[1]*cp[1]+cp[2]*cp[2]);

					ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
					ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
					SDS_UpdateEntDisp(ename,0);
					sds_redraw(ename,IC_REDRAW_XOR);

								SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt1,opt,-1,dashmode,0,SDS_CURDWG,0);
					}
					break;
						 case -8:	 // Update a direction vector.
					{
					if(SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()!=DB_RAY &&
						SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()!=DB_XLINE) break;

					SDS_CMainWindow->m_pDragVarsCur->elp->get_10(cp);
					lp[0]=opt[0]-cp[0];
					lp[1]=opt[1]-cp[1];
					lp[2]=opt[2]-cp[2];
					fr1=GR_LEN(lp);
					if(icadRealEqual(fr1,0.0))break;
					GR_UNITIZE(lp,fr1);
					SDS_CMainWindow->m_pDragVarsCur->elp->set_11(lp);

					ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
					ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
					SDS_UpdateEntDisp(ename,0);
					sds_redraw(ename,IC_REDRAW_XOR);
					}
					break;
						 case -7:	 // Update a direction vector.
					{
					if(SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()!=DB_RAY &&
						 SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()!=DB_XLINE &&
						 SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()!=DB_MTEXT &&
							 SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()!=DB_ELLIPSE) break;

					SDS_CURDWG->ucs2ent(opt,opt,SDS_CMainWindow->m_pDragVarsCur->xfa);
					SDS_CMainWindow->m_pDragVarsCur->elp->get_10(cp);
					lp[0]=opt[0]-cp[0];
					lp[1]=opt[1]-cp[1];
					lp[2]=opt[2]-cp[2];
					fr1=GR_LEN(lp);
					if(icadRealEqual(fr1,0.0)) break;
					if(SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()!=DB_ELLIPSE){
						//we'll need to unitize the vector
						if(SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_MTEXT)
						{
							// disable rotation of MTEXT on dragging, change only width
							sds_point dir;
							SDS_CMainWindow->m_pDragVarsCur->elp->get_11(dir);
							int justify;
							SDS_CMainWindow->m_pDragVarsCur->elp->get_71(&justify);

							fr1 = fabs(dir[0] * lp[0] + dir[1] * lp[1] + dir[2] * lp[2]) /
								(dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2]);
							if(justify % 3 == 2)
								fr1 *= 2.0;

							SDS_CMainWindow->m_pDragVarsCur->elp->get_11(lp);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_41(fr1);
							fr1 = 1.0;

							/* old code
							int justify;
							SDS_CMainWindow->m_pDragVarsCur->elp->get_71(&justify);
							if(2==(justify%3))fr1*=2.0;
							if(0==(justify%3)){
								lp[0]=-lp[0];
								lp[1]=-lp[1];
								lp[2]=-lp[2];
							}
							SDS_CMainWindow->m_pDragVarsCur->elp->set_41(fr1);
							*/
						}
						GR_UNITIZE(lp,fr1);
					}else{
						//do not unitize the vector
						if(icadRealEqual(SDS_CMainWindow->m_pDragVarsCur->ang,1.0)){
							//if user is dragging point opposite the x direction vector
							lp[0]=-lp[0];
							lp[1]=-lp[1];
							lp[2]=-lp[2];
						}
						fr1=GR_LEN(lp);
						if(icadRealEqual(fr1,0.0)) break;
					}
					SDS_CMainWindow->m_pDragVarsCur->elp->set_11(lp);
					ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
					ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;

						SDS_UpdateEntDisp(ename, 0);
						sds_redraw(ename, IC_REDRAW_XOR);
					}
					break;
					 case -6:
					{
					SDS_CURDWG->ucs2ent(opt,opt,SDS_CMainWindow->m_pDragVarsCur->xfa);
								if(mode==2) {
										SDS_CMainWindow->m_pDragVarsCur->rblst=sds_buildlist(10,SDS_CMainWindow->m_pDragVarsCur->pt1,11,opt,0);
										break;
								}
		//			if(SDS_CMainWindow->m_fIsCtrlDown) {
		//				SDS_CMainWindow->m_pDragVarsCur->elp->set_39(sds_distance(SDS_CMainWindow->m_pKeyDragPt,opt));
		//			} else {
						SDS_getvar(NULL,DB_QTHICKNESS,&rb,SDS_CMainWindow->m_pDragVarsCur->flp,&SDS_CURCFG,&SDS_CURSES);
						SDS_CMainWindow->m_pDragVarsCur->elp->set_39(rb.resval.rreal);
						SDS_CMainWindow->m_pDragVarsCur->elp->set_11(opt);
		//			}

					ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
					ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
					SDS_UpdateEntDisp(ename,0);
					sds_redraw(ename,IC_REDRAW_XOR);
					}
								break;
						case -5:	 // Draw text dynamically
					{
					SDS_CMainWindow->m_pDragVarsCur->elp->set_1(SDS_cmdcharbuf);

					dc = GetIcadTargetDeviceCDC();
					if (NULL == dc)
						break;

					gr_fixtextjust(SDS_CMainWindow->m_pDragVarsCur->elp,
									 SDS_CMainWindow->m_pDragVarsCur->flp,
									 SDS_CURGRVW,
									 dc,
									 0);
					ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
					ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
					SDS_UpdateEntDisp(ename,0);
					sds_redraw(ename,IC_REDRAW_XOR);
					}
					break;
						case -4:	 // Update the block scale
					{
					SDS_CMainWindow->m_pDragVarsCur->elp->set_41(opt[0]-SDS_CMainWindow->m_pDragVarsCur->pt1[0]);
					SDS_CMainWindow->m_pDragVarsCur->elp->set_42(opt[1]-SDS_CMainWindow->m_pDragVarsCur->pt1[1]);
					SDS_CMainWindow->m_pDragVarsCur->elp->set_43(opt[2]-SDS_CMainWindow->m_pDragVarsCur->pt1[2]);

					ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
					ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
					SDS_UpdateEntDisp(ename,0);
					sds_redraw(ename,IC_REDRAW_XOR);
					}
					break;
						case -3:	 // Update an angle

					if(SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_ELLIPSE){
						//for ellipses, angle is relative to axis angle, so we must
						//	subtract off axis angle
						struct resbuf rbucs,rbwcs;
						rbucs.restype=rbwcs.restype=RTSHORT;
						rbucs.resval.rint=1;
						rbwcs.resval.rint=0;

						SDS_CMainWindow->m_pDragVarsCur->elp->get_11(cp);
						sds_trans(cp,&rbwcs,&rbucs,1,cp);
						fr1=sds_angle(SDS_CMainWindow->m_pDragVarsCur->pt1,opt);
						fr1-=atan2(cp[1],cp[0]);
						while(fr1>IC_TWOPI){
							fr1-=IC_TWOPI;
						}
						*SDS_CMainWindow->m_pDragVarsCur->elpt=fr1;
				// Modified Cybage SBD 11/02/2002 DD/MM/YYYY [
				// Reason: Fix for bug no. 77974
				// EBATECHC(CNBR) -[ 2002/4/24 Suuport SHAPE
				//}else if(SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_INSERT){ //}
				}else if(SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_INSERT ||
					SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_SHAPE ){
					*SDS_CMainWindow->m_pDragVarsCur->elpt=
							sds_angle(SDS_CMainWindow->m_pDragVarsCur->pt1,opt)+ SDS_CMainWindow->m_pDragVarsCur->dang ;
					ic_normang(SDS_CMainWindow->m_pDragVarsCur->elpt,NULL);
				// Modified Cybage SBD 11/02/2002 DD/MM/YYYY ]
					}else
						*SDS_CMainWindow->m_pDragVarsCur->elpt=
							sds_angle(SDS_CMainWindow->m_pDragVarsCur->pt1,opt);

					ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
					ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;

					if(SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_TEXT ||
						 SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_ATTRIB ||
						 SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_ATTDEF)
						{
						dc = GetIcadTargetDeviceCDC();
						if (NULL == dc)
							break;

						gr_fixtextjust(SDS_CMainWindow->m_pDragVarsCur->elp,
										 SDS_CMainWindow->m_pDragVarsCur->flp,
										 NULL,
										 dc,
										 0);
						}

					SDS_UpdateEntDisp(ename,0);
					sds_redraw(ename,IC_REDRAW_XOR);

								SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt1,opt,-1,dashmode,0,SDS_CURDWG,0);
					break;
						case -2:	 // Update a distance
					//we may be updating the distance using the edge of an extruded ent
					ic_ptcpy(p1,SDS_CMainWindow->m_pDragVarsCur->pt1);
					if(fabs(SDS_CMainWindow->m_pDragVarsCur->ang)>IC_ZRO){
						sds_point temp_extru;
						struct resbuf rbucs,rbwcs;

						rbucs.restype=rbwcs.restype=RTSHORT;
						rbucs.resval.rint=1;
						rbwcs.resval.rint=0;
						SDS_CMainWindow->m_pDragVarsCur->elp->get_210(temp_extru);
						sds_trans(temp_extru,&rbwcs,&rbucs,1,temp_extru);
						p1[0]+=temp_extru[0]*SDS_CMainWindow->m_pDragVarsCur->ang;
						p1[1]+=temp_extru[1]*SDS_CMainWindow->m_pDragVarsCur->ang;
						p1[2]+=temp_extru[2]*SDS_CMainWindow->m_pDragVarsCur->ang;
					}
					//NOTE: we have to correct for elev difference between pt1 and drag pt
					if(p1[2]!=opt[2]){	//correct for elev
						SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CMainWindow->m_pDragVarsCur->flp,&SDS_CURCFG,&SDS_CURSES);
						ic_ptcpy(tpt1,rb.resval.rpoint);
						if(fabs(tpt1[2])>IC_ZRO){
							tpt2[0]=(p1[2]-opt[2])/tpt1[2];
							opt[0]+=tpt2[0]*tpt1[0];
							opt[1]+=tpt2[0]*tpt1[1];
							opt[2]=p1[2];
						}
					}//opt is now corrected for elevation difference
					if(SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()!=DB_ELLIPSE){
						*SDS_CMainWindow->m_pDragVarsCur->elpt=
							sds_distance(p1,opt);
					}else{
						SDS_CMainWindow->m_pDragVarsCur->elp->get_11(cp);//call semimajor axis vector cp
						*SDS_CMainWindow->m_pDragVarsCur->elpt=
							sds_distance(p1,opt)/
							sqrt((cp[0]*cp[0])+(cp[1]*cp[1])+(cp[2]*cp[2]));

					}

					ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
					ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;

					if(SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_TEXT ||
						 SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_ATTRIB ||
						 SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_ATTDEF)
						{
						dc = GetIcadTargetDeviceCDC();
						if (NULL == dc)
							break;

						gr_fixtextjust(SDS_CMainWindow->m_pDragVarsCur->elp,
										 SDS_CMainWindow->m_pDragVarsCur->flp,
										 NULL,
										 dc,
										 0);
						}

					SDS_UpdateEntDisp(ename,0);
					sds_redraw(ename,IC_REDRAW_XOR);

								SDS_DrawLine(p1,opt,-1,dashmode,0,SDS_CURDWG,0);
					break;
						case -1:	 // Update a point
					//if SDS_CMainWindow->m_pDragVarsCur->ang is non-zero, we're dragging an extruded edge
					//	and will have to compensate for the extrusion

					// If dragging the 3rd point 3dface or solid update the fourth point too.
					if(SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_3DFACE ||
						 SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_SOLID) {
						if(SDS_CMainWindow->m_pDragVarsCur->elp->retp_12()==SDS_CMainWindow->m_pDragVarsCur->elpt &&
								SDS_CMainWindow->m_pDragVarsCur->elp->ret_is3pt()) {
							ic_ptcpy(cp,opt);
							SDS_CURDWG->ucs2ent(cp,cp,SDS_CMainWindow->m_pDragVarsCur->xfa);
							cp[2]-=SDS_CMainWindow->m_pDragVarsCur->ang;
							SDS_CMainWindow->m_pDragVarsCur->elp->set_13(cp);
						}
					}
					//if text ent & dragging by 10pt, get the 10pt BEFORE altering it
					//	so we can update the 11pt
					if((SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_TEXT ||
						SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_ATTDEF ||
							SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_ATTRIB)&&
						SDS_CMainWindow->m_pDragVarsCur->elp->retp_10()==SDS_CMainWindow->m_pDragVarsCur->elpt){
							SDS_CMainWindow->m_pDragVarsCur->elp->get_10(p1);//call semimajor axis vector cp
					}
					//copy new point to target as passed...
					ic_ptcpy(SDS_CMainWindow->m_pDragVarsCur->elpt,opt);
					SDS_CURDWG->ucs2ent(SDS_CMainWindow->m_pDragVarsCur->elpt,
										 SDS_CMainWindow->m_pDragVarsCur->elpt,
											 SDS_CMainWindow->m_pDragVarsCur->xfa);
					//if ent has thickness and we're dragging the extruded edge, correct for it
					if(DB_LINE==SDS_CMainWindow->m_pDragVarsCur->elp->ret_type() && fabs(SDS_CMainWindow->m_pDragVarsCur->ang)>IC_ZRO){
						sds_point temp_extru;
						SDS_CMainWindow->m_pDragVarsCur->elp->get_210(temp_extru);
						SDS_CMainWindow->m_pDragVarsCur->elpt[0]-=temp_extru[0]*SDS_CMainWindow->m_pDragVarsCur->ang;
						SDS_CMainWindow->m_pDragVarsCur->elpt[1]-=temp_extru[1]*SDS_CMainWindow->m_pDragVarsCur->ang;
						SDS_CMainWindow->m_pDragVarsCur->elpt[2]-=temp_extru[2]*SDS_CMainWindow->m_pDragVarsCur->ang;
					}else SDS_CMainWindow->m_pDragVarsCur->elpt[2]-=SDS_CMainWindow->m_pDragVarsCur->ang;
					//if dragging a closed spline, there's one node for start pt and one for end pt.
					//	we need to be sure to update them both
					if(SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_SPLINE){
						int fi1,flag70;
						double *pp1;

						SDS_CMainWindow->m_pDragVarsCur->elp->get_70(&flag70);
						if(flag70&1){
							SDS_CMainWindow->m_pDragVarsCur->elp->get_74(&fi1);
							//we already copied the correctly xformed pt to the pointer,
							//	so we can use it for the other end of the closed spline
							if((pp1=SDS_CMainWindow->m_pDragVarsCur->elp->retp_11(0))==SDS_CMainWindow->m_pDragVarsCur->elpt)
								ic_ptcpy(SDS_CMainWindow->m_pDragVarsCur->elp->retp_11(fi1-1),pp1);
							else if((pp1=SDS_CMainWindow->m_pDragVarsCur->elp->retp_11(fi1-1))==SDS_CMainWindow->m_pDragVarsCur->elpt)
								ic_ptcpy(SDS_CMainWindow->m_pDragVarsCur->elp->retp_11(0),pp1);
						}
					}
					//if text ent:	if dragging by 11, fixjust so 10 is correct
					//				if dragging by 10, offset 11 appropriately
					if(SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_TEXT ||
						 SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_ATTDEF ||
						 SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_ATTRIB)
						 {
						if(SDS_CMainWindow->m_pDragVarsCur->elp->retp_10()!=SDS_CMainWindow->m_pDragVarsCur->elpt)
							{
							//dragging 11pt
							dc = GetIcadTargetDeviceCDC();
							if (NULL == dc)
								break;

							gr_fixtextjust(SDS_CMainWindow->m_pDragVarsCur->elp,
											 SDS_CMainWindow->m_pDragVarsCur->flp,
											 SDS_CURGRVW,
											 dc,
											 0);
							}
						else
							{
							//dragging 10pt
							SDS_CMainWindow->m_pDragVarsCur->elp->get_11(cp);//call semimajor axis vector cp
							cp[0]+=opt[0]-p1[0];
							cp[1]+=opt[1]-p1[1];
							cp[1]+=opt[2]-p1[2];
							SDS_CMainWindow->m_pDragVarsCur->elp->set_11(cp);
							}
						}
					if(SDS_CMainWindow->m_pDragVarsCur->elp->ret_type()==DB_DIMENSION) {
						if(dragit)
							cmd_makedimension(SDS_CMainWindow->m_pDragVarsCur->elp,
												SDS_CMainWindow->m_pDragVarsCur->flp, 1);
					} else {
						if(dragit) {
							ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
							ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;

								SDS_UpdateEntDisp(ename, 0);
								sds_redraw(ename, IC_REDRAW_XOR);
						}
					}
					break;
						case 0:		// ARC			start/second/[end]
					//if SDS_CMainWindow->m_pDragVarsCur->ang is non-zero, we're dragging the extruded edge
					if(dragit && mode!=2 && SDS_CMainWindow->m_pDragVarsCur->elp && fabs(SDS_CMainWindow->m_pDragVarsCur->ang)>IC_ZRO){
						sds_point extru;
						SDS_CMainWindow->m_pDragVarsCur->elp->get_210(extru);
						opt[0]-=extru[0]*SDS_CMainWindow->m_pDragVarsCur->ang;
						opt[1]-=extru[1]*SDS_CMainWindow->m_pDragVarsCur->ang;
						opt[2]-=extru[2]*SDS_CMainWindow->m_pDragVarsCur->ang;
					}
								if(!ic_3pt2arc(SDS_CMainWindow->m_pDragVarsCur->pt1,SDS_CMainWindow->m_pDragVarsCur->pt2,opt,cp,&rad,&sa,&ea)) {

						SDS_CURDWG->ucs2ent(cp,cp,SDS_CMainWindow->m_pDragVarsCur->xfa);
										sa+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&sa,NULL);
										ea+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&ea,NULL);

						if(mode==2) {
												SDS_CMainWindow->m_pDragVarsCur->rblst=sds_buildlist(10,cp,40,rad,50,sa,51,ea,0);
												break;
										}
										if(dragit) {	 // FUNCTION ?
							if(SDS_CMainWindow->m_pDragVarsCur->elp) {
								SDS_CMainWindow->m_pDragVarsCur->elp->set_10(cp);
								SDS_CMainWindow->m_pDragVarsCur->elp->set_40(rad);
								SDS_CMainWindow->m_pDragVarsCur->elp->set_50(sa);
								SDS_CMainWindow->m_pDragVarsCur->elp->set_51(ea);
								SDS_CMainWindow->m_pDragVarsCur->elp->set_39(SDS_CMainWindow->m_pDragVarsCur->ang);
								ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
								ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
								SDS_UpdateEntDisp(ename,0);
								sds_redraw(ename,IC_REDRAW_XOR);
							} else {
								SDS_DrawArc(-1,dashmode,0,SDS_CMainWindow->m_pDragVarsCur->scrht,cp[0],cp[1],rad,sa,ea,0.0);
							}
						}				 // FUNCTION ?
								}
								break;
						case 1:	 // CIRCLE	 center [radius]
								if(SDS_CMainWindow->m_pDragVarsCur->gotnub && mode==2) {
										rad=SDS_CMainWindow->m_pDragVarsCur->nub;
										sds_polar(SDS_CMainWindow->m_pDragVarsCur->pt1,0.0,rad,pt3);
								} else {
										rad=sds_distance(SDS_CMainWindow->m_pDragVarsCur->pt1,opt);
								}
								if(0.0==rad &&
						(SDSApplication::GetActiveApplication()->GetInputControlBits() & RSG_NOZERO))
						{
						break;
						}
					SDS_CURDWG->ucs2ent(SDS_CMainWindow->m_pDragVarsCur->pt1,cp,SDS_CMainWindow->m_pDragVarsCur->xfa);
								if(mode==2) {
										SDS_CMainWindow->m_pDragVarsCur->rblst=sds_buildlist(10,cp,40,rad,0);
										break;
								}
								if(dragit) {	 // FUNCTION ?
						if(SDS_CMainWindow->m_pDragVarsCur->elp) {
							SDS_CMainWindow->m_pDragVarsCur->elp->set_10(cp);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_40(rad);
							ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
							ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
							SDS_UpdateEntDisp(ename,0);
							sds_redraw(ename,IC_REDRAW_XOR);
						} else {
							SDS_DrawArc(-1,dashmode,2,SDS_CMainWindow->m_pDragVarsCur->scrht,cp[0],cp[1],rad,0.0,0.0,0.0);
						}
					}				 // FUNCTION ?
								SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt1,opt,-1,dashmode,0,SDS_CURDWG,0);
								break;
						case 2:	 // LINE		 first/angle/[length]
								if(SDS_CMainWindow->m_pDragVarsCur->gotnub && mode==2) {
										fr1=SDS_CMainWindow->m_pDragVarsCur->nub;
								} else {
										fr1=sds_distance(SDS_CMainWindow->m_pDragVarsCur->pt1,opt);
								}

					//Handle angbase & angdir
					SDS_getvar(NULL,DB_QANGBASE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
					angbase = rb.resval.rreal;
					SDS_getvar(NULL,DB_QANGDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
#ifndef OLD_ANGBASE
				ASSERT(angbase >= 0);
				if(rb.resval.rint)	// support CW direction was added
					angbase = IC_TWOPI - angbase;
#endif 
				if (rb.resval.rint) // don't change SDS_CMainWindow->m_pDragVarsCur->ang value! (this fuction is called many times during dragging)
						realAng = SDS_CMainWindow->m_pDragVarsCur->ang - angbase;
					else
						realAng = SDS_CMainWindow->m_pDragVarsCur->ang + angbase;
					sds_polar(SDS_CMainWindow->m_pDragVarsCur->pt1, realAng, fr1, cp);
				SDS_CURDWG->ucs2ent(SDS_CMainWindow->m_pDragVarsCur->pt1,tpt1,SDS_CMainWindow->m_pDragVarsCur->xfa);
					SDS_CURDWG->ucs2ent(cp,cp,SDS_CMainWindow->m_pDragVarsCur->xfa);
								if(mode==2) {
										SDS_CMainWindow->m_pDragVarsCur->rblst=sds_buildlist(10,tpt1,11,cp,0);
										break;
								}
								if(dragit) {	 // FUNCTION ?
						if(SDS_CMainWindow->m_pDragVarsCur->elp) {
							SDS_CMainWindow->m_pDragVarsCur->elp->set_10(tpt1);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_11(cp);
							ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
							ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
							SDS_UpdateEntDisp(ename,0);
							sds_redraw(ename,IC_REDRAW_XOR);
						} else {
							SDS_DrawLine(tpt1,cp,-1,dashmode,0,SDS_CURDWG,0);
						}
					}				 // FUNCTION ?
					SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt1,opt,-1,dashmode,0,SDS_CURDWG,0);
								break;
						case 3:	 // CIRCLE	 first/[second]
								opt[2]=SDS_CMainWindow->m_pDragVarsCur->pt1[2];
								rad=sds_distance(SDS_CMainWindow->m_pDragVarsCur->pt1,opt)/2.0;
								sds_polar(SDS_CMainWindow->m_pDragVarsCur->pt1,sds_angle(SDS_CMainWindow->m_pDragVarsCur->pt1,opt),sds_distance(SDS_CMainWindow->m_pDragVarsCur->pt1,opt)/2.0,cp);
					SDS_CURDWG->ucs2ent(cp,cp,SDS_CMainWindow->m_pDragVarsCur->xfa);
								if(mode==2) {
										SDS_CMainWindow->m_pDragVarsCur->rblst=sds_buildlist(10,cp,40,rad,0);
										break;
								}
								if(dragit) {	 // FUNCTION ?
						if(SDS_CMainWindow->m_pDragVarsCur->elp) {
							SDS_CMainWindow->m_pDragVarsCur->elp->set_10(cp);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_40(rad);
							ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
							ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
							SDS_UpdateEntDisp(ename,0);
							sds_redraw(ename,IC_REDRAW_XOR);
						} else {
							SDS_DrawArc(-1,dashmode,2,SDS_CMainWindow->m_pDragVarsCur->scrht,cp[0],cp[1],rad,0.0,0.0,0.0);
						}
					}				 // FUNCTION ?
								break;
						case 4:	 // CIRCLE	 first/second/[third]
								opt[2]=SDS_CMainWindow->m_pDragVarsCur->pt1[2];
								if(!ic_3pt2arc(SDS_CMainWindow->m_pDragVarsCur->pt1,SDS_CMainWindow->m_pDragVarsCur->pt2,opt,cp,&rad,&sa,&ea)) {
						SDS_CURDWG->ucs2ent(cp,cp,SDS_CMainWindow->m_pDragVarsCur->xfa);
										if(mode==2) {
												SDS_CMainWindow->m_pDragVarsCur->rblst=sds_buildlist(10,cp,40,rad,0);
												break;
										}
						if(dragit) {	 // FUNCTION ?
							if(SDS_CMainWindow->m_pDragVarsCur->elp) {
								SDS_CMainWindow->m_pDragVarsCur->elp->set_10(cp);
								SDS_CMainWindow->m_pDragVarsCur->elp->set_40(rad);
								ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
								ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
								SDS_UpdateEntDisp(ename,0);
								sds_redraw(ename,IC_REDRAW_XOR);
							} else {
								SDS_DrawArc(-1,dashmode,2,SDS_CMainWindow->m_pDragVarsCur->scrht,cp[0],cp[1],rad,0.0,0.0,0.0);
							}
						}				 // FUNCTION ?
								}
								break;
						case 5:		 // ARC 		 center/start/[angle]
								sa=sds_angle(SDS_CMainWindow->m_pDragVarsCur->pt1,SDS_CMainWindow->m_pDragVarsCur->pt2);
								if(SDS_CMainWindow->m_pDragVarsCur->gotnub && mode==2) {
						//if user entered value by hand, correct for angbase & angdir
						SDS_getvar(NULL,DB_QANGBASE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						angbase=rb.resval.rreal;
						SDS_getvar(NULL,DB_QANGDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						fr1=SDS_CMainWindow->m_pDragVarsCur->nub;
				#ifndef OLD_ANGBASE
					ASSERT(angbase >= 0);
					if(rb.resval.rint) // support CW direction was added
						angbase = IC_TWOPI - angbase;
				#endif
						if(fr1<0.0){
							fr1+=IC_TWOPI;
							if(rb.resval.rint){
								fr1=IC_TWOPI-fr1;
								fr1+=angbase;
							}else{
								fr1-=angbase;
							}
							fr1-=IC_TWOPI;
							ea=sa;
							sa=ea+fr1;
							ic_normang(&sa,NULL);
						}else{
							if(rb.resval.rint){
								fr1=IC_TWOPI-fr1;
								fr1+=angbase;
							}else{
								fr1-=angbase;
							}
							ea=sa+fr1;
							ic_normang(&ea,NULL);
						}
								} else {
										ea=sds_angle(SDS_CMainWindow->m_pDragVarsCur->pt1,opt);
								}

								/*if(sa<0.0) { // *** fr1 is the direction of the arc - 1.0 (CCW); -1.0 (CW)
										fr1=(-1.0);
										ea=(-ea);
								} else fr1=1.0;*/
								//ea=ea+angbase+sa;
								rad=sds_distance(SDS_CMainWindow->m_pDragVarsCur->pt1,SDS_CMainWindow->m_pDragVarsCur->pt2);
								//if (fr1<0.0) { fr1=ea; ea=sa; sa-=(fr1-sa); }

					SDS_CURDWG->ucs2ent(SDS_CMainWindow->m_pDragVarsCur->pt1,cp,SDS_CMainWindow->m_pDragVarsCur->xfa);
								sa+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&sa,NULL);
								ea+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&ea,NULL);

								if(mode==2) {
										SDS_CMainWindow->m_pDragVarsCur->rblst=sds_buildlist(10,cp,40,rad,50,sa,51,ea,0);
										break;
								}
								if(dragit) {	 // FUNCTION ?
						if(SDS_CMainWindow->m_pDragVarsCur->elp) {
							SDS_CMainWindow->m_pDragVarsCur->elp->set_10(cp);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_40(rad);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_50(sa);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_51(ea);
							ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
							ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
							SDS_UpdateEntDisp(ename,0);
							sds_redraw(ename,IC_REDRAW_XOR);
						} else {
							SDS_DrawArc(-1,dashmode,0,SDS_CMainWindow->m_pDragVarsCur->scrht,cp[0],cp[1],rad,sa,ea,0.0);
						}
					}				 // FUNCTION ?
								SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt1,opt,-1,dashmode,0,SDS_CURDWG,0);
								break;
						case 6:	 //  ARC			center/start/[length of chord]
								sa=sds_angle(SDS_CMainWindow->m_pDragVarsCur->pt1,SDS_CMainWindow->m_pDragVarsCur->pt2);
								rad=sds_distance(SDS_CMainWindow->m_pDragVarsCur->pt1,SDS_CMainWindow->m_pDragVarsCur->pt2);
								if(SDS_CMainWindow->m_pDragVarsCur->gotnub && mode==2) {
										fr1=SDS_CMainWindow->m_pDragVarsCur->nub;
								} else {
										fr1=sds_distance(SDS_CMainWindow->m_pDragVarsCur->pt2,opt);
								}
					fr1/=2.0*rad;
					if(fr1>1.0) fr1=1.0;
								ea=2.0*asin(fr1)+sa;

					SDS_CURDWG->ucs2ent(SDS_CMainWindow->m_pDragVarsCur->pt1,cp,SDS_CMainWindow->m_pDragVarsCur->xfa);
								sa+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&sa,NULL);
								ea+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&ea,NULL);

								if(mode==2) {
										SDS_CMainWindow->m_pDragVarsCur->rblst=sds_buildlist(10,cp,40,rad,50,sa,51,ea,0);
										break;
								}
								if(dragit) {	 // FUNCTION ?
						if(SDS_CMainWindow->m_pDragVarsCur->elp) {
							SDS_CMainWindow->m_pDragVarsCur->elp->set_10(cp);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_40(rad);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_50(sa);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_51(ea);
							ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
							ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
							SDS_UpdateEntDisp(ename,0);
							sds_redraw(ename,IC_REDRAW_XOR);
						} else {
							SDS_DrawArc(-1,dashmode,0,SDS_CMainWindow->m_pDragVarsCur->scrht,cp[0],cp[1],rad,sa,ea,0.0);
						}
					}				 // FUNCTION ?
								SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt2,opt,-1,dashmode,0,SDS_CURDWG,0);
								break;
						case 7:		// ARC			center/start/[end]
								sa=sds_angle(SDS_CMainWindow->m_pDragVarsCur->pt1,SDS_CMainWindow->m_pDragVarsCur->pt2);
								ea=sds_angle(SDS_CMainWindow->m_pDragVarsCur->pt1,opt);
								rad=sds_distance(SDS_CMainWindow->m_pDragVarsCur->pt1,SDS_CMainWindow->m_pDragVarsCur->pt2);

					SDS_CURDWG->ucs2ent(SDS_CMainWindow->m_pDragVarsCur->pt1,cp,SDS_CMainWindow->m_pDragVarsCur->xfa);
								sa+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&sa,NULL);
								ea+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&ea,NULL);

								if(mode==2) {
										SDS_CMainWindow->m_pDragVarsCur->rblst=sds_buildlist(10,cp,40,rad,50,sa,51,ea,0);
										break;
								}
								if(dragit) {	 // FUNCTION ?
						if(SDS_CMainWindow->m_pDragVarsCur->elp) {
							SDS_CMainWindow->m_pDragVarsCur->elp->set_10(cp);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_40(rad);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_50(sa);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_51(ea);
							ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
							ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
							SDS_UpdateEntDisp(ename,0);
							sds_redraw(ename,IC_REDRAW_XOR);
						} else {
							SDS_DrawArc(-1,dashmode,0,SDS_CMainWindow->m_pDragVarsCur->scrht,cp[0],cp[1],rad,sa,ea,0.0);
						}
					}				 // FUNCTION ?
								SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt1,opt,-1,dashmode,0,SDS_CURDWG,0);
								break;
						case 8:		// ARC			start/center/[angle]
								sa=sds_angle(SDS_CMainWindow->m_pDragVarsCur->pt2,SDS_CMainWindow->m_pDragVarsCur->pt1);
								if(SDS_CMainWindow->m_pDragVarsCur->gotnub && mode==2) {
						ea=SDS_CMainWindow->m_pDragVarsCur->nub;
								} else {
										ea=sds_angle(SDS_CMainWindow->m_pDragVarsCur->pt2,opt);
								}
								if(ea<0.0) { // *** fr1 is the direction of the arc - 1.0 (CCW); -1.0 (CW)
										fr1=(-1.0);
										ea=(-ea);
								} else fr1=1.0;
					SDS_getvar(NULL,DB_QANGBASE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
								angbase=rb.resval.rreal;

			#ifndef OLD_ANGBASE
	 				SDS_getvar(NULL,DB_QANGDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				ASSERT(angbase >= 0);
				if(rb.resval.rint) // support CW direction was added
					angbase = IC_TWOPI - angbase;
			#endif
				
								if(ea<0.0) {
										sa=sa-ea+angbase;
										ea=sa+ea+angbase;
								} else {
										ea=ea+angbase+sa;
								}
								rad=sds_distance(SDS_CMainWindow->m_pDragVarsCur->pt2,SDS_CMainWindow->m_pDragVarsCur->pt1);
								if (fr1<0.0) { fr1=ea; ea=sa; sa-=(fr1-sa); }

					SDS_CURDWG->ucs2ent(SDS_CMainWindow->m_pDragVarsCur->pt2,cp,SDS_CMainWindow->m_pDragVarsCur->xfa);
								sa+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&sa,NULL);
								ea+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&ea,NULL);

								if(mode==2) {
										SDS_CMainWindow->m_pDragVarsCur->rblst=sds_buildlist(10,cp,40,rad,50,sa,51,ea,0);
										break;
								}
								if(dragit) {	 // FUNCTION ?
						if(SDS_CMainWindow->m_pDragVarsCur->elp) {
							SDS_CMainWindow->m_pDragVarsCur->elp->set_10(cp);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_40(rad);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_50(sa);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_51(ea);
							ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
							ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
							SDS_UpdateEntDisp(ename,0);
							sds_redraw(ename,IC_REDRAW_XOR);
						} else {
							SDS_DrawArc(-1,dashmode,0,SDS_CMainWindow->m_pDragVarsCur->scrht,cp[0],cp[1],rad,sa,ea,0.0);
						}
					}				 // FUNCTION ?
								SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt2,opt,-1,dashmode,0,SDS_CURDWG,0);
								break;
						case 9:	 // ARC 		 start/center/[end]
								sa=sds_angle(SDS_CMainWindow->m_pDragVarsCur->pt2,SDS_CMainWindow->m_pDragVarsCur->pt1);
								ea=sds_angle(SDS_CMainWindow->m_pDragVarsCur->pt2,opt);
								rad=sds_distance(SDS_CMainWindow->m_pDragVarsCur->pt2,SDS_CMainWindow->m_pDragVarsCur->pt1);

					SDS_CURDWG->ucs2ent(SDS_CMainWindow->m_pDragVarsCur->pt2,cp,SDS_CMainWindow->m_pDragVarsCur->xfa);
								sa+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&sa,NULL);
								ea+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&ea,NULL);

								if(mode==2) {
										SDS_CMainWindow->m_pDragVarsCur->rblst=sds_buildlist(10,cp,40,rad,50,sa,51,ea,0);
										break;
								}
								if(dragit) {	 // FUNCTION ?
						if(SDS_CMainWindow->m_pDragVarsCur->elp) {
							SDS_CMainWindow->m_pDragVarsCur->elp->set_10(cp);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_40(rad);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_50(sa);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_51(ea);
							ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
							ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
							SDS_UpdateEntDisp(ename,0);
							sds_redraw(ename,IC_REDRAW_XOR);
						} else {
							SDS_DrawArc(-1,dashmode,0,SDS_CMainWindow->m_pDragVarsCur->scrht,cp[0],cp[1],rad,sa,ea,0.0);
						}
					}				 // FUNCTION ?
								SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt2,opt,-1,dashmode,0,SDS_CURDWG,0);
								break;
						case 10:	// ARC			start/center/[length of chord]
								sa=sds_angle(SDS_CMainWindow->m_pDragVarsCur->pt2,SDS_CMainWindow->m_pDragVarsCur->pt1);
								rad=sds_distance(SDS_CMainWindow->m_pDragVarsCur->pt2,SDS_CMainWindow->m_pDragVarsCur->pt1);
								if(SDS_CMainWindow->m_pDragVarsCur->gotnub && mode==2) {
										fr2=SDS_CMainWindow->m_pDragVarsCur->nub;
								} else {
										fr2=sds_distance(SDS_CMainWindow->m_pDragVarsCur->pt1,opt);
								}

						fr2/=2.0*rad;
					//check for valid ang for asin
								if(fabs(fr2)>1.0) {fr2/=fabs(fr2);};
								ea=2.0*asin(fr2)+sa;

					SDS_CURDWG->ucs2ent(SDS_CMainWindow->m_pDragVarsCur->pt2,cp,SDS_CMainWindow->m_pDragVarsCur->xfa);
								sa+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&sa,NULL);
								ea+=SDS_CMainWindow->m_pDragVarsCur->dang;
								ic_normang(&ea,NULL);

								if(mode==2) {
										SDS_CMainWindow->m_pDragVarsCur->rblst=sds_buildlist(10,cp,40,rad,50,sa,51,ea,0);
										break;
								}
								if(dragit) {	 // FUNCTION ?
						if(SDS_CMainWindow->m_pDragVarsCur->elp) {
							SDS_CMainWindow->m_pDragVarsCur->elp->set_10(cp);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_40(rad);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_50(sa);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_51(ea);
							ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
							ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
							SDS_UpdateEntDisp(ename,0);
							sds_redraw(ename,IC_REDRAW_XOR);
						} else {
							SDS_DrawArc(-1,dashmode,0,SDS_CMainWindow->m_pDragVarsCur->scrht,cp[0],cp[1],rad,sa,ea,0.0);
						}
					}				 // FUNCTION ?
								SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt1,opt,-1,dashmode,0,SDS_CURDWG,0);
								break;
						case 11:	 // ARC 		 start/end/[angle]
								if(SDS_CMainWindow->m_pDragVarsCur->gotnub && mode==2) {
										fr2=SDS_CMainWindow->m_pDragVarsCur->nub;
								} else {
										fr2=sds_angle(SDS_CMainWindow->m_pDragVarsCur->pt1,opt);
								}
								fr1=sds_distance(SDS_CMainWindow->m_pDragVarsCur->pt1,SDS_CMainWindow->m_pDragVarsCur->pt2);
								fr2=2.0*tan(fr2/2.0);
								cp[0]=(SDS_CMainWindow->m_pDragVarsCur->pt1[0]+SDS_CMainWindow->m_pDragVarsCur->pt2[0])/2.0;										 // *((fr2<0.0) ? -1.0 : 1.0)
								cp[1]=(SDS_CMainWindow->m_pDragVarsCur->pt1[1]+SDS_CMainWindow->m_pDragVarsCur->pt2[1])/2.0;
								cp[2]=SDS_CMainWindow->m_pDragVarsCur->pt1[2];
								sds_polar(cp,sds_angle(SDS_CMainWindow->m_pDragVarsCur->pt1,SDS_CMainWindow->m_pDragVarsCur->pt2)+(IC_PI/2.0),icadRealEqual(fr2,0.0) ? 1.0E+100 : fr1/fr2,cp);
								sa=sds_angle(cp,SDS_CMainWindow->m_pDragVarsCur->pt1);
								ea=sds_angle(cp,SDS_CMainWindow->m_pDragVarsCur->pt2);

					// Swap the sa and ea if the entered angle is <0.
							if(SDS_CMainWindow->m_pDragVarsCur->gotnub && mode==2 &&
						SDS_CMainWindow->m_pDragVarsCur->nub<0.0) {
							fr2=ea; ea=sa; sa=fr2;
					}

					rad=sds_distance(cp,SDS_CMainWindow->m_pDragVarsCur->pt1);

					SDS_CURDWG->ucs2ent(cp,cp,SDS_CMainWindow->m_pDragVarsCur->xfa);
								sa+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&sa,NULL);
								ea+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&ea,NULL);

								if(mode==2) {
										SDS_CMainWindow->m_pDragVarsCur->rblst=sds_buildlist(10,cp,40,rad,50,sa,51,ea,0);
										break;
								}
								if(dragit) {	 // FUNCTION ?
						if(SDS_CMainWindow->m_pDragVarsCur->elp) {
							SDS_CMainWindow->m_pDragVarsCur->elp->set_10(cp);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_40(rad);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_50(sa);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_51(ea);
							ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
							ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
							SDS_UpdateEntDisp(ename,0);
							sds_redraw(ename,IC_REDRAW_XOR);
						} else {
							SDS_DrawArc(-1,dashmode,0,SDS_CMainWindow->m_pDragVarsCur->scrht,cp[0],cp[1],rad,sa,ea,0.0);
						}
					}				 // FUNCTION ?
								SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt1,opt,-1,dashmode,0,SDS_CURDWG,0);
								break;
						case 12:	// ARC			start/end/[direction]
								if(SDS_CMainWindow->m_pDragVarsCur->gotnub && mode==2) {
										fr2=SDS_CMainWindow->m_pDragVarsCur->nub;
								} else {
										fr2=sds_angle(SDS_CMainWindow->m_pDragVarsCur->pt1,opt);
								}
								if(!ic_2pta2arc(SDS_CMainWindow->m_pDragVarsCur->pt1,SDS_CMainWindow->m_pDragVarsCur->pt2,fr2,cp,&rad,&sa,&ea)) {
						SDS_CURDWG->ucs2ent(cp,cp,SDS_CMainWindow->m_pDragVarsCur->xfa);
						sa+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&sa,NULL);
						ea+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&ea,NULL);

										if(mode==2) {
												SDS_CMainWindow->m_pDragVarsCur->rblst=sds_buildlist(10,cp,40,rad,50,sa,51,ea,0);
												break;
										}
						if(dragit) {	 // FUNCTION ?
							if(SDS_CMainWindow->m_pDragVarsCur->elp) {
								SDS_CMainWindow->m_pDragVarsCur->elp->set_10(cp);
								SDS_CMainWindow->m_pDragVarsCur->elp->set_40(rad);
								SDS_CMainWindow->m_pDragVarsCur->elp->set_50(sa);
								SDS_CMainWindow->m_pDragVarsCur->elp->set_51(ea);
								ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
								ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
								SDS_UpdateEntDisp(ename,0);
								sds_redraw(ename,IC_REDRAW_XOR);
							} else {
								SDS_DrawArc(-1,dashmode,0,SDS_CMainWindow->m_pDragVarsCur->scrht,cp[0],cp[1],rad,sa,ea,0.0);
							}
						}				 // FUNCTION ?
								}
								SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt1,opt,-1,dashmode,0,SDS_CURDWG,0);
								break;
						case 13:	// ARC			start/end/[radius]
								if(SDS_CMainWindow->m_pDragVarsCur->gotnub && mode==2) {
										rad=SDS_CMainWindow->m_pDragVarsCur->nub;
								} else {
										rad=sds_distance(SDS_CMainWindow->m_pDragVarsCur->pt2,opt);
								}
								cp[0]=(SDS_CMainWindow->m_pDragVarsCur->pt1[0]+SDS_CMainWindow->m_pDragVarsCur->pt2[0])/2.0;
								cp[1]=(SDS_CMainWindow->m_pDragVarsCur->pt1[1]+SDS_CMainWindow->m_pDragVarsCur->pt2[1])/2.0;
								cp[2]=SDS_CMainWindow->m_pDragVarsCur->pt1[2];
								fr1=sds_distance(SDS_CMainWindow->m_pDragVarsCur->pt1,SDS_CMainWindow->m_pDragVarsCur->pt2)/2.0;
								if(fabs(rad)<fr1) { if(rad<0.0) rad=-fr1; else rad=fr1; }
								sds_polar(cp,sds_angle(SDS_CMainWindow->m_pDragVarsCur->pt1,SDS_CMainWindow->m_pDragVarsCur->pt2)+(IC_PI/2.0)*((rad<0.0) ? -1.0 : 1.0),sqrt(rad*rad-fr1*fr1),cp);
								sa=sds_angle(cp,SDS_CMainWindow->m_pDragVarsCur->pt1);
								ea=sds_angle(cp,SDS_CMainWindow->m_pDragVarsCur->pt2);
								if(rad<0.0) rad=fabs(rad);

					SDS_CURDWG->ucs2ent(cp,cp,SDS_CMainWindow->m_pDragVarsCur->xfa);
								sa+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&sa,NULL);
								ea+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&ea,NULL);

								if(mode==2) {
										SDS_CMainWindow->m_pDragVarsCur->rblst=sds_buildlist(10,cp,40,rad,50,sa,51,ea,0);
										break;													 // +(rad<0.0) ? 2.0*IC_PI : 0.0
								}
								if(dragit) {	 // FUNCTION ?
						if(SDS_CMainWindow->m_pDragVarsCur->elp) {
							SDS_CMainWindow->m_pDragVarsCur->elp->set_10(cp);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_40(rad);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_50(sa);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_51(ea);
							ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
							ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
							SDS_UpdateEntDisp(ename,0);
							sds_redraw(ename,IC_REDRAW_XOR);
						} else {
							SDS_DrawArc(-1,dashmode,0,SDS_CMainWindow->m_pDragVarsCur->scrht,cp[0],cp[1],rad,sa,ea,0.0);
						}
					}				 // FUNCTION ?
								SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt2,opt,-1,dashmode,0,SDS_CURDWG,0);
								break;
						case 14:	// ARC			start/end/[center]
								sa=sds_angle(opt,SDS_CMainWindow->m_pDragVarsCur->pt1);
								ea=sds_angle(opt,SDS_CMainWindow->m_pDragVarsCur->pt2);
								rad=sds_distance(SDS_CMainWindow->m_pDragVarsCur->pt1,opt);

					SDS_CURDWG->ucs2ent(opt,cp,SDS_CMainWindow->m_pDragVarsCur->xfa);
								sa+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&sa,NULL);
								ea+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&ea,NULL);

								if(mode==2) {
										SDS_CMainWindow->m_pDragVarsCur->rblst=sds_buildlist(10,cp,40,rad,50,sa,51,ea,0);
										break;
								}
								if(dragit) {	 // FUNCTION ?
						if(SDS_CMainWindow->m_pDragVarsCur->elp) {
							SDS_CMainWindow->m_pDragVarsCur->elp->set_10(cp);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_40(rad);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_50(sa);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_51(ea);
							ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
							ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
							SDS_UpdateEntDisp(ename,0);
							sds_redraw(ename,IC_REDRAW_XOR);
						} else {
							SDS_DrawArc(-1,dashmode,0,SDS_CMainWindow->m_pDragVarsCur->scrht,cp[0],cp[1],rad,sa,ea,0.0);
						}
					}				 // FUNCTION ?
								break;
						case 15:	// ARC			start/direction/[end]
								if(!ic_2pta2arc(SDS_CMainWindow->m_pDragVarsCur->pt1,opt,SDS_CMainWindow->m_pDragVarsCur->ang,cp,&rad,&sa,&ea)) {

						SDS_CURDWG->ucs2ent(cp,cp,SDS_CMainWindow->m_pDragVarsCur->xfa);
						sa+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&sa,NULL);
						ea+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&ea,NULL);

										if(mode==2) {
												SDS_CMainWindow->m_pDragVarsCur->rblst=sds_buildlist(10,cp,40,rad,50,sa,51,ea,0);
												break;
										}
						if(dragit) {	 // FUNCTION ?
							if(SDS_CMainWindow->m_pDragVarsCur->elp) {
								SDS_CMainWindow->m_pDragVarsCur->elp->set_10(cp);
								SDS_CMainWindow->m_pDragVarsCur->elp->set_40(rad);
								SDS_CMainWindow->m_pDragVarsCur->elp->set_50(sa);
								SDS_CMainWindow->m_pDragVarsCur->elp->set_51(ea);
								ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
								ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
								SDS_UpdateEntDisp(ename,0);
								sds_redraw(ename,IC_REDRAW_XOR);
							} else {
								SDS_DrawArc(-1,dashmode,0,SDS_CMainWindow->m_pDragVarsCur->scrht,cp[0],cp[1],rad,sa,ea,0.0);
							}
						}				 // FUNCTION ?
								}
								SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt1,opt,-1,dashmode,0,SDS_CURDWG,0);
								break;
						case 16: // POLYGON  center/n sides/[radius] (inscribed)
								if(SDS_CMainWindow->m_pDragVarsCur->ang<3.0) return(RTNORM);
								if(SDS_CMainWindow->m_pDragVarsCur->gotnub && mode==2) {
						SDS_getvar(NULL,DB_QSNAPANG,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
						fr1=((((2.0*IC_PI)/SDS_CMainWindow->m_pDragVarsCur->ang)/2.0)+(rb.resval.rreal-IC_PI/2.0));
										sds_polar(SDS_CMainWindow->m_pDragVarsCur->pt1,fr1,SDS_CMainWindow->m_pDragVarsCur->nub,opt);
								}
								sa=sds_angle(SDS_CMainWindow->m_pDragVarsCur->pt1,opt);
								sds_polar(SDS_CMainWindow->m_pDragVarsCur->pt1,sa,sds_distance(SDS_CMainWindow->m_pDragVarsCur->pt1,opt),SDS_CMainWindow->m_pDragVarsCur->pt2);
								for(fr1=0.0,ct=0; ct<=(int)SDS_CMainWindow->m_pDragVarsCur->ang; ++ct,fr1+=(IC_PI*2.0)/SDS_CMainWindow->m_pDragVarsCur->ang) {
										sds_polar(SDS_CMainWindow->m_pDragVarsCur->pt1,fr1+sa,sds_distance(SDS_CMainWindow->m_pDragVarsCur->pt1,opt),cp);
										if(mode==2) {
												if(ct==0) rbb=rbc=sds_buildlist(10,cp,0);
												else			rbc=rbc->rbnext=sds_buildlist(10,cp,0);
										}
										if(dragit && mode!=2) SDS_DrawLine(cp,SDS_CMainWindow->m_pDragVarsCur->pt2,SDS_CMainWindow->m_pDragVarsCur->color,dashmode,1,SDS_CURDWG,0);
										ic_ptcpy(SDS_CMainWindow->m_pDragVarsCur->pt2,cp);
						if(mode==2 && 1==((int)SDS_CMainWindow->m_pDragVarsCur->ang)-ct)break;
								}
								if(mode==2) SDS_CMainWindow->m_pDragVarsCur->rblst=rbb;
								if(mode!=2) SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt1,opt,-1,dashmode,0,SDS_CURDWG,0);
								break;
						case 17: // POLYGON  center/n sides/[radius] (cirscribed)
								if(SDS_CMainWindow->m_pDragVarsCur->ang<3.0) return(RTNORM);
								if(SDS_CMainWindow->m_pDragVarsCur->gotnub && mode==2) {
						SDS_getvar(NULL,DB_QSNAPANG,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
										fr1=(rb.resval.rreal-IC_PI/2.0);
										sds_polar(SDS_CMainWindow->m_pDragVarsCur->pt1,fr1,SDS_CMainWindow->m_pDragVarsCur->nub,opt);
								}
								sa=sds_angle(SDS_CMainWindow->m_pDragVarsCur->pt1,opt);
								fr1=((IC_PI*2.0)/SDS_CMainWindow->m_pDragVarsCur->ang)/2.0;
								sds_polar(SDS_CMainWindow->m_pDragVarsCur->pt1,fr1+sa,sds_distance(SDS_CMainWindow->m_pDragVarsCur->pt1,opt)/cos(IC_PI/SDS_CMainWindow->m_pDragVarsCur->ang),SDS_CMainWindow->m_pDragVarsCur->pt2);
								for(ct=0; ct<=(int)SDS_CMainWindow->m_pDragVarsCur->ang; ++ct,fr1+=(IC_PI*2.0)/SDS_CMainWindow->m_pDragVarsCur->ang) {
										sds_polar(SDS_CMainWindow->m_pDragVarsCur->pt1,fr1+sa,sds_distance(SDS_CMainWindow->m_pDragVarsCur->pt1,opt)/cos(IC_PI/SDS_CMainWindow->m_pDragVarsCur->ang),cp);
										if(mode==2) {
												if(ct==0) rbb=rbc=sds_buildlist(10,cp,0);
												else			rbc=rbc->rbnext=sds_buildlist(10,cp,0);
										}
										if(dragit && mode!=2) SDS_DrawLine(cp,SDS_CMainWindow->m_pDragVarsCur->pt2,SDS_CMainWindow->m_pDragVarsCur->color,dashmode,1,SDS_CURDWG,0);
										ic_ptcpy(SDS_CMainWindow->m_pDragVarsCur->pt2,cp);
						if(mode==2 && 1==((int)SDS_CMainWindow->m_pDragVarsCur->ang)-ct)break;
								}
								if(mode==2) SDS_CMainWindow->m_pDragVarsCur->rblst=rbb;
								else SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt1,opt,-1,dashmode,0,SDS_CURDWG,0);
								break;
						case 18:	// LINE 		first/[second]
					SDS_CURDWG->ucs2ent(SDS_CMainWindow->m_pDragVarsCur->pt1,tpt1,SDS_CMainWindow->m_pDragVarsCur->xfa);
					SDS_CURDWG->ucs2ent(opt,tpt2,SDS_CMainWindow->m_pDragVarsCur->xfa);
								if(mode==2) {
										SDS_CMainWindow->m_pDragVarsCur->rblst=sds_buildlist(10,tpt1,11,tpt2,0);
										break;
								}
					if(SDS_CMainWindow->m_pDragVarsCur->elp) {
						SDS_CMainWindow->m_pDragVarsCur->elp->set_10(tpt1);
						SDS_CMainWindow->m_pDragVarsCur->elp->set_11(tpt2);
						ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
						ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
						SDS_UpdateEntDisp(ename,0);
						sds_redraw(ename,IC_REDRAW_XOR);
					} else {
						SDS_DrawLine(tpt1,tpt2,-1,dashmode,0,SDS_CURDWG,0);
					}
								break;
						case 19:	// POLYGON	edge/n sides/[side length]
								if(SDS_CMainWindow->m_pDragVarsCur->ang<3.0) return(RTNORM);
								if(SDS_CMainWindow->m_pDragVarsCur->gotnub && mode==2) {
										rad=SDS_CMainWindow->m_pDragVarsCur->nub;
								} else {
										rad=sds_distance(SDS_CMainWindow->m_pDragVarsCur->pt1,opt);
								}
								fr1=sds_angle(SDS_CMainWindow->m_pDragVarsCur->pt1,opt);
								ic_ptcpy(SDS_CMainWindow->m_pDragVarsCur->pt2,SDS_CMainWindow->m_pDragVarsCur->pt1);
								ic_ptcpy(lp,opt);
								for(ct=0; ct<(int)SDS_CMainWindow->m_pDragVarsCur->ang; ++ct) {
						if(ct>0){
							ic_ptcpy(SDS_CMainWindow->m_pDragVarsCur->pt2,lp);
							sds_polar(lp,(fr1+=(IC_PI*2.0)/SDS_CMainWindow->m_pDragVarsCur->ang),rad,lp);
						}
										if(dragit && mode!=2) SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt2,lp,SDS_CMainWindow->m_pDragVarsCur->color,dashmode,1,SDS_CURDWG,0);
										if(mode==2) {
												if(ct==0){
								rbb=sds_buildlist(10,SDS_CMainWindow->m_pDragVarsCur->pt2,10,lp,0);
								rbc=rbb->rbnext;
												}else rbc=rbc->rbnext=sds_buildlist(10,lp,0);
										}
								}
								if(mode==2) SDS_CMainWindow->m_pDragVarsCur->rblst=rbb;
								break;
						case 20:	// 20 	CIRCLE	 center [diameter]
								if(SDS_CMainWindow->m_pDragVarsCur->gotnub && mode==2) {
										rad=SDS_CMainWindow->m_pDragVarsCur->nub/2.0;
								} else {
										rad=sds_distance(SDS_CMainWindow->m_pDragVarsCur->pt1,opt)/2.0;
								}
								if(0.0==rad &&
						(SDSApplication::GetActiveApplication()->GetInputControlBits() & RSG_NOZERO))
						{
						break;
						}
					SDS_CURDWG->ucs2ent(SDS_CMainWindow->m_pDragVarsCur->pt1,cp,SDS_CMainWindow->m_pDragVarsCur->xfa);
								if(mode==2) {
										SDS_CMainWindow->m_pDragVarsCur->rblst=sds_buildlist(10,cp,40,rad,0);
										break;
								}
								if(dragit) {	 // FUNCTION ?
						if(SDS_CMainWindow->m_pDragVarsCur->elp) {
							SDS_CMainWindow->m_pDragVarsCur->elp->set_10(cp);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_40(rad);
							ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
							ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
							SDS_UpdateEntDisp(ename,0);
							sds_redraw(ename,IC_REDRAW_XOR);
						} else {
							SDS_DrawArc(-1,dashmode,2,SDS_CMainWindow->m_pDragVarsCur->scrht,cp[0],cp[1],rad,0.0,0.0,0.0);
						}
					}				 // FUNCTION ?
								SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt1,opt,-1,dashmode,0,SDS_CURDWG,0);
								break;
						case 21:	// RECTANG	first corner [other corner]
								tpt1[2]=tpt2[2]=SDS_CMainWindow->m_pDragVarsCur->pt1[2];

								tpt1[0]=SDS_CMainWindow->m_pDragVarsCur->pt1[0];
								tpt1[1]=SDS_CMainWindow->m_pDragVarsCur->pt1[1];
								tpt2[0]=SDS_CMainWindow->m_pDragVarsCur->pt1[0];
								tpt2[1]=opt[1];
								if(mode!=2) SDS_DrawLine(tpt1,tpt2,SDS_CMainWindow->m_pDragVarsCur->color,dashmode,1,SDS_CURDWG,0);

								tpt1[0]=SDS_CMainWindow->m_pDragVarsCur->pt1[0];
								tpt1[1]=opt[1];
								tpt2[0]=opt[0];
								tpt2[1]=opt[1];
								if(mode!=2) SDS_DrawLine(tpt1,tpt2,SDS_CMainWindow->m_pDragVarsCur->color,dashmode,1,SDS_CURDWG,0);

								tpt1[0]=opt[0];
								tpt1[1]=opt[1];
								tpt2[0]=opt[0];
								tpt2[1]=SDS_CMainWindow->m_pDragVarsCur->pt1[1];
								if(mode!=2) SDS_DrawLine(tpt1,tpt2,SDS_CMainWindow->m_pDragVarsCur->color,dashmode,1,SDS_CURDWG,0);

								tpt1[0]=opt[0];
								tpt1[1]=SDS_CMainWindow->m_pDragVarsCur->pt1[1];
								tpt2[0]=SDS_CMainWindow->m_pDragVarsCur->pt1[0];
								tpt2[1]=SDS_CMainWindow->m_pDragVarsCur->pt1[1];
								if(mode!=2) SDS_DrawLine(tpt1,tpt2,SDS_CMainWindow->m_pDragVarsCur->color,dashmode,1,SDS_CURDWG,0);
								break;
						case 22:	// DONUT		inside dia/outside dia/[cenpt]
								if(dragit) {
										SDS_DrawArc(-1,dashmode,2,SDS_CMainWindow->m_pDragVarsCur->scrht,opt[0],opt[1],(SDS_CMainWindow->m_pDragVarsCur->pt1[0])/2.0,0.0,0.0,0.0);
										SDS_DrawArc(-1,dashmode,2,SDS_CMainWindow->m_pDragVarsCur->scrht,opt[0],opt[1],(SDS_CMainWindow->m_pDragVarsCur->pt1[1])/2.0,0.0,0.0,0.0);
								}
								break;
						case 23:	// ARC			start/radius/[end]
								rad=SDS_CMainWindow->m_pDragVarsCur->ang;
								if(rad<0.0) {
										fr1=(-1.0);
										rad=(-rad);
								} else fr1=1.0;
								if((fr2=sds_distance(SDS_CMainWindow->m_pDragVarsCur->pt1,opt))>(2.0*rad)) {
										// *** If the distance between the two points is greater than twice the radius
										// *** then create a semi-circle
										sds_polar(SDS_CMainWindow->m_pDragVarsCur->pt1,sds_angle(SDS_CMainWindow->m_pDragVarsCur->pt1,opt),(2.0*rad),tpt1);
										cp[0]=((tpt1[0]+SDS_CMainWindow->m_pDragVarsCur->pt1[0])/2.0); cp[1]=((tpt1[1]+SDS_CMainWindow->m_pDragVarsCur->pt1[1])/2.0); cp[2]=SDS_CMainWindow->m_pDragVarsCur->pt1[2];
								} else {
										fr2=sqrt((rad*rad)-((fr2/2.0)*(fr2/2.0))); // *** Distance from mid-point of start and endpoints to the center point
										tpt1[0]=((opt[0]+SDS_CMainWindow->m_pDragVarsCur->pt1[0])/2.0); tpt1[1]=((opt[1]+SDS_CMainWindow->m_pDragVarsCur->pt1[1])/2.0); tpt1[2]=SDS_CMainWindow->m_pDragVarsCur->pt1[2];
										// *** Normalize the angles
										tang1=sds_angle(SDS_CMainWindow->m_pDragVarsCur->pt1,opt); tang2=(IC_PI/2.0);
										ic_normang(&tang1,&tang2);
										// *** Get the center point
										if(fr1<0.0) sds_polar(tpt1,(tang1-tang2),fr2,cp);
										else sds_polar(tpt1,(tang1+tang2),fr2,cp);
								}
								sa=sds_angle(cp,SDS_CMainWindow->m_pDragVarsCur->pt1);
								ea=sds_angle(cp,opt);

					SDS_CURDWG->ucs2ent(cp,cp,SDS_CMainWindow->m_pDragVarsCur->xfa);
								sa+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&sa,NULL);
								ea+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&ea,NULL);

								if(mode==2) {
										SDS_CMainWindow->m_pDragVarsCur->rblst=sds_buildlist(10,cp,40,rad,50,sa,51,ea,0);
										break;
								}
								if(dragit) {	 // FUNCTION ?
						if(SDS_CMainWindow->m_pDragVarsCur->elp) {
							SDS_CMainWindow->m_pDragVarsCur->elp->set_10(cp);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_40(rad);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_50(sa);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_51(ea);
							ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
							ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
							SDS_UpdateEntDisp(ename,0);
							sds_redraw(ename,IC_REDRAW_XOR);
						} else {
							SDS_DrawArc(-1,dashmode,0,SDS_CMainWindow->m_pDragVarsCur->scrht,cp[0],cp[1],rad,sa,ea,0.0);
						}
					}				 // FUNCTION ?
								SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt1,opt,-1,dashmode,0,SDS_CURDWG,0);
								break;
						case 24:	// ARC			start/radius/angle/[dir-of-chord]
								if(SDS_CMainWindow->m_pDragVarsCur->gotnub && mode==2) {
										cdir=SDS_CMainWindow->m_pDragVarsCur->nub;
								} else {
										cdir=sds_angle(SDS_CMainWindow->m_pDragVarsCur->pt1,opt);
								}
								rad=SDS_CMainWindow->m_pDragVarsCur->ang;
								iang=SDS_CMainWindow->m_pDragVarsCur->pt2[0];

								opt[2]=tpt1[2]=SDS_CMainWindow->m_pDragVarsCur->pt1[2];  /* Make sure elevations are the same. "tpt1" is the endpoint of the arc. */
								if (rad<0.0) rad=-rad;
								if(iang<0.0) { // *** fr1 is the direction of the arc - 1.0 (CCW); -1.0 (CW)
										fr1=(-1.0);
										iang=(-iang);
								} else fr1=1.0;
								ic_normang(&iang,NULL);
								fr2=2.0*rad*sin(iang/2.0); // *** fr2 is the chord length
								if (icadRealEqual(fr2,0.0)) break; /* Invalid; rad==0.0 or normalized iang==0.0 */
								tpt1[0]=SDS_CMainWindow->m_pDragVarsCur->pt1[0]+fr2*cos(cdir); tpt1[1]=SDS_CMainWindow->m_pDragVarsCur->pt1[1]+fr2*sin(cdir);
								bulge=fr1*tan(iang/4.0);
								ic_bulge2arc(SDS_CMainWindow->m_pDragVarsCur->pt1,tpt1,bulge,cp,&rad,&sa,&ea);	/* Should return 0 */

					SDS_CURDWG->ucs2ent(cp,cp,SDS_CMainWindow->m_pDragVarsCur->xfa);
								sa+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&sa,NULL);
								ea+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&ea,NULL);

								if(mode==2) {
										SDS_CMainWindow->m_pDragVarsCur->rblst=sds_buildlist(10,cp,40,rad,50,sa,51,ea,0);
										break;
								}
								if(dragit) {	 // FUNCTION ?
						if(SDS_CMainWindow->m_pDragVarsCur->elp) {
							SDS_CMainWindow->m_pDragVarsCur->elp->set_10(cp);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_40(rad);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_50(sa);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_51(ea);
							ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
							ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
							SDS_UpdateEntDisp(ename,0);
							sds_redraw(ename,IC_REDRAW_XOR);
						} else {
							SDS_DrawArc(-1,dashmode,0,SDS_CMainWindow->m_pDragVarsCur->scrht,cp[0],cp[1],rad,sa,ea,0.0);
						}
					}				 // FUNCTION ?
								SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt1,opt,-1,dashmode,0,SDS_CURDWG,0);
								break;
						case 25:	// ARC			start/angle/[end]
								iang=SDS_CMainWindow->m_pDragVarsCur->ang;
								opt[2]=SDS_CMainWindow->m_pDragVarsCur->pt1[2];  /* Make sure elevations are the same. "opt" is the endpoint of the arc. */
								if(iang<0.0) { // *** fr1 is the direction of the arc - 1.0 (CCW); -1.0 (CW)
										fr1=(-1.0);
										iang=(-iang);
								} else fr1=1.0;
								ic_normang(&iang,NULL);
								if (SDS_CMainWindow->m_pDragVarsCur->pt1[0]==opt[0] && SDS_CMainWindow->m_pDragVarsCur->pt1[1]==opt[1]) break; /* Invalid; coincident pts */
								if (icadAngleEqual(iang,0.0)) break; /* Invalid; normalized include angle is 0.0. */
								bulge=fr1*tan(iang/4.0);
								ic_bulge2arc(SDS_CMainWindow->m_pDragVarsCur->pt1,opt,bulge,cp,&rad,&sa,&ea);  /* Should return 0 */

					SDS_CURDWG->ucs2ent(cp,cp,SDS_CMainWindow->m_pDragVarsCur->xfa);
								sa+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&sa,NULL);
								ea+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&ea,NULL);

								if(mode==2) {
										SDS_CMainWindow->m_pDragVarsCur->rblst=sds_buildlist(10,cp,40,rad,50,sa,51,ea,0);
										break;
								}
								if(dragit) {	 // FUNCTION ?
						if(SDS_CMainWindow->m_pDragVarsCur->elp) {
							SDS_CMainWindow->m_pDragVarsCur->elp->set_10(cp);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_40(rad);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_50(sa);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_51(ea);
							ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
							ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
							SDS_UpdateEntDisp(ename,0);
							sds_redraw(ename,IC_REDRAW_XOR);
						} else {
							SDS_DrawArc(-1,dashmode,0,SDS_CMainWindow->m_pDragVarsCur->scrht,cp[0],cp[1],rad,sa,ea,0.0);
						}
					}				 // FUNCTION ?
								SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt1,opt,-1,dashmode,0,SDS_CURDWG,0);
								break;
						case 26:	// ARC			start/angle/[cenpt]
								iang=SDS_CMainWindow->m_pDragVarsCur->ang;
								opt[2]=SDS_CMainWindow->m_pDragVarsCur->pt1[2];  /* Make sure elevations are the same. */
								if(iang<0.0) { // *** fr1 is the direction of the arc - 1.0 (CCW); -1.0 (CW)
										fr1=(-1.0);
										iang=(-iang);
								} else fr1=1.0;
								ic_normang(&iang,NULL);
								rad=sqrt((SDS_CMainWindow->m_pDragVarsCur->pt1[0]-opt[0])*(SDS_CMainWindow->m_pDragVarsCur->pt1[0]-opt[0])+
												 (SDS_CMainWindow->m_pDragVarsCur->pt1[1]-opt[1])*(SDS_CMainWindow->m_pDragVarsCur->pt1[1]-opt[1]));
								if (icadAngleEqual(rad,0.0) || icadAngleEqual(iang,0.0)) break;  /* Invalid; rad or normalized iang is 0.0. */
								sa=sds_angle(opt,SDS_CMainWindow->m_pDragVarsCur->pt1);
								if (fr1>0.0) ea=sa+iang; else { ea=sa; sa=ea-iang; }
								ic_normang(&sa,NULL); ic_normang(&ea,NULL);
								ic_ptcpy(cp,opt);

					SDS_CURDWG->ucs2ent(cp,cp,SDS_CMainWindow->m_pDragVarsCur->xfa);
								sa+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&sa,NULL);
								ea+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&ea,NULL);

								if(mode==2) {
										SDS_CMainWindow->m_pDragVarsCur->rblst=sds_buildlist(10,cp,40,rad,50,sa,51,ea,0);
										break;
								}
								if(dragit) {	 // FUNCTION ?
						if(SDS_CMainWindow->m_pDragVarsCur->elp) {
							SDS_CMainWindow->m_pDragVarsCur->elp->set_10(cp);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_40(rad);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_50(sa);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_51(ea);
							ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
							ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
							SDS_UpdateEntDisp(ename,0);
							sds_redraw(ename,IC_REDRAW_XOR);
						} else {
							SDS_DrawArc(-1,dashmode,0,SDS_CMainWindow->m_pDragVarsCur->scrht,cp[0],cp[1],rad,sa,ea,0.0);
						}
					}				 // FUNCTION ?
								break;
						case 27:	 // LINE		 start/length/[angle]
								if(SDS_CMainWindow->m_pDragVarsCur->gotnub && mode==2) {
										fr1=SDS_CMainWindow->m_pDragVarsCur->nub;
								} else {
										fr1=sds_angle(SDS_CMainWindow->m_pDragVarsCur->pt1,opt);
								}
								sds_polar(SDS_CMainWindow->m_pDragVarsCur->pt1,fr1,SDS_CMainWindow->m_pDragVarsCur->ang,cp);
					SDS_CURDWG->ucs2ent(cp,cp,SDS_CMainWindow->m_pDragVarsCur->xfa);
					SDS_CURDWG->ucs2ent(SDS_CMainWindow->m_pDragVarsCur->pt1,tpt1,SDS_CMainWindow->m_pDragVarsCur->xfa);
								if(mode==2) {
										SDS_CMainWindow->m_pDragVarsCur->rblst=sds_buildlist(10,tpt1,11,cp,0);
										break;
								}
								if(dragit) {	 // FUNCTION ?
						if(SDS_CMainWindow->m_pDragVarsCur->elp) {
							SDS_CMainWindow->m_pDragVarsCur->elp->set_10(tpt1);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_11(cp);
							ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
							ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
							SDS_UpdateEntDisp(ename,0);
							sds_redraw(ename,IC_REDRAW_XOR);
						} else {
							SDS_DrawLine(tpt1,cp,-1,dashmode,0,SDS_CURDWG,0);
						}
					}				 // FUNCTION ?
								break;
						case 28:	// ARC			start/end/[second]
					//if SDS_CMainWindow->m_pDragVarsCur->ang is non-zero, we're dragging the extruded edge
					if(dragit && mode!=2 && SDS_CMainWindow->m_pDragVarsCur->elp && fabs(SDS_CMainWindow->m_pDragVarsCur->ang)>IC_ZRO){
						sds_point extru;
						SDS_CMainWindow->m_pDragVarsCur->elp->get_210(extru);
						opt[0]-=extru[0]*SDS_CMainWindow->m_pDragVarsCur->ang;
						opt[1]-=extru[1]*SDS_CMainWindow->m_pDragVarsCur->ang;
						opt[2]-=extru[2]*SDS_CMainWindow->m_pDragVarsCur->ang;
					}
								if(!ic_3pt2arc(SDS_CMainWindow->m_pDragVarsCur->pt1,opt,SDS_CMainWindow->m_pDragVarsCur->pt2,cp,&rad,&sa,&ea)) {

						SDS_CURDWG->ucs2ent(cp,cp,SDS_CMainWindow->m_pDragVarsCur->xfa);
						sa+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&sa,NULL);
						ea+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&ea,NULL);

										if(mode==2) {
												SDS_CMainWindow->m_pDragVarsCur->rblst=sds_buildlist(10,cp,40,rad,50,sa,51,ea,0);
												break;
										}
						if(dragit) {	 // FUNCTION ?
							if(SDS_CMainWindow->m_pDragVarsCur->elp) {
								SDS_CMainWindow->m_pDragVarsCur->elp->set_10(cp);
								SDS_CMainWindow->m_pDragVarsCur->elp->set_40(rad);
								SDS_CMainWindow->m_pDragVarsCur->elp->set_50(sa);
								SDS_CMainWindow->m_pDragVarsCur->elp->set_51(ea);
								SDS_CMainWindow->m_pDragVarsCur->elp->set_39(SDS_CMainWindow->m_pDragVarsCur->ang);
								ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
								ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
								SDS_UpdateEntDisp(ename,0);
								sds_redraw(ename,IC_REDRAW_XOR);
							} else {
								SDS_DrawArc(-1,dashmode,0,SDS_CMainWindow->m_pDragVarsCur->scrht,cp[0],cp[1],rad,sa,ea,0.0);
							}
						}				 // FUNCTION ?
								}
								break;
						case 29:	// ARC			end/second/[start]
					//if SDS_CMainWindow->m_pDragVarsCur->ang is non-zero, we're dragging the extruded edge
					if(dragit && mode!=2 && SDS_CMainWindow->m_pDragVarsCur->elp && fabs(SDS_CMainWindow->m_pDragVarsCur->ang)>IC_ZRO){
						sds_point extru;
						SDS_CMainWindow->m_pDragVarsCur->elp->get_210(extru);
						opt[0]-=extru[0]*SDS_CMainWindow->m_pDragVarsCur->ang;
						opt[1]-=extru[1]*SDS_CMainWindow->m_pDragVarsCur->ang;
						opt[2]-=extru[2]*SDS_CMainWindow->m_pDragVarsCur->ang;
					}
								if(!ic_3pt2arc(opt,SDS_CMainWindow->m_pDragVarsCur->pt2,SDS_CMainWindow->m_pDragVarsCur->pt1,cp,&rad,&sa,&ea)) {

						SDS_CURDWG->ucs2ent(cp,cp,SDS_CMainWindow->m_pDragVarsCur->xfa);
						sa+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&sa,NULL);
						ea+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&ea,NULL);

										if(mode==2) {
												SDS_CMainWindow->m_pDragVarsCur->rblst=sds_buildlist(10,cp,40,rad,50,sa,51,ea,0);
												break;
										}
						if(dragit) {	 // FUNCTION ?
							if(SDS_CMainWindow->m_pDragVarsCur->elp) {
								SDS_CMainWindow->m_pDragVarsCur->elp->set_10(cp);
								SDS_CMainWindow->m_pDragVarsCur->elp->set_40(rad);
								SDS_CMainWindow->m_pDragVarsCur->elp->set_50(sa);
								SDS_CMainWindow->m_pDragVarsCur->elp->set_51(ea);
								SDS_CMainWindow->m_pDragVarsCur->elp->set_39(SDS_CMainWindow->m_pDragVarsCur->ang);
								ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
								ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
								SDS_UpdateEntDisp(ename,0);
								sds_redraw(ename,IC_REDRAW_XOR);
							} else {
								SDS_DrawArc(-1,dashmode,0,SDS_CMainWindow->m_pDragVarsCur->scrht,cp[0],cp[1],rad,sa,ea,0.0);
							}
						}				 // FUNCTION ?
								}
								break;
						case 30:	// LINE 		 start/end/[mid]
					cp[0]=0.5*(SDS_CMainWindow->m_pDragVarsCur->pt1[0]+SDS_CMainWindow->m_pDragVarsCur->pt2[0]);
					cp[1]=0.5*(SDS_CMainWindow->m_pDragVarsCur->pt1[1]+SDS_CMainWindow->m_pDragVarsCur->pt2[1]);
					cp[2]=0.5*(SDS_CMainWindow->m_pDragVarsCur->pt1[2]+SDS_CMainWindow->m_pDragVarsCur->pt2[2]);
					tpt1[0]=SDS_CMainWindow->m_pDragVarsCur->pt1[0]+opt[0]-cp[0];
					tpt1[1]=SDS_CMainWindow->m_pDragVarsCur->pt1[1]+opt[1]-cp[1];
					tpt1[2]=SDS_CMainWindow->m_pDragVarsCur->pt1[2]+opt[2]-cp[2];
					tpt2[0]=SDS_CMainWindow->m_pDragVarsCur->pt2[0]+opt[0]-cp[0];
					tpt2[1]=SDS_CMainWindow->m_pDragVarsCur->pt2[1]+opt[1]-cp[1];
					tpt2[2]=SDS_CMainWindow->m_pDragVarsCur->pt2[2]+opt[2]-cp[2];
					//old code below is crap
							//sds_polar(SDS_CMainWindow->m_pDragVarsCur->pt1,
					//			sds_angle(SDS_CMainWindow->m_pDragVarsCur->pt1,SDS_CMainWindow->m_pDragVarsCur->pt2),
					//			sds_distance(SDS_CMainWindow->m_pDragVarsCur->pt1,SDS_CMainWindow->m_pDragVarsCur->pt2)/2.0,cp);
					//sds_polar(SDS_CMainWindow->m_pDragVarsCur->pt1,sds_angle(cp,opt),sds_distance(cp,opt),tpt1);
					//sds_polar(SDS_CMainWindow->m_pDragVarsCur->pt2,sds_angle(cp,opt),sds_distance(cp,opt),tpt2);


								if(mode==2) {
						SDS_CURDWG->ucs2ent(tpt1,tpt1,SDS_CMainWindow->m_pDragVarsCur->xfa);
						SDS_CURDWG->ucs2ent(tpt2,tpt2,SDS_CMainWindow->m_pDragVarsCur->xfa);
										SDS_CMainWindow->m_pDragVarsCur->rblst=sds_buildlist(10,tpt1,11,tpt2,0);
										break;
								}
								SDS_DrawLine(tpt1,tpt2,-1,dashmode,0,SDS_CURDWG,0);
								break;
						case 31:	// ARC			end/center/[end]
								ea=sds_angle(SDS_CMainWindow->m_pDragVarsCur->pt2,SDS_CMainWindow->m_pDragVarsCur->pt1);
								sa=sds_angle(SDS_CMainWindow->m_pDragVarsCur->pt2,opt);
								rad=sds_distance(SDS_CMainWindow->m_pDragVarsCur->pt2,SDS_CMainWindow->m_pDragVarsCur->pt1);

					SDS_CURDWG->ucs2ent(SDS_CMainWindow->m_pDragVarsCur->pt2,cp,SDS_CMainWindow->m_pDragVarsCur->xfa);
								sa+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&sa,NULL);
								ea+=SDS_CMainWindow->m_pDragVarsCur->dang; ic_normang(&ea,NULL);

								if(mode==2) {
										SDS_CMainWindow->m_pDragVarsCur->rblst=sds_buildlist(10,cp,40,rad,50,sa,51,ea,0);
										break;
								}
								if(dragit) {	 // FUNCTION ?
						if(SDS_CMainWindow->m_pDragVarsCur->elp) {
							SDS_CMainWindow->m_pDragVarsCur->elp->set_10(cp);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_40(rad);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_50(sa);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_51(ea);
							ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
							ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
							SDS_UpdateEntDisp(ename,0);
							sds_redraw(ename,IC_REDRAW_XOR);
						} else {
							SDS_DrawArc(-1,dashmode,0,SDS_CMainWindow->m_pDragVarsCur->scrht,cp[0],cp[1],rad,sa,ea,0.0);
						}
					}				 // FUNCTION ?
								SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt2,opt,-1,dashmode,0,SDS_CURDWG,0);
								break;
						case 32:	// GETPOINT first/[second]	 (w/o db_handitem)
								SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt1,opt,-1,dashmode,0,SDS_CURDWG,0);
								break;
						case 33:	// GETCORNER first corner [other corner]
					if(view==NULL) break;

								if(SDS_BoxModeCursor) {
						gr_ucs2rp(opt,tpt1,view);
						gr_ucs2rp(SDS_CMainWindow->m_pDragVarsCur->pt1,tpt2,view);
						if(tpt1[0]>tpt2[0]) {
							SDS_SetCursor(IDC_ICAD_WINDOW);
							dashmode=0; //EBATECH(FUTA)
						} else {
							SDS_SetCursor(IDC_ICAD_CROSSING);
							dashmode=1; //EBATECH(FUTA) use dashmode at crossing
						}
					}
					if(view==NULL) break;

								ic_ptcpy(tpt1,SDS_CMainWindow->m_pDragVarsCur->pt1);
								ic_ptcpy(tpt2,opt);
					gr_ucs2rp(tpt1,tpt1,view);
					gr_ucs2rp(tpt2,tpt2,view);

								p1[0]=tpt1[0]; p1[1]=tpt1[1]; gr_rp2ucs(p1,p1,view);
								p2[0]=tpt1[0]; p2[1]=tpt2[1]; gr_rp2ucs(p2,p2,view);
								p3[0]=tpt2[0]; p3[1]=tpt2[1]; gr_rp2ucs(p3,p3,view);
								p4[0]=tpt2[0]; p4[1]=tpt1[1]; gr_rp2ucs(p4,p4,view);

								if(mode!=2) {
						SDS_DrawLine(p1,p2,-1,dashmode,0,SDS_CURDWG,0);
						SDS_DrawLine(p2,p3,-1,dashmode,0,SDS_CURDWG,0);
						SDS_DrawLine(p3,p4,-1,dashmode,0,SDS_CURDWG,0);
						SDS_DrawLine(p4,p1,-1,dashmode,0,SDS_CURDWG,0);
					}
								break;
						case 34:	// ZOOM WIN  first corner [other corner]
					if(view==NULL) break;

								ic_ptcpy(tpt1,SDS_CMainWindow->m_pDragVarsCur->pt1);
								ic_ptcpy(tpt2,opt);
					gr_ucs2rp(tpt1,tpt1,view);
					gr_ucs2rp(tpt2,tpt2,view);

								p1[0]=tpt1[0]; p1[1]=tpt1[1]; gr_rp2ucs(p1,p1,view);
								p2[0]=tpt1[0]; p2[1]=tpt2[1]; gr_rp2ucs(p2,p2,view);
								p3[0]=tpt2[0]; p3[1]=tpt2[1]; gr_rp2ucs(p3,p3,view);
								p4[0]=tpt2[0]; p4[1]=tpt1[1]; gr_rp2ucs(p4,p4,view);

								if(mode!=2) {
						SDS_DrawLine(p1,p2,-1,dashmode,0,SDS_CURDWG,0);
						SDS_DrawLine(p2,p3,-1,dashmode,0,SDS_CURDWG,0);
						SDS_DrawLine(p3,p4,-1,dashmode,0,SDS_CURDWG,0);
						SDS_DrawLine(p4,p1,-1,dashmode,0,SDS_CURDWG,0);
					}
								break;
						case 35:	// GET POLYGON	first pt/second pt [third pt]
					SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt1,opt,-1,dashmode,0,SDS_CURDWG,0);
								SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt2,opt,-1,dashmode,0,SDS_CURDWG,0);
								break;
						case 36:	// VIEWPORT (drag left/right) first corner/second corner [edge]
					//TODO - these 2 modes (36 & 37)need to be more like mode 33 so the viewport is
					//	dragged otho in the WCS, not along the ucs. pt1 & pt2 here are wcs points
								SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt2,SDS_CMainWindow->m_pDragVarsCur->pt1,-1,dashmode,0,SDS_CURDWG,0);

								tpt1[2]=tpt2[2]=SDS_CMainWindow->m_pDragVarsCur->pt1[2];
								tpt1[0]=opt[0];
								tpt1[1]=SDS_CMainWindow->m_pDragVarsCur->pt1[1];
								tpt2[0]=opt[0];
								tpt2[1]=SDS_CMainWindow->m_pDragVarsCur->pt2[1];

								SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt1,tpt1,-1,dashmode,0,SDS_CURDWG,0);
								SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt2,tpt2,-1,dashmode,0,SDS_CURDWG,0);
								SDS_DrawLine(tpt1,tpt2,-1,dashmode,0,SDS_CURDWG,0);
								break;
						case 37:	// VIEWPORT (drag top/bottom) first corner/second corner [edge]
								SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt2,SDS_CMainWindow->m_pDragVarsCur->pt1,-1,dashmode,0,SDS_CURDWG,0);

								tpt1[2]=tpt2[2]=SDS_CMainWindow->m_pDragVarsCur->pt1[2];
								tpt1[0]=SDS_CMainWindow->m_pDragVarsCur->pt1[0];
								tpt1[1]=opt[1];
								tpt2[0]=SDS_CMainWindow->m_pDragVarsCur->pt2[0];
								tpt2[1]=opt[1];

								SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt1,tpt1,-1,dashmode,0,SDS_CURDWG,0);
								SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt2,tpt2,-1,dashmode,0,SDS_CURDWG,0);
								SDS_DrawLine(tpt1,tpt2,-1,dashmode,0,SDS_CURDWG,0);
								break;
						case 38:	 // LINE		 first/second/[length] for LENGHTEN
					//SDS_CURDWG->ucs2ent(opt,tpt1,SDS_CMainWindow->m_pDragVarsCur->xfa);
					gr_ucs2rp(SDS_CMainWindow->m_pDragVarsCur->pt1,p1,view);
					gr_ucs2rp(SDS_CMainWindow->m_pDragVarsCur->pt2,p2,view);
					gr_ucs2rp(opt,tpt1,view);
					fr1=cmd_dist2d(p1,p2);
					if(fr1>IC_ZRO){
						fr2=(p1[0]-p2[0])*(tpt1[0]-p2[0])+(p1[1]-p2[1])*(tpt1[1]-p2[1]);
						fr2=1.0-fr2/(fr1*fr1);
						SDS_CURDWG->ucs2ent(SDS_CMainWindow->m_pDragVarsCur->pt1,p1,SDS_CMainWindow->m_pDragVarsCur->xfa);
						SDS_CURDWG->ucs2ent(SDS_CMainWindow->m_pDragVarsCur->pt2,p2,SDS_CMainWindow->m_pDragVarsCur->xfa);
						p2[0] =p1[0] +fr2*(p2[0]-p1[0]);
						p2[1] =p1[1] +fr2*(p2[1]-p1[1]);
						p2[2] =p1[2] +fr2*(p2[2]-p1[2]);
						p3[0]=SDS_CMainWindow->m_pDragVarsCur->pt1[0]+fr2*(SDS_CMainWindow->m_pDragVarsCur->pt2[0]-SDS_CMainWindow->m_pDragVarsCur->pt1[0]);
						p3[1]=SDS_CMainWindow->m_pDragVarsCur->pt1[1]+fr2*(SDS_CMainWindow->m_pDragVarsCur->pt2[1]-SDS_CMainWindow->m_pDragVarsCur->pt1[1]);
						p3[2]=SDS_CMainWindow->m_pDragVarsCur->pt1[2]+fr2*(SDS_CMainWindow->m_pDragVarsCur->pt2[2]-SDS_CMainWindow->m_pDragVarsCur->pt1[2]);
					}else{
						SDS_CURDWG->ucs2ent(SDS_CMainWindow->m_pDragVarsCur->pt1,p1,SDS_CMainWindow->m_pDragVarsCur->xfa);
						SDS_CURDWG->ucs2ent(SDS_CMainWindow->m_pDragVarsCur->pt2,p2,SDS_CMainWindow->m_pDragVarsCur->xfa);
					}
								if(mode==2) {
										SDS_CMainWindow->m_pDragVarsCur->rblst=sds_buildlist(10,p1,11,p2,0);
										break;
								}
					SDS_DrawLine(opt,SDS_CMainWindow->m_pDragVarsCur->pt2,-1,dashmode,0,SDS_CURDWG,0);
					if(fr1>IC_ZRO)SDS_DrawLine(p3,SDS_CMainWindow->m_pDragVarsCur->pt1,-1,dashmode,0,SDS_CURDWG,0);

								break;



					if(SDS_CMainWindow->m_pDragVarsCur->gotnub && mode==2) {
										fr1=SDS_CMainWindow->m_pDragVarsCur->nub;
								} else {
										fr1=sds_distance(SDS_CMainWindow->m_pDragVarsCur->pt1,opt);
								}
								sds_polar(SDS_CMainWindow->m_pDragVarsCur->pt1,SDS_CMainWindow->m_pDragVarsCur->ang,fr1,cp);
					SDS_CURDWG->ucs2ent(SDS_CMainWindow->m_pDragVarsCur->pt1,tpt1,SDS_CMainWindow->m_pDragVarsCur->xfa);
					SDS_CURDWG->ucs2ent(cp,cp,SDS_CMainWindow->m_pDragVarsCur->xfa);
								if(mode==2) {
										SDS_CMainWindow->m_pDragVarsCur->rblst=sds_buildlist(10,tpt1,11,cp,0);
										break;
								}
								if(dragit) {	 // FUNCTION ?
						if(SDS_CMainWindow->m_pDragVarsCur->elp) {
							SDS_CMainWindow->m_pDragVarsCur->elp->set_10(tpt1);
							SDS_CMainWindow->m_pDragVarsCur->elp->set_11(cp);
							ename[0]=(long)SDS_CMainWindow->m_pDragVarsCur->elp;
							ename[1]=(long)SDS_CMainWindow->m_pDragVarsCur->flp;
							SDS_UpdateEntDisp(ename,0);
							sds_redraw(ename,IC_REDRAW_XOR);
						} else {
							SDS_DrawLine(tpt1,cp,-1,dashmode,0,SDS_CURDWG,0);
						}
					}				 // FUNCTION ?
					SDS_DrawLine(SDS_CMainWindow->m_pDragVarsCur->pt1,opt,-1,dashmode,0,SDS_CURDWG,0);
								break;
				}
				return(RTNORM);
		}

		struct resbuf *SDS_SSToVects(const sds_name ss, db_drawing *flp,struct gr_view *view) {
		/*
		**	Builds a llist of vectors from ss for dragging via sds_grvecs().
		*/
			int face,ndims,fi1,fi2, menttype;
				long ssidx;
				sds_name ename;
			sds_point pt1,pt2;
				struct resbuf *vll=NULL,*last;
			db_handitem *elp=NULL,*telp;
				struct gr_displayobject *tdo1,*beg,*end;
        double elev;

			if(ss==NULL || flp==NULL) goto out;

			if(view==NULL) view=SDS_CURGRVW;
			if(view==NULL) goto out;

				if ((last=vll=sds_buildlist(RTSHORT,256,0))==NULL) goto out;

			SDS_SetRegenListFlag(SDS_CURVIEW,SDS_CURDOC);

			for (ssidx=0L; sds_ssname(ss,ssidx,ename)==RTNORM; ssidx++) {

				elp=(db_handitem *)ename[0];
				menttype = elp->ret_type();

				telp=elp;
				//added cybage AJK on 03/09/2001
			//reason dragmode on ,failure
			//bugzilla bug no 77837[
			resbuf		rb;
			int			ret;
			if((ret = SDS_getvar(NULL, DB_QDRAGMODE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES)) != RTNORM)
				goto out;
			//Modified Cybage AW 03-12-01 [
			//Reason : When DRAGMODE = OFF, entity should not be displayed while dragging
			if(rb.resval.rint == 1 || rb.resval.rint == 0)//Modified Cybage AW 03-12-01 ]
				 continue;
			//]added cybage AJK on 03/09/2001
			if(SDS_getdispobjs(flp,&telp,&beg,&end,NULL,flp,view,cmd_InsideSds_draggen ? 2 : 1)) continue;

				for (tdo1=beg; tdo1!=NULL; tdo1=tdo1->next) {

					if (tdo1->npts<2 || tdo1->chain==NULL) continue;
					ndims=((tdo1->type&0x01)!=0)+2;
					face=((tdo1->type&0x02)!=0);
          // Ebatech(CNBR) -[ Bugzilla#78693 fill 0 in ndim=2 16-11-2003
          if(ndims==2)
          {
            if( elp->get_38(&elev) == false )
            {
              if( elp->get_10(pt1) == false )
                elev = 0.0;
              else
                elev = pt1[2];
            }
          }
          // EBATECH(CNBR) ]-

					for (fi1=0,fi2=(tdo1->npts+face)*ndims; fi1<fi2; fi1+=ndims) {
						if (face && fi1>=fi2-ndims) {
							pt1[0]=tdo1->chain[0];
							pt1[1]=tdo1->chain[1];
							if(ndims==3) pt1[2]=tdo1->chain[2];
							else	pt1[2]=elev;
						} else {
							pt1[0]=tdo1->chain[fi1];
							pt1[1]=tdo1->chain[fi1+1];
							if(ndims==3) pt1[2]=tdo1->chain[fi1+2];
							else	pt1[2]=elev;
						}
						/*D.Gavrilov*/// The following conditions dealing with
						// DB_MTEXT & DB_DIMENSION was added for non-WCS UCS
						// because of the "HACK HACK HACK" with p_mode in gr_getdispobjs().
						// So, it's a HACK too...
						if(!cmd_InsideSds_draggen ||
							menttype == DB_MTEXT || menttype == DB_DIMENSION)
							gr_rp2ucs(pt1,pt1,view);

						if(fi1==0) { ic_ptcpy(pt2,pt1); continue; }

						last->rbnext=sds_buildlist(RT3DPOINT,pt1,RT3DPOINT,pt2,0);
						while (last->rbnext!=NULL) last=last->rbnext;
						ic_ptcpy(pt2,pt1);
					}
				}
				gr_freedisplayobjectll(beg);
				}

			out:

				return vll;
		}


		int internalGetpoint(const sds_point ptReference, const char *szPointMsg, sds_point ptPoint, bool internalCall)
		{
//		// Modified CyberAge VSB 20/05/2001
//		bGetPoint = TRUE; //Used for Flyover Snapping
			if(!SDS_AtEntGet && !SDS_IsDragging && !SDS_AtAngDist) {
				char snapstr[IC_ACADBUF];
				int osnapmode;
				SDS_SetOsnapCursor(snapstr,&osnapmode,TRUE);
			}
			// Modified CyberAge AP 10/18/2000 [
			// To fix the CADR problem
			extern int LSP_ConvertToPoint;
			int preValue=LSP_ConvertToPoint;
			LSP_ConvertToPoint=2; // Must convert list into Point form.
			int ret=SDS_AskForPoint( ptReference,szPointMsg,ptPoint, internalCall);
			LSP_ConvertToPoint=preValue; // Restore the value
			// Modified CyberAge AP 10/18/2000 ]
			SDS_SkipLastOrthoSet=0;
//		// Modified CyberAge VSB 20/05/2001
//		bGetPoint = FALSE; //Used for Flyover Snapping
			return(ret);
		}


