/* C:\DEV\PRJ\ICAD\DIMBITMAP.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Support functions for linking Ramona to the CAD engine.
 *
 */

#include "Icad.h"
#include "IcadDoc.h"
#include "cmds.h"
#include "IcadView.h"
#include "styletabrec.h"


CBitmap *SDS_DimBitmap(int cx,int cy, int mode)
{
	sds_point pt0, pt1, pt2, pt3, pt4, pt5;
	sds_real  fr1, ViewSize, LinesWidth;
	sds_resbuf *elist = NULL, rb;
	resbuf baselineOffset;
	db_handitem *elp;
	db_setfontsubstfn(NULL);
	db_drawing *savdwg;
	SDS_getvar(NULL, DB_QMEASUREMENT, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	db_drawing *flp=new db_drawing(rb.resval.rint, DB_BYCOL_PSTYLEMODE);
	db_setfontsubstfn(SDS_FontSubsMsg);
	flp->copyheaderfrom(SDS_CURDWG);

	/*D.G.*/// We don't store DIMSTYLE in the header buffer but in the drawing itself.
	SDS_getvar(NULL, DB_QDIMSTYLE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	SDS_setvar(NULL, DB_QDIMSTYLE, &rb, flp, &SDS_CURCFG, &SDS_CURSES, 0);
	if(rb.resval.rstring)
	{
		IC_FREE(rb.resval.rstring);
		rb.resval.rstring = NULL;
	}

	db_styletabrec* pStandardStyleTabRec = (db_styletabrec*)SDS_CURDWG->findtabrec(DB_STYLETAB, "STANDARD"/*DNT*/, 1);
	if(pStandardStyleTabRec)
	{
		char*	pFontName;
		char*	pBigFontName;	// EBATECH(CNBR)
		char	cFontFamily[100];
		long	stylenumber;
		bool	bTTF;
		pStandardStyleTabRec->get_3(&pFontName);
		pStandardStyleTabRec->get_4(&pBigFontName); // EBATECH(CNBR)
		bTTF = pStandardStyleTabRec->hasTrueTypeFont();
		if(bTTF){
			pStandardStyleTabRec->getFontFamily(cFontFamily);
			pStandardStyleTabRec->getFontStyleNumber(&stylenumber);
		}
		pStandardStyleTabRec = (db_styletabrec*)flp->findtabrec(DB_STYLETAB, "STANDARD"/*DNT*/, 1);
		if(pStandardStyleTabRec)
		{
			pStandardStyleTabRec->set_3(pFontName, flp);
			pStandardStyleTabRec->set_4(pBigFontName, flp); // EBATECH(CNBR)
			if(bTTF)
			{
				//EBATECH(CNBR) -[ setFontStyleNumber() requires local name
				//pStandardStyleTabRec->setFontFamily(cFontFamily, flp);
				//pStandardStyleTabRec->setFontStyleNumber(pFontName, StyleNumber, flp);
				pStandardStyleTabRec->setFontFamily(cFontFamily, flp);
				pStandardStyleTabRec->setFontStyleNumber(cFontFamily, stylenumber, flp);
				//EBAETCH(CNBR) ]-
			}
			else
				pStandardStyleTabRec->ensureNoTrueTypeEED(flp);
		}
	}

//	  if((elist = SDS_tblgetter("STYLE"/*DNT*/, "STANDARD"/*DNT*/, 0, flp)) != NULL)
//		SDS_tblmesser(elist, IC_TBLMESSER_DELETE, flp);

	SDS_getvar(NULL, DB_QDIMTXSTY, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	if((elist = SDS_tblgetter("STYLE"/*DNT*/, rb.resval.rstring, 0, flp)) == NULL)
	{
		elist = SDS_tblgetter("STYLE"/*DNT*/, rb.resval.rstring, 0, SDS_CURDWG);
		if(elist != NULL)
		{
			SDS_tblmesser(elist, IC_TBLMESSER_MAKE, flp);
			IC_RELRB(elist);
		}
	}
	IC_RELRB(elist);
	SDS_setvar(NULL, DB_QDIMTXSTY, &rb, flp, &SDS_CURCFG, &SDS_CURSES, 0);
	if(rb.resval.rstring)
	{
		IC_FREE(rb.resval.rstring);
		rb.resval.rstring = NULL;
	}

	CIcadView *pView = SDS_CURVIEW;
	gr_view Tempview, *Saveview;
	if(pView == NULL)
		return(NULL);

	Saveview = pView->m_pCurDwgView;
	pView->m_pCurDwgView = &Tempview;
	savdwg = SDS_CMainWindow->m_pCurDoc->m_dbDrawing;
	SDS_CMainWindow->m_pCurDoc->m_dbDrawing = flp;

	/*D.G.*/// I've made the preview drawing's view settings be dependent from the current drawing's ones
	// (not fixed as it was before!). Also, the entities in the preview are drawn with non-fixed lengths actually
	// and with fixed ones visually. Then a user will see the same sizes both in the preview and in his current drawing.

	SDS_getvar(NULL, DB_QVIEWSIZE, &rb, flp, &SDS_CURCFG, &SDS_CURSES);
	ViewSize = rb.resval.rreal = 0.4 * rb.resval.rreal;		/*D.G.*/// TO DO. I don't know,
															// why preview dimension's sizes look more little.
															// So, decrease preview's viewsize with some
															// empirical coefficient (sorry).
															// Maybe, some Tempview's settings should be changed...
	SDS_setvar(NULL, DB_QVIEWSIZE, &rb, flp, &SDS_CURCFG, &SDS_CURSES, 0);

	rb.restype=RT3DPOINT;
	rb.resval.rpoint[0] = 0.0;
	rb.resval.rpoint[1] = 0.0;
	rb.resval.rpoint[2] = 0.0;
	SDS_setvar(NULL, DB_QEXTMIN, &rb, flp, &SDS_CURCFG, &SDS_CURSES, 0);

	rb.resval.rpoint[0] = ViewSize;
	rb.resval.rpoint[1] = ViewSize;
	rb.resval.rpoint[2] = 0.0;
	SDS_setvar(NULL, DB_QEXTMAX, &rb, flp, &SDS_CURCFG, &SDS_CURSES, 0);

	rb.resval.rpoint[0] = ViewSize / 2.0;
	rb.resval.rpoint[1] = ViewSize / 2.0;
	rb.resval.rpoint[2] = 0.0;
	SDS_setvar(NULL, DB_QVIEWCTR, &rb, flp, &SDS_CURCFG, &SDS_CURSES, 0);


	rb.restype=RTREAL;
	rb.resval.rreal=0.0;
	SDS_setvar(NULL,DB_QVIEWTWIST,&rb,flp,&SDS_CURCFG,&SDS_CURSES,0);

	rb.restype=RTSHORT;
	rb.resval.rint=0;
	SDS_setvar(NULL,DB_QVIEWMODE,&rb,flp,&SDS_CURCFG,&SDS_CURSES,0);

	/*DG - 1.2.2003*/// Set TILEMODE of new drawing to ON (regardless current drw's TILEMODE value). This one can be used in drawing functions, e.g. SDS_DrawBitmap.
	rb.resval.rint = 1;
	SDS_setvar(NULL, DB_QTILEMODE, &rb, flp, &SDS_CURCFG, &SDS_CURSES, 0);

	rb.restype=RT3DPOINT;
	rb.resval.rpoint[0]=0.0;
	rb.resval.rpoint[1]=0.0;
	rb.resval.rpoint[2]=1.0;
	SDS_setvar(NULL,DB_QVIEWDIR,&rb,flp,&SDS_CURCFG,&SDS_CURSES,0);

	rb.resval.rpoint[0]=1.0;
	rb.resval.rpoint[2]=0.0;
	SDS_setvar(NULL,DB_QUCSXDIR,&rb,flp,&SDS_CURCFG,&SDS_CURSES,0);
	rb.resval.rpoint[0]=0.0;
	rb.resval.rpoint[1]=1.0;
	SDS_setvar(NULL,DB_QUCSYDIR,&rb,flp,&SDS_CURCFG,&SDS_CURSES,0);
	rb.resval.rpoint[1]=0.0;
	SDS_setvar(NULL,DB_QUCSORG,&rb,flp,&SDS_CURCFG,&SDS_CURSES,0);

	LinesWidth = ViewSize / 200.0;

	switch(mode)
	{

	case 2:			// Format tab - Rectangle with 2 concentric circles inside.

		// Polyline - Horizontal rectangle
		pt0[0] = 0.0;  pt0[1] = 0.0;  pt0[2] = 0.0;
	   elist = sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/, 62,195, 66,1, 70,1, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
		elp = new db_polyline(flp);
		elp->entmod(elist, flp);
		flp->addhanditem(elp);
		sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 3.3;  pt0[1] = ViewSize / 2.8;	pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 3.3;  pt0[1] = ViewSize / 1.57;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 1.42;  pt0[1] = ViewSize / 1.57;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 1.42;  pt0[1] = ViewSize / 2.8;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Sequence End
		   elist = sds_buildlist(RTDXF0,"SEQEND"/*DNT*/, 0);
			elp = new db_seqend(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);

		// Polyline - Larger circle		(These are polylines so I can use the line width on them.)
		pt0[0] = 0.0;  pt0[1] = 0.0;  pt0[2] = 0.0;	// These stay 0,0,0.
	   elist = sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/, 62,195, 66,1, 70,1, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
		elp = new db_polyline(flp);
		elp->entmod(elist, flp);
		flp->addhanditem(elp);
		sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 1.63;  pt0[1] = ViewSize / 2.02;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 42,-1.0, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 2.5;  pt0[1] = ViewSize / 2.02;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 42,-1.0, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 1.63;  pt0[1] = ViewSize / 2.02;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Sequence End
		   elist = sds_buildlist(RTDXF0,"SEQEND"/*DNT*/, 0);
			elp = new db_seqend(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);

		// Polyline - Smaller circle
		pt0[0] = 0.0;  pt0[1] = 0.0;  pt0[2] = 0.0;	// These stay 0,0,0.
	   elist = sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/, 62,195, 66,1, 70,1, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
		elp = new db_polyline(flp);
		elp->entmod(elist, flp);
		flp->addhanditem(elp);
		sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 1.77;  pt0[1] = ViewSize / 2.02;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 42,-1.0, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 2.24;  pt0[1] = ViewSize / 2.02;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 42,-1.0, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 1.77;  pt0[1] = ViewSize / 2.02;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Sequence End
		   elist = sds_buildlist(RTDXF0,"SEQEND"/*DNT*/, 0);
			elp=new db_seqend(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);

		// Dimension - Horizontal - Diameter of larger circle
		pt0[0] = ViewSize / 1.63;  pt0[1] = ViewSize / 1.15;  pt0[2] = 0.0;		// 10 - Position of dimension line (out away from entity)
		pt1[0] = ViewSize / 1.98;  pt1[1] = ViewSize / 2.5;   pt1[2] = 0.0;		// 11 - Midpoint of dimension text
		pt2[0] = ViewSize / 2.5;   pt2[1] = ViewSize / 2.02;  pt2[2] = 0.0;		// 13 - First point defining dimension (usually on an entity)
		pt3[0] = ViewSize / 1.63;  pt3[1] = ViewSize / 2.02;  pt3[2] = 0.0;		// 14 - Second point defining dimension (other end of entity)

		db_ucshelper(pView->m_pCurDwgView->ucsorg, pView->m_pCurDwgView->ucsaxis, pView->m_pCurDwgView->elevation, pt4, pt5, &fr1);
//		gr_ucshelper(pView->m_pCurDwgView,pt4,pt5,&fr1);  // ?	 Points 4 and 5 come from here, otherwise, I have no idea what it is.

	   elist = sds_buildlist(RTDXF0,"DIMENSION"/*DNT*/, 2,""/*DNT*/, 10,pt0, 11,pt1, 70,0, 1,""/*DNT*/, 13,pt2, 14,pt3, 50,0.0, 12,pt5, 51,fr1, 210,pView->m_pCurDwgView->ucsaxis[2], 0);
		elp = new db_dimension(flp);
		elp->entmod(elist, flp);
		flp->addhanditem(elp);
		sds_relrb(elist);
		cmd_makedimension(elp, flp, 0);

		// Dimension - Horizontal - Diameter of smaller circle
		pt0[0] = ViewSize / 1.77;  pt0[1] = ViewSize / 1.33;  pt0[2] = 0.0;		// 10 - Position of dimension line (out away from entity)
		pt1[0] = ViewSize / 1.55;  pt1[1] = ViewSize / 1.33;  pt1[2] = 0.0;		// 11 - Midpoint of dimension text
		pt2[0] = ViewSize / 2.24;  pt2[1] = ViewSize / 2.02;  pt2[2] = 0.0;		// 13 - First point defining dimension (usually on an entity)
		pt3[0] = ViewSize / 1.77;  pt3[1] = ViewSize / 2.02;  pt3[2] = 0.0;		// 14 - Second point defining dimension (other end of entity)
	   elist = sds_buildlist(RTDXF0,"DIMENSION"/*DNT*/, 2,""/*DNT*/, 10,pt0, 11,pt1, 70,0, 1,""/*DNT*/, 13,pt2, 14,pt3, 50,0.0, 12,pt5, 51,fr1, 210,pView->m_pCurDwgView->ucsaxis[2], 0);
		elp = new db_dimension(flp);
		elp->entmod(elist, flp);
		flp->addhanditem(elp);
		sds_relrb(elist);
		cmd_makedimension(elp, flp, 0);

		// Dimension - Vertical - Left side of rectangle
		pt0[0] = ViewSize / 5.31;  pt0[1] = ViewSize / 1.57;  pt0[2] = 0.0;		// 10 - Position of dimension line (out away from entity)
		pt1[0] = ViewSize / 5.31;  pt1[1] = ViewSize / 2.02;  pt1[2] = 0.0;		// 11 - Midpoint of dimension text
		pt2[0] = ViewSize / 3.3;   pt2[1] = ViewSize / 2.8;   pt2[2] = 0.0;		// 13 - First point defining dimension (usually on an entity)
		pt3[0] = ViewSize / 3.3;   pt3[1] = ViewSize / 1.57;  pt3[2] = 0.0;		// 14 - Second point defining dimension (other end of entity)
	   elist=sds_buildlist(RTDXF0,"DIMENSION"/*DNT*/, 2,""/*DNT*/, 10,pt0, 11,pt1, 70,0, 1,""/*DNT*/, 13,pt2, 14,pt3, 50,(IC_PI/2), 12,pt5, 51,fr1, 210,pView->m_pCurDwgView->ucsaxis[2], 0);
		elp = new db_dimension(flp);
		elp->entmod(elist, flp);
		flp->addhanditem(elp);
		sds_relrb(elist);
		cmd_makedimension(elp, flp, 0);

		// Dimension - Horizontal - Across bottom of rectangle
		pt0[0] = ViewSize / 3.3;   pt0[1] = ViewSize / 4.72;  pt0[2] = 0.0;		// 10 - Position of dimension line (out away from entity)
		pt1[0] = ViewSize / 1.98;  pt1[1] = ViewSize / 4.72;  pt1[2] = 0.0;		// 11 - Midpoint of dimension text
		pt2[0] = ViewSize / 1.42;  pt2[1] = ViewSize / 2.8;   pt2[2] = 0.0;		// 13 - First point defining dimension (usually on an entity)
		pt3[0] = ViewSize / 3.3;   pt3[1] = ViewSize / 2.8;   pt3[2] = 0.0;		// 14 - Second point defining dimension (other end of entity)
	   elist = sds_buildlist(RTDXF0,"DIMENSION"/*DNT*/, 2,""/*DNT*/, 10,pt0, 11,pt1, 70,0, 1,""/*DNT*/, 13,pt2, 14,pt3, 50,0.0, 12,pt5, 51,fr1, 210,pView->m_pCurDwgView->ucsaxis[2], 0);
		elp = new db_dimension(flp);
		elp->entmod(elist, flp);
		flp->addhanditem(elp);
		sds_relrb(elist);
		cmd_makedimension(elp, flp, 0);

		// Dimension - Vertical - Bottom of rectangle to bottom of circle
		pt0[0] = ViewSize / 1.15;  pt0[1] = ViewSize / 2.8;   pt0[2] = 0.0;		// 10 - Position of dimension line (out away from entity)
		pt1[0] = ViewSize / 1.27;  pt1[1] = ViewSize / 3.22;  pt1[2] = 0.0;		// 11 - Midpoint of dimension text
		pt2[0] = ViewSize / 1.98;  pt2[1] = ViewSize / 2.58;  pt2[2] = 0.0;		// 13 - First point defining dimension (usually on an entity)
		pt3[0] = ViewSize / 1.42;  pt3[1] = ViewSize / 2.8;   pt3[2] = 0.0;		// 14 - Second point defining dimension (other end of entity)
	   elist = sds_buildlist(RTDXF0,"DIMENSION"/*DNT*/, 2,""/*DNT*/, 10,pt0, 11,pt1, 70,0, 1,""/*DNT*/, 13,pt2, 14,pt3, 50,(IC_PI/2), 12,pt5, 51,fr1, 210,pView->m_pCurDwgView->ucsaxis[2], 0);
		elp = new db_dimension(flp);
		elp->entmod(elist, flp);
		flp->addhanditem(elp);
		sds_relrb(elist);
		cmd_makedimension(elp, flp, 0);

		break;

	case 3:			// Lines tab

		// Polyline - A rectangle with a big notch out of the upper-left side
		pt0[0] = 0.0;  pt0[1] = 0.0;  pt0[2] = 0.0;	// These stay 0,0,0.
	   elist = sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/, 62,195, 66,1, 70,1, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
		elp = new db_polyline(flp);
		elp->entmod(elist, flp);
		flp->addhanditem(elp);
		sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 2.13;  pt0[1] = ViewSize / 6.07;  pt0[2] = 0.0;	// Lower left corner
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 2.13;  pt0[1] = ViewSize / 2.66;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 1.47;  pt0[1] = ViewSize / 2.66;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 1.47;  pt0[1] = ViewSize / 2.24;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 1.18;  pt0[1] = ViewSize / 2.24;  pt0[2] = 0.0;	// Upper right corner
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 1.18;  pt0[1] = ViewSize / 3.54;  pt0[2] = 0.0;	// Start of arc
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 42,-tan((IC_PI/2)/4), 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 1.37;  pt0[1] = ViewSize / 6.07;  pt0[2] = 0.0;	// End of arc
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 2.13;  pt0[1] = ViewSize / 6.07;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Sequence End
		   elist = sds_buildlist(RTDXF0,"SEQEND"/*DNT*/, 0);
			elp = new db_seqend(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);

		// Dimension - Horizontal dimension of notch on upper left
		pt0[0] = ViewSize / 1.47;  pt0[1] = ViewSize / 1.89;  pt0[2] = 0.0;		// 10 - Position of dimension line (out away from entity)
		pt1[0] = ViewSize / 1.73;  pt1[1] = ViewSize / 1.89;  pt1[2] = 0.0;		// 11 - Midpoint of dimension text
		pt2[0] = ViewSize / 1.18;  pt2[1] = ViewSize / 2.66;  pt2[2] = 0.0;		// 13 - First point defining dimension (usually on an entity)
		pt3[0] = ViewSize / 1.47;  pt3[1] = ViewSize / 2.24;  pt3[2] = 0.0;		// 14 - Second point defining dimension (other end of entity)

		db_ucshelper(pView->m_pCurDwgView->ucsorg, pView->m_pCurDwgView->ucsaxis, pView->m_pCurDwgView->elevation, pt4, pt5, &fr1);
//		gr_ucshelper(pView->m_pCurDwgView,pt4,pt5,&fr1);  // ?	 Points 4 and 5 come from here, otherwise, I have no idea what it is.

	   elist = sds_buildlist(RTDXF0,"DIMENSION"/*DNT*/, 2,""/*DNT*/, 10,pt0, 11,pt1, 70,0, 1,""/*DNT*/, 13,pt2, 14,pt3, 50,0.0, 12,pt5, 51,fr1, 210,pView->m_pCurDwgView->ucsaxis[2], 0);
		elp = new db_dimension(flp);
		elp->entmod(elist, flp);
		flp->addhanditem(elp);
		sds_relrb(elist);
		cmd_makedimension(elp, flp, 0);

		// Dimension - Horizontal baseline dimension - Space from above determined by baseline spacing variable
		SDS_getvar(NULL,DB_QDIMDLI, &baselineOffset, flp, &SDS_CURCFG, &SDS_CURSES);
		pt0[0] = ViewSize / 1.18;  pt0[1] = (pt0[1] + baselineOffset.resval.rreal);  pt0[2] = 0.0;		// 10 - Y determined by Y of previous dimension + baseline spacing
		pt1[0] = ViewSize / 1.52;  pt1[1] = (pt1[1] + baselineOffset.resval.rreal);  pt1[2] = 0.0;		// 11 - Y determined by Y of previous dimension + baseline spacing
		pt2[0] = ViewSize / 2.13;  pt2[1] = ViewSize / 2.24;						 pt2[2] = 0.0;		// 13 - First point defining dimension (usually on an entity)
		pt3[0] = ViewSize / 1.18;  pt3[1] = ViewSize / 2.24;						 pt3[2] = 0.0;		// 14 - Second point defining dimension (other end of entity)
	   elist = sds_buildlist(RTDXF0,"DIMENSION"/*DNT*/, 2,""/*DNT*/, 10,pt0, 11,pt1, 70,0, 1,""/*DNT*/, 13,pt2, 14,pt3, 50,0.0, 12,pt5, 51,fr1, 210,pView->m_pCurDwgView->ucsaxis[2], 0);
		elp = new db_dimension(flp);
		elp->entmod(elist, flp);
		flp->addhanditem(elp);
		sds_relrb(elist);
		cmd_makedimension(elp, flp, 0);

		// Dimension - Vertical dimension - bottom of rectangle to bottom of notch
		pt0[0] = ViewSize / 2.83;  pt0[1] = ViewSize / 2.66;  pt0[2] = 0.0;		// 10 - Position of dimension line (out away from entity)
		pt1[0] = ViewSize / 2.83;  pt1[1] = ViewSize / 3.7;   pt1[2] = 0.0;		// 11 - Midpoint of dimension text
		pt2[0] = ViewSize / 2.13;  pt2[1] = ViewSize / 6.07;  pt2[2] = 0.0;		// 13 - First point defining dimension (usually on an entity)
		pt3[0] = ViewSize / 2.13;  pt3[1] = ViewSize / 2.66;  pt3[2] = 0.0;		// 14 - Second point defining dimension (other end of entity)
	   elist = sds_buildlist(RTDXF0,"DIMENSION"/*DNT*/, 2,""/*DNT*/, 10,pt0, 11,pt1, 70,0, 1,""/*DNT*/, 13,pt2, 14,pt3, 50,(IC_PI/2), 12,pt5, 51,fr1, 210,pView->m_pCurDwgView->ucsaxis[2], 0);
		elp = new db_dimension(flp);
		elp->entmod(elist, flp);
		flp->addhanditem(elp);
		sds_relrb(elist);
		cmd_makedimension(elp, flp, 0);

		// Dimension - Vertical baseline dimension - Space from above determined by baseline spacing variable
		pt0[0] = (pt0[0] - baselineOffset.resval.rreal);  pt0[1] = ViewSize / 2.24;  pt0[2] = 0.0;		// 10 - Position of dimension line (out away from entity)
		pt1[0] = (pt1[0] - baselineOffset.resval.rreal);  pt1[1] = ViewSize / 3.3;	 pt1[2] = 0.0;		// 11 - Midpoint of dimension text
		pt2[0] = ViewSize / 2.13;						  pt2[1] = ViewSize / 6.07;  pt2[2] = 0.0;		// 13 - First point defining dimension (usually on an entity)
		pt3[0] = ViewSize / 1.47;						  pt3[1] = ViewSize / 2.24;  pt3[2] = 0.0;		// 14 - Second point defining dimension (other end of entity)
	   elist = sds_buildlist(RTDXF0,"DIMENSION"/*DNT*/, 2,""/*DNT*/, 10,pt0, 11,pt1, 70,0, 1,""/*DNT*/, 13,pt2, 14,pt3, 50,(IC_PI/2), 12,pt5, 51,fr1, 210,pView->m_pCurDwgView->ucsaxis[2], 0);
		elp = new db_dimension(flp);
		elp->entmod(elist, flp);
		flp->addhanditem(elp);
		sds_relrb(elist);
		cmd_makedimension(elp, flp, 0);

		// Lines - These are as drawn by the center line dimension command - length determined by center line size variable
		struct resbuf dimcen;
		SDS_getvar(NULL, DB_QDIMCEN, &dimcen, flp, &SDS_CURCFG, &SDS_CURSES);
		pt0[0] = ViewSize / 1.37;  pt0[1] = ViewSize / 3.54;  pt0[2] = 0.0;		// Center of arc is at ViewSize / 1.37, ViewSize / 3.54, 0.0
		if(dimcen.resval.rreal > 0)
		{								// Draws center marks only. Marks extend from center a dist of DIMCEN.
			struct resbuf color;		// Color of center marks/lines is same as dimension lines.
			SDS_getvar(NULL, DB_QDIMCLRD, &color, flp, &SDS_CURCFG, &SDS_CURSES);
			sds_real angle;
			for(angle = 0L; angle < (2*IC_PI); angle = (angle + (IC_PI/2)))
			{
				// Draw the center mark as lines in 4 directions:
				sds_polar(pt0, angle, dimcen.resval.rreal, pt1);
			   elist = sds_buildlist(RTDXF0,"LINE"/*DNT*/, 10,pt0, 11,pt1, 62,color.resval.rint, 0);
				elp = new db_line(flp);
				elp->entmod(elist, flp);
				flp->addhanditem(elp);
				sds_relrb(elist);
			}
		}
		else
			if(dimcen.resval.rreal < 0)
			{								// Draws center lines.	Gaps are also DIMCEN wide, and
				struct resbuf color;		// Color of center marks/lines is same as dimension lines.
				SDS_getvar(NULL, DB_QDIMCLRD, &color, flp, &SDS_CURCFG, &SDS_CURSES);
				dimcen.resval.rreal =- (dimcen.resval.rreal);	// Use as pos value from here (not saved back to setvar, so OK).
				sds_real angle;
				sds_point pt2;
				for(angle = 0L; angle < (2*IC_PI); angle = (angle + (IC_PI/2)))
				{	// lines extend past circle by a dist of DIMCEN.
					// Draw the center mark as lines in 4 directions:
					sds_polar(pt0, angle, dimcen.resval.rreal, pt1);
				   elist = sds_buildlist(RTDXF0,"LINE"/*DNT*/, 10,pt0, 11,pt1, 62,color.resval.rint, 0);
					elp = new db_line(flp);
					elp->entmod(elist, flp);
					flp->addhanditem(elp);
					sds_relrb(elist);
					// Leave a gap as wide as DIMCEN
					sds_polar(pt1, angle, dimcen.resval.rreal, pt2);	// pt2 is end of gap
					// Draw another line from the gap to DIMCEN past the edge of the circle
					sds_polar(pt0, angle, (ViewSize / 8.51 + dimcen.resval.rreal), pt1); // ViewSize / 8.51 = radius of arc, pt1 is now end of line
				   elist = sds_buildlist(RTDXF0,"LINE"/*DNT*/, 10,pt2, 11,pt1, 62,color.resval.rint, 0);
					elp = new db_line(flp);
					elp->entmod(elist, flp);
					flp->addhanditem(elp);
					sds_relrb(elist);
				}
			}	// If zero, doesn't draw center lines or center marks at all.

		break;

	case 4:			// Text tab

		// Polyline - rectangle w/ notches on left side and lower right corner
		pt0[0] = 0.0;  pt0[1] = 0.0;  pt0[2] = 0.0;	// These stay 0,0,0.
	   elist = sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/, 62,195, 66,1, 70,1, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
		elp = new db_polyline(flp);
		elp->entmod(elist, flp);
		flp->addhanditem(elp);
		sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 2.13;  pt0[1] = ViewSize / 2.8;  pt0[2] = 0.0;	// Lower left corner
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 2.13;  pt0[1] = ViewSize / 1.44;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 1.93;  pt0[1] = ViewSize / 1.44;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 1.93;  pt0[1] = ViewSize / 1.29;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 2.13;  pt0[1] = ViewSize / 1.29;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 2.13;  pt0[1] = ViewSize / 1.21;  pt0[2] = 0.0;	// Upper left corner
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 1.21;  pt0[1] = ViewSize / 1.21;  pt0[2] = 0.0;	// Upper right corner
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 1.21;  pt0[1] = ViewSize / 2.5;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 1.37;  pt0[1] = ViewSize / 2.5;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 1.37;  pt0[1] = ViewSize / 2.8;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 2.13;  pt0[1] = ViewSize / 2.8;  pt0[2] = 0.0;	// Lower left corner again
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Sequence End
		   elist = sds_buildlist(RTDXF0,"SEQEND"/*DNT*/, 0);
			elp = new db_seqend(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);

		// Dimension - Horizontal - notch on lower right side
		pt0[0] = ViewSize / 1.37;  pt0[1] = ViewSize / 4.47;  pt0[2] = 0.0;		// 10 - Position of dimension line (out away from entity)
		pt1[0] = ViewSize / 1.55;  pt1[1] = ViewSize / 4.47;  pt1[2] = 0.0;		// 11 - Midpoint of dimension text
		pt2[0] = ViewSize / 1.21;  pt2[1] = ViewSize / 2.24;  pt2[2] = 0.0;		// 13 - First point defining dimension (usually on an entity)
		pt3[0] = ViewSize / 1.37;  pt3[1] = ViewSize / 2.8;   pt3[2] = 0.0;		// 14 - Second point defining dimension (other end of entity)

		db_ucshelper(pView->m_pCurDwgView->ucsorg, pView->m_pCurDwgView->ucsaxis, pView->m_pCurDwgView->elevation, pt4, pt5, &fr1);
//		gr_ucshelper(pView->m_pCurDwgView,pt4,pt5,&fr1);  // ?	 Points 4 and 5 come from here, otherwise, I have no idea what it is.

	   elist = sds_buildlist(RTDXF0,"DIMENSION"/*DNT*/, 2,""/*DNT*/, 10,pt0, 11,pt1, 70,0, 1,""/*DNT*/, 13,pt2, 14,pt3, 50,0.0, 12,pt5, 51,fr1, 210,pView->m_pCurDwgView->ucsaxis[2], 0);
		elp = new db_dimension(flp);
		elp->entmod(elist, flp);
		flp->addhanditem(elp);
		sds_relrb(elist);
		cmd_makedimension(elp, flp, 0);

		// Dimension - Horizontal baseline dimension - Space from above determined by baseline spacing variable
		SDS_getvar(NULL,DB_QDIMDLI, &baselineOffset, flp, &SDS_CURCFG, &SDS_CURSES);
		pt0[0] = ViewSize / 2.13;  pt0[1] = (pt0[1] - baselineOffset.resval.rreal);  pt0[2] = 0.0;		// 10 - Y determined by Y of previous dimension - baseline spacing
		pt1[0] = ViewSize / 1.55;  pt1[1] = (pt1[1] - baselineOffset.resval.rreal);  pt1[2] = 0.0;		// 11 - Y determined by Y of previous dimension - baseline spacing
		pt2[0] = ViewSize / 1.21;  pt2[1] = ViewSize / 2.8; 						 pt2[2] = 0.0;		// 13 - First point defining dimension (usually on an entity)
		pt3[0] = ViewSize / 2.13;  pt3[1] = ViewSize / 2.8; 						 pt3[2] = 0.0;		// 14 - Second point defining dimension (other end of entity)
	   elist = sds_buildlist(RTDXF0,"DIMENSION"/*DNT*/, 2,""/*DNT*/, 10,pt0, 11,pt1, 70,0, 1,""/*DNT*/, 13,pt2, 14,pt3, 50,0.0, 12,pt5, 51,fr1, 210,pView->m_pCurDwgView->ucsaxis[2], 0);
		elp = new db_dimension(flp);
		elp->entmod(elist, flp);
		flp->addhanditem(elp);
		sds_relrb(elist);
		cmd_makedimension(elp, flp, 0);

		// Dimension - Vertical - for notch on left side of rectangle
		pt0[0] = ViewSize / 2.43;  pt0[1] = ViewSize / 1.44;  pt0[2] = 0.0;		// 10 - Position of dimension line (out away from entity)
		pt1[0] = ViewSize / 3.05;  pt1[1] = ViewSize / 1.49;  pt1[2] = 0.0;		// 11 - Midpoint of dimension text
		pt2[0] = ViewSize / 2.13;  pt2[1] = ViewSize / 1.29;  pt2[2] = 0.0;		// 13 - First point defining dimension (usually on an entity)
		pt3[0] = ViewSize / 2.13;  pt3[1] = ViewSize / 1.44;  pt3[2] = 0.0;		// 14 - Second point defining dimension (other end of entity)
	   elist = sds_buildlist(RTDXF0,"DIMENSION"/*DNT*/, 2,""/*DNT*/, 10,pt0, 11,pt1, 70,0, 1,""/*DNT*/, 13,pt2, 14,pt3, 50,(IC_PI/2), 12,pt5, 51,fr1, 210,pView->m_pCurDwgView->ucsaxis[2], 0);
		elp = new db_dimension(flp);
		elp->entmod(elist, flp);
		flp->addhanditem(elp);
		sds_relrb(elist);
		cmd_makedimension(elp, flp, 0);

		// Dimension - Vertical - entire height of rectangle
		pt0[0] = ViewSize / 4.05;  pt0[1] = ViewSize / 2.8;   pt0[2] = 0.0;		// 10 - Position of dimension line (out away from entity)
		pt1[0] = ViewSize / 4.05;  pt1[1] = ViewSize / 1.7;   pt1[2] = 0.0;		// 11 - Midpoint of dimension text
		pt2[0] = ViewSize / 2.13;  pt2[1] = ViewSize / 1.21;  pt2[2] = 0.0;		// 13 - First point defining dimension (usually on an entity)
		pt3[0] = ViewSize / 2.13;  pt3[1] = ViewSize / 2.5;   pt3[2] = 0.0;		// 14 - Second point defining dimension (other end of entity)
	   elist = sds_buildlist(RTDXF0,"DIMENSION"/*DNT*/, 2,""/*DNT*/, 10,pt0, 11,pt1, 70,0, 1,""/*DNT*/, 13,pt2, 14,pt3, 50,(IC_PI/2), 12,pt5, 51,fr1, 210,pView->m_pCurDwgView->ucsaxis[2], 0);
		elp = new db_dimension(flp);
		elp->entmod(elist, flp);
		flp->addhanditem(elp);
		sds_relrb(elist);
		cmd_makedimension(elp, flp, 0);

		break;

	case 6:			// Units tab

		// Polyline - one slanted side and a notch out of the lower right corner
		pt0[0] = 0.0;  pt0[1] = 0.0;  pt0[2] = 0.0;	// These stay 0,0,0.
	   elist = sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/, 62,195, 66,1, 70,1, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
		elp = new db_polyline(flp);
		elp->entmod(elist, flp);
		flp->addhanditem(elp);
		sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 5.0;  pt0[1] = ViewSize / 2.36;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 5.0;  pt0[1] = ViewSize / 1.29;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 2.36;  pt0[1] = ViewSize / 1.29;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 1.98;  pt0[1] = ViewSize / 1.73;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 1.98;  pt0[1] = ViewSize / 2.13;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 2.3;  pt0[1] = ViewSize / 2.13;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 2.3;  pt0[1] = ViewSize / 2.36;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 5.0;  pt0[1] = ViewSize / 2.36;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Sequence End
		   elist = sds_buildlist(RTDXF0,"SEQEND"/*DNT*/, 0);
			elp = new db_seqend(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);

		// Horizontal dimension across bottom - illustrates trailing zeros
		pt0[0] = ViewSize / 5.0;   pt0[1] = ViewSize / 3.3;   pt0[2] = 0.0;		// 10 - Position of dimension line (out away from entity)
		pt1[0] = ViewSize / 3.15;  pt1[1] = ViewSize / 3.3;   pt1[2] = 0.0;		// 11 - Midpoint of dimension text
		pt2[0] = ViewSize / 2.3;   pt2[1] = ViewSize / 2.36;  pt2[2] = 0.0;		// 13 - First point defining dimension (usually on an entity)
		pt3[0] = ViewSize / 5.0;   pt3[1] = ViewSize / 2.36;  pt3[2] = 0.0;		// 14 - Second point defining dimension (other end of entity)

		db_ucshelper(pView->m_pCurDwgView->ucsorg, pView->m_pCurDwgView->ucsaxis, pView->m_pCurDwgView->elevation, pt4, pt5, &fr1);
//		gr_ucshelper(pView->m_pCurDwgView,pt4,pt5,&fr1);  // ?	 Points 4 and 5 come from here, otherwise, I have no idea what it is.

	   elist = sds_buildlist(RTDXF0,"DIMENSION"/*DNT*/, 2,""/*DNT*/, 10,pt0, 11,pt1, 70,0, 1,""/*DNT*/, 13,pt2, 14,pt3, 50,0.0, 12,pt5, 51,fr1, 210,pView->m_pCurDwgView->ucsaxis[2], 0);
		elp = new db_dimension(flp);
		elp->entmod(elist, flp);
		flp->addhanditem(elp);
		sds_relrb(elist);
		cmd_makedimension(elp, flp, 0);

		// Vertical dimension on right side - illustrates leading zeros
		pt0[0] = ViewSize / 1.63;  pt0[1] = ViewSize / 2.13;  pt0[2] = 0.0;		// 10 - Position of dimension line (out away from entity)
		pt1[0] = ViewSize / 1.44;  pt1[1] = ViewSize / 1.95;  pt1[2] = 0.0;		// 11 - Midpoint of dimension text
		pt2[0] = ViewSize / 2.3;   pt2[1] = ViewSize / 2.36;  pt2[2] = 0.0;		// 13 - First point defining dimension (usually on an entity)
		pt3[0] = ViewSize / 1.98;  pt3[1] = ViewSize / 2.13;  pt3[2] = 0.0;		// 14 - Second point defining dimension (other end of entity)
	   elist = sds_buildlist(RTDXF0,"DIMENSION"/*DNT*/, 2,""/*DNT*/, 10,pt0, 11,pt1, 70,0, 1,""/*DNT*/, 13,pt2, 14,pt3, 50,(IC_PI/2), 12,pt5, 51,fr1, 210,pView->m_pCurDwgView->ucsaxis[2], 0);
		elp = new db_dimension(flp);
		elp->entmod(elist, flp);
		flp->addhanditem(elp);
		sds_relrb(elist);
		cmd_makedimension(elp, flp, 0);

		// Angular dimension - for showing angular formatting
		pt0[0] = ViewSize / 1.98;  pt0[1] = ViewSize / 1.73;  pt0[2] = 0.0;		// 10 - Definition point
		pt1[0] = ViewSize / 1.56;  pt1[1] = ViewSize / 1.58;  pt1[2] = 0.0;	// 11 - Midpoint of text
		pt2[0] = ViewSize / 5.0;   pt2[1] = ViewSize / 1.29;  pt2[2] = 0.0;		// 13 - Definition point
		pt3[0] = ViewSize / 2.36;  pt3[1] = ViewSize / 1.29;  pt3[2] = 0.0;		// 14 - Definition point
		pt4[0] = ViewSize / 2.36;  pt4[1] = ViewSize / 1.29;  pt4[2] = 0.0;		// 15 - Definition point
		pt5[0] = ViewSize / 1.49;  pt5[1] = ViewSize / 1.44;  pt5[2] = 0.0;		// 16 - Point defining arc'd dimension line
	   elist = sds_buildlist(RTDXF0,"DIMENSION"/*DNT*/, 2,""/*DNT*/, 10,pt0, 11,pt1, 70,34, 1,""/*DNT*/, 13,pt2, 14,pt3, 15,pt4, 16,pt5, 50,0.0, 0);
		elp = new db_dimension(flp);
		elp->entmod(elist, flp);
		flp->addhanditem(elp);
		sds_relrb(elist);
		cmd_makedimension(elp, flp, 0);

		break;

	case 5:			// Tolerance tab - Disabled for now, but in case it *thinks* it got called, it
//		break;		// will go on to display the drawing for the Arrows tab.


	case 7:			// Alternate Units tab - Also disabled, uses Arrows drawing if accidentally called.
//		break;


	case 1:			// Arrows tab

		// Polyline
		pt0[0] = 0.0;  pt0[1] = 0.0;  pt0[2] = 0.0;
	   elist = sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/, 62,195, 66,1, 70,1, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
		elp = new db_polyline(flp);
		elp->entmod(elist, flp);
		flp->addhanditem(elp);
		sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 3.04;  pt0[1] = ViewSize / 2.66;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 1.42;  pt0[1] = ViewSize / 2.66;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 2.36;  pt0[1] = ViewSize / 1.52;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 3.04;  pt0[1] = ViewSize / 1.52;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Vertex
			pt0[0] = ViewSize / 3.04;  pt0[1] = ViewSize / 2.66;  pt0[2] = 0.0;
		   elist = sds_buildlist(RTDXF0,"VERTEX"/*DNT*/, 10,pt0, 40,LinesWidth, 41,LinesWidth, 0);
			elp = new db_vertex(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			// Sequence End
		   elist = sds_buildlist(RTDXF0,"SEQEND"/*DNT*/, 0);
			elp = new db_seqend(flp);
			elp->entmod(elist, flp);
			flp->addhanditem(elp);
			sds_relrb(elist);

		// Dimension - Horizontal, across small top edge of thing
		pt0[0] = ViewSize / 2.36;  pt0[1] = ViewSize / 1.18;  pt0[2] = 0.0;		// 10 - Position of dimension line (out away from entity)
		pt1[0] = ViewSize / 1.97;  pt1[1] = ViewSize / 1.18;  pt1[2] = 0.0;		// 11 - Midpoint of dimension text
		pt2[0] = ViewSize / 3.04;  pt2[1] = ViewSize / 1.52;  pt2[2] = 0.0;		// 13 - First point defining dimension (usually on an entity)
		pt3[0] = ViewSize / 2.36;  pt3[1] = ViewSize / 1.52;  pt3[2] = 0.0;		// 14 - Second point defining dimension (other end of entity)

		db_ucshelper(pView->m_pCurDwgView->ucsorg, pView->m_pCurDwgView->ucsaxis, pView->m_pCurDwgView->elevation, pt4, pt5, &fr1);
//		gr_ucshelper(pView->m_pCurDwgView,pt4,pt5,&fr1);

	   elist = sds_buildlist(RTDXF0,"DIMENSION"/*DNT*/, 2,""/*DNT*/, 10,pt0, 11,pt1, 70,0, 1,""/*DNT*/, 13,pt2, 14,pt3, 50,0.0, 12,pt5, 51,fr1, 210,pView->m_pCurDwgView->ucsaxis[2], 0);
		elp = new db_dimension(flp);
		elp->entmod(elist, flp);
		flp->addhanditem(elp);
		sds_relrb(elist);
		cmd_makedimension(elp, flp, 0);

		// Dimension - Vertical, along left side of thing
		pt0[0] = ViewSize / 6.07;  pt0[1] = ViewSize / 2.13;  pt0[2] = 0.0;		// 10 - Position of dimension line (out away from entity)
		pt1[0] = ViewSize / 6.07;  pt1[1] = ViewSize / 1.93;  pt1[2] = 0.0;		// 11 - Midpoint of dimension text
		pt2[0] = ViewSize / 3.04;  pt2[1] = ViewSize / 1.52;  pt2[2] = 0.0;		// 13 - First point defining dimension (usually on an entity)
		pt3[0] = ViewSize / 3.04;  pt3[1] = ViewSize / 2.66;  pt3[2] = 0.0;		// 14 - Second point defining dimension (other end of entity)
	   elist = sds_buildlist(RTDXF0,"DIMENSION"/*DNT*/, 2,""/*DNT*/, 10,pt0, 11,pt1, 70,0, 1,""/*DNT*/, 13,pt2, 14,pt3, 50,(IC_PI/2), 0);
		elp = new db_dimension(flp);
		elp->entmod(elist, flp);
		flp->addhanditem(elp);
		sds_relrb(elist);
		cmd_makedimension(elp, flp, 0);

		// Dimension - Aligned, along slanty part on right
		pt0[0] = ViewSize / 1.52;  pt0[1] = ViewSize / 1.52;  pt0[2] = 0.0;		// 10 - Position of dimension line (out away from entity)
		pt1[0] = ViewSize / 1.47;  pt1[1] = ViewSize / 1.57;  pt1[2] = 0.0;		// 11 - Midpoint of dimension text
		pt2[0] = ViewSize / 1.42;  pt2[1] = ViewSize / 2.66;  pt2[2] = 0.0;		// 13 - First point defining dimension (usually on an entity)
		pt3[0] = ViewSize / 2.36;  pt3[1] = ViewSize / 1.52;  pt3[2] = 0.0;		// 14 - Second point defining dimension (other end of entity)
	   elist = sds_buildlist(RTDXF0,"DIMENSION"/*DNT*/, 2,""/*DNT*/, 10,pt0, 11,pt1, 1,""/*DNT*/, 13,pt2, 14,pt3, 70,32, 50,(135.0 * (IC_PI / 180.0)), 0);
		elp = new db_dimension(flp);
		elp->entmod(elist, flp);
		flp->addhanditem(elp);
		sds_relrb(elist);
		cmd_makedimension(elp, flp, 0);

		// Dimension - Horizontal, across whole bottom side
		pt0[0] = ViewSize / 2.13;  pt0[1] = ViewSize / 5.31;  pt0[2] = 0.0;		// 10 - Position of dimension line (out away from entity)
		pt1[0] = ViewSize / 1.93;  pt1[1] = ViewSize / 5.31;  pt1[2] = 0.0;		// 11 - Midpoint of dimension text
		pt2[0] = ViewSize / 3.04;  pt2[1] = ViewSize / 2.66;  pt2[2] = 0.0;		// 13 - First point defining dimension (usually on an entity)
		pt3[0] = ViewSize / 1.42;  pt3[1] = ViewSize / 2.66;  pt3[2] = 0.0;		// 14 - Second point defining dimension (other end of entity)
	   elist = sds_buildlist(RTDXF0,"DIMENSION"/*DNT*/, 2,""/*DNT*/, 10,pt0, 11,pt1, 70,0, 1,""/*DNT*/, 13,pt2, 14,pt3, 50,0.0, 0);
		elp = new db_dimension(flp);
		elp->entmod(elist, flp);
		flp->addhanditem(elp);
		sds_relrb(elist);
		cmd_makedimension(elp, flp, 0);

		break;

	}	// switch(mode)

	CBitmap *bm = SDS_DrawBitmap(flp, NULL, NULL, NULL, cx, cy, TRUE);
	SDS_CMainWindow->m_pCurDoc->m_dbDrawing = savdwg;
	delete flp;
	pView->m_pCurDwgView = Saveview;

	return(bm);
}


/*D.G.*/// If these "examples" aren't needed already then remove it.

/*			// Examples:  *** Please leave these here.	I need to do more work on this eventually, and they are really handy.  ***
			// Polyline
			pt0[0]=0.0; pt0[1]=0.0; pt0[2]=0.0;	// These stay 0,0,0.
			elist=sds_buildlist(RTDXF0,"POLYLINE"DNT, 66,1, 70,1, 10,pt0, 40,0.05, 41,0.05, 0);
			elp=new db_polyline(flp);
			elp->entmod(elist,flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
				// Vertex
				pt0[0]=x.x; pt0[1]=x.x; pt0[2]=0.0;
			   elist=sds_buildlist(RTDXF0,"VERTEX"DNT, 10,pt0, 40,0.05, 41,0.05, 0);
				elp=new db_vertex(flp);
				elp->entmod(elist,flp);
				flp->addhanditem(elp);
				sds_relrb(elist);
				// Vertex
				pt0[0]=x.x; pt0[1]=x.x; pt0[2]=0.0;
			   elist=sds_buildlist(RTDXF0,"VERTEX"DNT, 10,pt0, 40,0.05, 41,0.05, 0);
				elp=new db_vertex(flp);
				elp->entmod(elist,flp);
				flp->addhanditem(elp);
				sds_relrb(elist);
				// Sequence End
			   elist=sds_buildlist(RTDXF0,"SEQEND"DNT,0);
				elp=new db_seqend(flp);
				elp->entmod(elist,flp);
				flp->addhanditem(elp);
				sds_relrb(elist);

		// Under construction - don't mess with this, even if you have it checked out.
			// Dimension - Horizontal
//			pt0[0]=x.x;		pt0[1]=x.x;		pt0[2]=0.0;		// 10 - Position of dimension line (out away from entity)
//			pt1[0]=x.x;		pt1[1]=x.x;		pt1[2]=0.0;		// 11 - Midpoint of dimension text
//			pt2[0]=x.x;		pt2[1]=x.x;		pt2[2]=0.0;		// 13 - First point defining dimension (usually on an entity)
//			pt3[0]=x.x;		pt3[1]=x.x;		pt3[2]=0.0;		// 14 - Second point defining dimension (other end of entity)
//			elist=sds_buildlist(RTDXF0,"DIMENSION", 2,"", 10,pt0, 11,pt1, 70,0, 1,"", 13,pt2, 14,pt3, 50,0.0, 12,pt5, 51,fr1, 210,pView->m_pCurDwgView->ucsaxis[2],0);
//			elp=new db_dimension(flp);						// Change group 50, above, to (IC_PI/2) for a vertical dimension
//			elp->entmod(elist,flp);
//			flp->addhanditem(elp);
//			sds_relrb(elist);
//			cmd_makedimension(elp,flp,0);

		// Under construction - don't mess with this, even if you have it checked out.
			// Ordinate dimension.
			pt0[0]=x.x;		pt0[1]=x.x;		pt0[2]=0.0;
			pt1[0]=x.x;		pt1[1]=x.x;		pt1[2]=0.0;
		   elist=sds_buildlist(RTDXF0,"DIMENSION"DNT,2,""DNT,13,pt0,14,pt1,70,6,0);
			elp=new db_dimension(flp);
			elp->entmod(elist,flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			cmd_makedimension(elp,flp,0);
		// Under construction - don't mess with this, even if you have it checked out.
			// Diameter dimension
			pt0[0]=x.x;		pt0[1]=x.x;		pt0[2]=0.0;
			pt1[0]=x.x;		pt1[1]=x.x;		pt1[2]=0.0;
			pt2[0]=x.x;		pt2[1]=x.x;		pt2[2]=0.0;
			pt3[0]=x.x;		pt3[1]=x.x;		pt3[2]=0.0;
			pt4[0]=x.x; 	pt4[1]=x.x; 	pt4[2]=0.0;
			elist=sds_buildlist(RTDXF0,"DIMENSION",2,ResourceString(IDC_ICADAPI__75, "" ),10,pt0,11,pt4,12,pt4,13,pt1,14,pt4,70,3,1,ResourceString(IDC_ICADAPI__75, "" ),15,pt2,16,pt3,40,0.929346,50,0.0,51,6.28319,52,1.0,0);
			elp=new db_dimension(flp);
			elp->entmod(elist,flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			cmd_makedimension(elp,flp,0);
		// Under construction - don't mess with this, even if you have it checked out..
			// 3 point angular dimension
			pt0[0]=x.x;		pt0[1]=x.x;		pt0[2]=0.0;
			pt1[0]=x.x;		pt1[1]=x.x;		pt1[2]=0.0;
			pt2[0]=x.x;		pt2[1]=x.x;		pt2[2]=0.0;
			pt3[0]=x.x;		pt3[1]=x.x;		pt3[2]=0.0;
			pt4[0]=x.x; 	pt4[1]=x.x; 	pt4[2]=0.0;
		   elist=sds_buildlist(RTDXF0,"DIMENSION"DNT,2,""DNT,10,pt0,11,pt1,70,2,1,""DNT,13,pt2,14,pt3,15,pt4,40,0.839960,50,0.0,0);
			elp=new db_dimension(flp);
			elp->entmod(elist,flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			cmd_makedimension(elp,flp,0);
		// Under construction - don't mess with this, even if you have it checked out.
			// Radius dimension
			pt0[0]=x.x;		pt0[1]=x.x;		pt0[2]=0.0;
			pt1[0]=x.x;		pt1[1]=x.x;		pt1[2]=0.0;
			pt2[0]=x.x;		pt2[1]=x.x;		pt2[2]=0.0;
			pt3[0]=x.x;		pt3[1]=x.x;		pt3[2]=0.0;
			pt4[0]=x.x; 	pt4[1]=x.x; 	pt4[2]=0.0;
			elist=sds_buildlist(RTDXF0,"DIMENSION",2,"",10,pt0,11,pt4,12,pt4,13,pt1,14,pt4,70,4,1,"",15,pt2,16,pt3,40,0.169608,50,4.51208,51,2.53212,0);
			elp=new db_dimension(flp);
			elp->entmod(elist,flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			cmd_makedimension(elp,flp,0);
		// Under construction - don't mess with this, even if you have it checked out.
			// Radius dimension
			pt0[0]=x.x;		pt0[1]=x.x;		pt0[2]=0.0;
			pt1[0]=x.x;		pt1[1]=x.x;		pt1[2]=0.0;
			pt2[0]=x.x;		pt2[1]=x.x;		pt2[2]=0.0;
			pt3[0]=x.x;		pt3[1]=x.x;		pt3[2]=0.0;
			pt4[0]=x.x; 	pt4[1]=x.x; 	pt4[2]=0.0;
		 elist=sds_buildlist(RTDXF0,Re"DIMENSION"DNT,2,""DNT,10,pt0,11,pt4,12,pt4,13,pt1,14,pt4,70,4,1,""DNT,15,pt2,16,pt3,40,0.439460,50,0.0,52,1.0,0);
			elp=new db_dimension(flp);
			elp->entmod(elist,flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			cmd_makedimension(elp,flp,0);
		// Under construction - don't mess with this, even if you have it checked out.
			// 3 point angular dimension
			pt0[0]=x.x;		pt0[1]=x.x;		pt0[2]=0.0;
			pt1[0]=x.x;		pt1[1]=x.x;		pt1[2]=0.0;
			pt2[0]=x.x;		pt2[1]=x.x;		pt2[2]=0.0;
			pt3[0]=x.x;		pt3[1]=x.x;		pt3[2]=0.0;
			pt4[0]=x.x; 	pt4[1]=x.x; 	pt4[2]=0.0;
		   elist=sds_buildlist(RTDXF0,"DIMENSION"DNT,2,""DNT,10,pt0,11,pt1,70,2,1,""DNT,13,pt2,14,pt3,15,pt4,50,0.0,0);
			elp=new db_dimension(flp);
			elp->entmod(elist,flp);
			flp->addhanditem(elp);
			sds_relrb(elist);
			cmd_makedimension(elp,flp,0);  */
