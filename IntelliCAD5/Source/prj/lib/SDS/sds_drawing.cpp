/*	LIB\SDS\SDS_DRAWING.CPP 
 *	Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 *	Abstract
 *
 *  Printing/Drawing/Screen Changing Functions 
 *
 */

#include "Icad.h"/*DNT*/
#include "sdsapplication.h" /*DNT*/
#include "lisp.h"/*DNT*/
#include "cmds.h"/*DNT*/
#include "IcadTablet.h"/*DNT*/
#include "vxtabrec.h"
#include "IcadView.h"
#include "IcadEntityRenderer.h"

/****************************************************************************/
/************** Printing/Drawing/Screen Changing Functions ******************/
/****************************************************************************/


// *****************************************
// This is an SDS External API
//
int 
sdsengine_graphscr(void) 
	{

	return(SDS_CmdLineSize(SDS_GRAPHSCR));
	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_textpage(void) 
	{
	return(SDS_CmdLineSize(SDS_TEXTPAGE));
	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_textscr(void) 
	{
	return(SDS_CmdLineSize(SDS_TEXTSCR));
	}

// *****************************************
// This is an SDS External API
//
// TODOTHREAD -- probably also safe on UI thread
//
int 
sdsengine_grclear(void) 
	{
	ASSERT( OnEngineThread() );

	ExecuteUIAction(ICAD_WNDACTION_UDSTATBAR);

	CIcadView* pView = SDS_CURVIEW;
	if(pView == NULL) 
		return RTERROR;

	pView->CursorOn(false);
	RECT rect;
	if(!pView->IsPrinting()) 
	{
		GetClientRect(pView->m_hWnd,&rect);
		pView->GetFrameBufferDrawDevice()->fillRect(rect, 256);
		SDS_BitBlt2Scr(0);
	}

	return(RTNORM);
	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_grtext(int nWhere, const char *szTextMsg, int swHighlight) 
	{
	return(SDS_PrintOnStatus(nWhere,(char *)szTextMsg,swHighlight));
	}




// **************************************************************************************

char *SDS_MenuCmdStr;

// *****************************************
// This is an SDS External API
//
int 
sdsengine_menucmd(const char *szPartToDisplay) 
	{

	if(szPartToDisplay==NULL) 
		{
		return(RTERROR);
		}

	if(toupper(szPartToDisplay[0])=='M'/*DNT*/) 
		{
		sds_retstr("(princ)"/*DNT*/);
		return(RTNORM);
		}

	// TODOTHREAD -- get rid of global

	SDS_MenuCmdStr=(char *)szPartToDisplay;
	ExecuteUIAction( ICAD_WNDACTION_MENUCMD );

    return(RTNORM);
	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_redraw(const sds_name nmEntity, int nHowToDraw) 
{
	ASSERT(OnEngineThread());
	
	if(nmEntity == NULL)
	{
		return(SDS_RedrawDrawing(SDS_CURDWG, NULL, NULL, 1));
	}
	else
	{
		if(!db_is_entitytype(((db_handitem*)nmEntity[0])->ret_type()))
		{
			return(RTNORM);
		}
		return(SDS_RedrawEntity(nmEntity, nHowToDraw, NULL, NULL, 1));
	}
}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_grdraw(const sds_point ptFrom, const sds_point ptTo, int nColor, int swHighlight) 
{
	ASSERT(OnEngineThread());

	return(SDS_DrawLine(ptFrom, ptTo, nColor, swHighlight, 0, SDS_CURDWG, 1));
}

// *****************************************
// This is an SDS External API
//
int	
sdsengine_grfill(const sds_point *pptPoints, int nNumPoints, int nColor, int swHighlight) 
{	
	ASSERT(OnEngineThread());

	return(SDS_DrawFill(pptPoints, nNumPoints, nColor, swHighlight, 0, SDS_CURDWG, 1));
}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_grarc(const sds_point ptCenter, double dRadius, double dStartAngle, double dEndAngle, int nColor, int swHighlight) 
{
	ASSERT(OnEngineThread());

	struct resbuf rb;
	SDS_getvar(NULL, DB_QVIEWSIZE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	SDS_DrawArc(nColor, swHighlight, 0, rb.resval.rreal, ptCenter[0], ptCenter[1], dRadius, dStartAngle, dEndAngle, ptCenter[2]);

	SDS_BitBlt2Scr(1);
	return(RTNORM);
}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_grvecs(const struct sds_resbuf *prbVectList, sds_matrix mxDispTrans) 
{
	ASSERT( OnEngineThread() );
/*
**  Uses sds_grdraw() to emulate the real sds_grvecs().
**
**  Quirks with the real sds_grvects():
**
**      The real sds_grvecs() has a color-highlighting bug: with the
**      default color (no color specified in prbVectList yet) and with
**      negative colors, it uses the most recently accessed color.
**      (Move a LINE of one color and try sds_grvecs(); then move a LINE
**      of another color and try it again.)  (The default is drawn
**      unhighlighted; the negatives are highlighted.)
**
**      The real sds_grvecs() begins using XOR color with 255 --
**      not 256 as the manual says.
**
**  Ours switches to XOR color with 255, too, to follow theirs, but
**  ours tries to handle the default and negative colors properly.
**  Here's a comparison of theirs versus ours for all color ranges:
**
**            Color Range       Theirs          Ours
**          ----------------    -------        -------
**              Default         L   (U)        7   (U)
**           less than -254     L   (H)        XOR (H)
**            -254 thru -1      L   (H)        A   (H)
**             0 thru 254       A   (U)        A   (U)
**          greater than 254    XOR (U)        XOR (U)
**
**        L : Last accessed color (a MOVE will set it)
**        A : Absolute value of the specified color
**      XOR : Exclusive-OR color (for non-destructive dragging)
**        H : Highlighted
**        U : Unhighlighted
**        7 : Color 7 (white)
**
**  There is another significant difference between our sds_grvecs()
**  and the real one.  Ours draws vectors in 3D; the real function
**  always draws in plan view -- something I didn't expect at all since
**  I thought it was a multi-vector version of gis_grdraw().  It would
**  definitely take some extra work to get ours to do that using
**  sds_grdraw() and I'm not sure we'd WANT it to.
*/
#ifdef _SDS_GRVECS_
    int color,hilite,row,col,fi1,fi2;
	int ret=RTNORM;
    sds_point ap[4];
    struct resbuf *tp1;

    color=7; hilite=0;

    for (tp1=(struct resbuf *)prbVectList; tp1!=NULL; tp1=tp1->rbnext) 
		{
        if (tp1->restype==RT3DPOINT || tp1->restype==RTPOINT) 
			{

            if (tp1->rbnext==NULL ||
				(tp1->rbnext->restype!=RT3DPOINT && tp1->rbnext->restype!=RTPOINT)) 
				{ 
				ret=RTERROR; 
				goto out; 
				}

            if (mxDispTrans==NULL) 
				{  /* No xforming */
                ic_ptcpy(ap[2],tp1->resval.rpoint);
                ic_ptcpy(ap[3],tp1->rbnext->resval.rpoint);
				} 
			else 
				{  /* Xform via the matrix */
                ic_ptcpy(ap[0],tp1->resval.rpoint);
                ic_ptcpy(ap[1],tp1->rbnext->resval.rpoint);
                for (fi1=0,fi2=2; fi1<2; fi1++,fi2++) 
					{  /* Each pt */
                    ap[fi2][0]=ap[fi2][1]=ap[fi2][2]=0.0;
                    for (row=0; row<3; row++) 
						{
                        for (col=0; col<3; col++)
							{
                            ap[fi2][row]+=mxDispTrans[row][col]*ap[fi1][col];
							}
                        ap[fi2][row]+=mxDispTrans[row][3];
	                    }
		            }
			    }

            SDS_DrawLine(ap[2],ap[3],color,hilite,0,SDS_CURDWG,0);

            tp1=tp1->rbnext;  /* Step to 2nd point and continue */

			} 
		else if (tp1->restype==RTSHORT) 
			{
            color=tp1->resval.rint;
            /* Now set color and hilite for sds_grdraw(): */
            if (color<-254) 
				{  
                color=-1; 
				hilite=1;
				} 
			else if (color<0) 
				{  /* -254 through -1 */
                color=-color; 
				hilite=1;
				} 
			else if (color<255) 
				{  /* 0 through 254 (see quirks) */
                hilite=0;
				} 
			else 
				{  /* greater than 254 */
                color=-1; 
				hilite=0;
	            }
			} 
		else 
			{ 
			ret=RTERROR; 
			goto out; 
			}
    
		} // end of for()

	out: ;

	SDS_BitBlt2Scr(1);
    return(ret);
#else
	CIcadEntityRenderer renderer(SDS_CURDOC);
	return renderer.drawVecs(prbVectList, mxDispTrans);
#endif
}

// ************************************************
// ************************************************
// ************************************************

// *****************************
// Display Object functions
//
// sds_dobjll was identical to gr_displayobject, but
// they have diverged.  Before these just used to make
// type casts to internal functions.  Now they must 
// make "copies" of the internal structures.
//
// *****************************************
// This is an SDS External API
//
void sdsengine_freedispobjs(struct sds_dobjll* pDispObjs) 
{
    struct sds_dobjll* doll, *tp;
    doll = pDispObjs;
    while(doll != NULL) 
	{
        if(doll->chain != NULL) 
		{
			delete doll->chain;
			doll->chain = NULL;
		}
        tp = doll->next; 
		delete doll; 
		doll = tp;
	}
}

// *************
// HELPER FUNCTION
//
// Make copy of internal format list
//
static struct sds_dobjll* copyDispObjs(gr_displayobject* gr_obj)
{
	/*DG - 16.4.2003*/// I made it non-recursive.

	sds_dobjll	*pRetval = NULL,
				*pDO = NULL;
	int			iSize;

	for( ; gr_obj; gr_obj = gr_obj->next)
	{
		// DP: skip special/internal display objects
		if(gr_obj->type & DISP_OBJ_SPECIAL)
			continue;

		if(pDO)
		{
			pDO->next = new sds_dobjll ;
			pDO = pDO->next;
		}
		else
			pRetval = pDO = new sds_dobjll;

		ASSERT(pDO);
		if(!pDO)
			break;
		else
		{
			pDO->type = gr_obj->type;
			pDO->color = gr_obj->color;
#ifdef BUILD_WITH_NEW_SDS_DOBJLL
			pDO->lweight = gr_obj->lweight;
			pDO->npts = gr_obj->npts;
#else
			ASSERT(gr_obj->npts <= SHRT_MAX);
			pDO->npts = ( gr_obj->npts > SHRT_MAX ? SHRT_MAX :  gr_obj->npts);
#endif
			pDO->next = NULL;

			iSize = sizeof(sds_real) * pDO->npts * ((pDO->type & DISP_OBJ_PTS_3D) ? 3 : 2);

			pDO->chain = new sds_real [iSize];
			memcpy(pDO->chain, gr_obj->chain, iSize);
		}
	}

	return pRetval;
}

// *****************************************
// This is an SDS External API
//
struct sds_dobjll* sdsengine_dispobjs(const sds_name nmEntity, int nDispMode) 
{
	ASSERT(OnEngineThread());

	if(nmEntity == NULL || nmEntity[0] == 0L || nmEntity[1] == 0L) 
		return NULL;

	struct gr_view* pViewData = SDS_CURGRVW;
	if(pViewData == NULL) 
		return NULL;

	gr_displayobject *beg, *end;
	SDS_getdispobjs((db_drawing*)nmEntity[1],
					(db_handitem**)&nmEntity[0],
					&beg,
					&end,
					NULL,
					(db_drawing*)nmEntity[1],
					pViewData,
					nDispMode,
					NULL,
					true);	/*D.G.*/// 'true' to get mesh surface disp. objs for ACIS entities.
							// It's for renderers here. Maybe sds_dispobjs interface should be changed...

	struct sds_dobjll* pRetval = copyDispObjs(beg);
	if(beg != NULL)
		gr_freedisplayobjectll(beg);

	return pRetval;
}

// ************************************************
// ************************************************
// ************************************************


// *****************************************
// This is an SDS External API
//
int 
sdsengine_tablet (const struct sds_resbuf *prbGetOrSet, struct sds_resbuf **pprbCalibration) 
	{

	// TODO return(CMainFrame.XXXXX); 

	if (!SDS_CMainWindow->m_pTablet)
		//Bugzilla::72001; 18-08-03
		//return RTNORM;
		return RTERROR;

	if (prbGetOrSet->restype!=RTSHORT)
		return RTERROR;

	sds_matrix xform;
	struct resbuf *rb;
	int i;

	if (prbGetOrSet->resval.rint == 1)	//Set the Calibration
		{
		for (rb=prbGetOrSet->rbnext,i=0; rb!=NULL && i<4; rb=rb->rbnext,i++) 
			{
			if (rb->restype!=RT3DPOINT) 
				{ 
				return RTERROR; 
				}
			if (i==3)
				{
				xform[0][i] = rb->resval.rpoint[0];
				xform[1][i] = rb->resval.rpoint[1];
				xform[2][i] = rb->resval.rpoint[2];
				xform[3][i] = 1.0;
				xform[3][0] = xform[3][1] = xform[3][2] = 0.0;
				}
			else
				{
				xform[i][0] = rb->resval.rpoint[0];
				xform[i][1] = rb->resval.rpoint[1];
				xform[i][2] = rb->resval.rpoint[2];
				xform[i][3] = 0.0;
				}
			}

		if (!SDS_CMainWindow->m_pTablet->SetCalibration (xform, XFORM_PROJECTIVE))
			return RTERROR;
		}
	else	//Get the Calibration
		{

		if (!SDS_CMainWindow->m_pTablet->GetCalibration (xform))
			return RTERROR;

		sds_real p1[3], p2[3], p3[3], p4[3];
		for (i=0; i<3; i++)
			{
			p1[i] = xform[0][i];
			p2[i] = xform[1][i];
			p3[i] = xform[2][i];
			p4[i] = xform[i][3];
			}

		if ((rb = sds_buildlist(RT3DPOINT,p1,
								RT3DPOINT,p2,
								RT3DPOINT,p3,
								RT3DPOINT,p4,
								0))==NULL) { return RTERROR; }
		*pprbCalibration=rb;
		}

    return(RTNORM);
	}

// *****************************************
// This is an SDS External API
//
int 
sdsengine_vports(struct sds_resbuf **prbViewSpecs) 
	{

	ASSERT( OnEngineThread() );

   	if(SDS_CURDWG==NULL) 
		{
		return(RTERROR);
		}
	/*
**  Allocates a llist of data about *ACTIVE VPORTs or VIEWPORT entities
**  (depending on TILEMODE).  RTLB/RTLE pairs surround the data
**  for each table entry.  We don't handle VIEWPORT entities yet.
**  For VPORT tables, the current VPORT is supposed to be the first
**  listed.  Since this function simply lists them, it had better
**  be the first in the table.  That's fine for ACAD-saved DWGs that
**  we've just read in.  Someday, when we change the current VPORT,
**  we'll have to make sure the current one stays on top.
**
**  We don't currently assign VPORT ID numbers, so they all show up
**  as 0.
**
**  rbpp must be the ADDRESS of a struct resbuf pointer.
**  If this function sets *rbpp to non-NULL, the caller must free it
**  via sds_relrb() eventually.
**
**  The allocated llist has the following format:
**
**  TILEMODE==1:
**
**      mode==0:
**
**          RTLB
**          RTSHORT VPORT ID number (currently 0)
**          RTPOINT VPORT lower left  coordinates (in 0-1 screen units)
**          RTPOINT VPORT upper right coordinates (in 0-1 screen units)
**          RTLE
**
**      mode==1:
**
**          RTLB
**          VPORT data in tblnext/tblsearch format
**          RTLE
**
**  TILEMODE==0:
**
**      NULL (Not handling VIEWPORT entities yet.)
*/
	char *fcp1;
    int tilemd,rc,cvport,pass,stat;
    struct resbuf rb,*rbll,*last,*trbp1;
    db_handitem *tabhip;
	db_drawing *dp=SDS_CURDWG;

    rc=RTNORM; tilemd=1; rbll=last=NULL;
    rb.restype=RTSHORT; rb.rbnext=NULL;

    if (prbViewSpecs==NULL || dp==NULL || SDS_CURVIEW==NULL) { rc=RTERROR; goto out; }

    SDS_getvar(NULL,DB_QCVPORT,&rb,dp,NULL,NULL);
	cvport=rb.resval.rint;

    if (SDS_getvar(NULL,DB_QTILEMODE,&rb,dp,NULL,NULL)==RTNORM)
        tilemd=(rb.resval.rint!=0);

    if (tilemd) {

        /* Pick out *ACTIVE ones */
		for(pass=0; pass<2; pass++) {
			for (tabhip=dp->tblnext(DB_VPORTTABREC,1); tabhip!=NULL; tabhip=dp->tblnext(DB_VPORTTABREC,0)) {
				tabhip->get_2(&fcp1);
				if (fcp1==NULL || stricmp(fcp1,SDS_STARACTIVE)) continue; 

				// So the active one is always first.
				if(SDS_CURVIEW->m_pVportTabHip==tabhip && pass==1) continue;
				if(SDS_CURVIEW->m_pVportTabHip!=tabhip && pass==0) continue;

				int id;
				sds_point ll,ur;
				tabhip->get_69(&id);
				tabhip->get_10(ll);
				tabhip->get_11(ur);
				if ((trbp1=sds_buildlist(
					   RTLB,
					RTSHORT,id,
					RTPOINT,ll,
					RTPOINT,ur,
					   RTLE,
						  0))==NULL) { rc=RTERROR; goto out; }
				if (rbll==NULL) rbll=trbp1; else last->rbnext=trbp1;
				last=trbp1; while (last->rbnext!=NULL) last=last->rbnext;
			}
		}

    } else {  /* !tilemd */

        /* Pick out non-deleted ones */
		for(pass=0; pass<2; pass++) {
			for (tabhip=dp->tblnext(DB_VXTABREC,1); tabhip!=NULL; tabhip=dp->tblnext(DB_VXTABREC,0)) {

				tabhip=((db_vxtabrec *)tabhip)->ret_vpehip();
				if(tabhip==NULL || tabhip->ret_deleted()) continue;

				// So the active one is always first.
				tabhip->get_68(&stat);
				if(stat==1 && pass==1) continue;
				if(stat!=1 && pass==0) continue;

				int id;
				sds_point cp,ll,ur;
				double wd,ht;
				tabhip->get_69(&id);
				tabhip->get_10(cp);
				tabhip->get_40(&wd);
				tabhip->get_41(&ht);
				ll[0]=cp[0]-(wd/2.0);
				ll[1]=cp[1]-(ht/2.0);
				ll[2]=0.0;
				ur[0]=cp[0]+(wd/2.0);
				ur[1]=cp[1]+(ht/2.0);
				ur[2]=0.0;
				if ((trbp1=sds_buildlist(
					   RTLB,
					RTSHORT,id,
					RTPOINT,ll,
					RTPOINT,ur,
					   RTLE,
						  0))==NULL) { rc=RTERROR; goto out; }
				if (rbll==NULL) rbll=trbp1; else last->rbnext=trbp1;
				last=trbp1; while (last->rbnext!=NULL) last=last->rbnext;
			}
		}

    }

out:
    if (rc!=RTNORM && rbll!=NULL) { sds_relrb(rbll); rbll=NULL; }
    if (prbViewSpecs!=NULL) *prbViewSpecs=rbll;

    return rc;
}

