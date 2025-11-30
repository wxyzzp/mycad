/* D:\ICADDEV\PRJ\ICAD\ICADCURSOR.CPP
 * Copyright (C) 1997-1999 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "Icad.h"/*DNT*/
#include "IcadCursor.h"

#include "IcadApp.h"
#include "DrawDevice.h"  /*DNT*/
#include "IcadView.h"
#include "Preferences.h"

#ifndef _SDS_CURSOR_
void IcadCursor::DrawCrosshair(CPoint point, struct gr_view* pViewData, bool always, CDrawDevice* pDrawDevice)
{
	if(always || m_alwaysUseCrosshair)
	{
		struct resbuf rb;
		sds_point pt, pt1, pt2;
		POINT DrPt[6];
		sds_real cs;

		//int	size = IcadCursor::AppCursor().GetCrossHairSize();
		int	size = GetCrossHairSize();
		
		cs=pViewData->viewsize;
		if ( size EQ 100 )
			cs *= 10;
		else
			cs *= size/100.;

		static double addx,addy,lastsnapang,snapangsin,snapangcos;
		static int firstcursor,lastsnapstly,lastsnapisopair;
		static sds_point lastviewdir;

		SDS_getvar(NULL, DB_QSNAPSTYL, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		int snapstyl = rb.resval.rint;
		SDS_getvar(NULL,DB_QSNAPISOPAIR, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		int snapisopair = rb.resval.rint;
		SDS_getvar(NULL, DB_QSNAPANG, &rb, SDS_CURDWG, &SDS_CURCFG,&SDS_CURSES);
		double snapang = rb.resval.rreal;
		SDS_getvar(NULL, DB_QVIEWDIR, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		sds_point viewdir;
		ic_ptcpy(viewdir, rb.resval.rpoint);

		if(!firstcursor || lastsnapstly!=snapstyl || lastsnapisopair!=snapisopair || lastsnapang!=snapang ||
    	   !icadPointEqual(viewdir,lastviewdir))
		{
			firstcursor=1;
			lastsnapstly=snapstyl;
			lastsnapisopair=snapisopair;
			lastsnapang=snapang; 
    		ic_ptcpy(lastviewdir,viewdir);

			addx=0.0;
			addy=0.0;
			if(snapstyl==1) 
			{
				//addx & addy will grab "offset" from std crosshair positions which
				//	are due to ISOMETRIC snap setting ONLY.  Crosshair rotation is 
				//	handled after this is done when the cursor pts are calc'd
				if(icadRealEqual(viewdir[0],0.0) && icadRealEqual(viewdir[1],0.0) && icadRealEqual(viewdir[2],1.0)) { 
					switch(snapisopair) {
						case 0:
							addy=-tan(IC_PI/6.0);
							break;
						case 1:
							addy=tan(IC_PI/6.0);
							addx=tan(IC_PI/3.0);
							break;
						case 2:
							addy=tan(IC_PI/6.0);
							break;
					}
				}
			}
			snapangsin=sin(snapang);
			snapangcos=cos(snapang);
		}

		SDS_getvar(NULL, DB_QCOLORX, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		int colx = rb.resval.rint;
		SDS_getvar(NULL, DB_QCOLORY, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		int coly = rb.resval.rint;
		SDS_getvar(NULL, DB_QCOLORZ, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		int colz = rb.resval.rint;

		//draw X
		gr_pix2rp(pViewData, point.x, point.y, &pt[0], &pt[1]);
		gr_rp2ucs(pt, pt, pViewData);

		/*********long form of code for X looks as follows:*********

		//account for isometric pViewData
		pt3[0]=pt[0]+cs;  pt3[1]=pt[1]+cs*addy;
		//account for snap rotation...
		pt1[0]=(pt3[0]-pt[0])*snapangcos-(pt3[1]-pt[1])*snapangsin+pt[0];
		pt1[1]=(pt3[1]-pt[1])*snapangcos+(pt3[0]-pt[0])*snapangsin+pt[1];
		pt1[2]=pt[2];

		pt3[0]=pt[0]-cs;  pt3[1]=pt[1]-cs*addy;  
		pt2[0]=(pt3[0]-pt[0])*snapangcos-(pt3[1]-pt[1])*snapangsin+pt[0];
		pt2[1]=(pt3[1]-pt[1])*snapangcos+(pt3[0]-pt[0])*snapangsin+pt[1];
		pt2[2]=pt[2];

		**********short form *********/
		pt1[0] = cs * snapangcos - cs * addy * snapangsin + pt[0];
		pt1[1] = cs * addy * snapangcos + cs * snapangsin + pt[1];
		pt1[2] = pt[2];
		pt2[0] = -cs * snapangcos + cs * addy * snapangsin + pt[0];
		pt2[1] = -cs * addy * snapangcos - cs * snapangsin + pt[1];
		pt2[2] = pt[2];

		int result = 0;
		if(pDrawDevice->is3D())
		{
			memcpy((sds_real*)&DrPt[0], pt1, sizeof(sds_point));
			memcpy((sds_real*)&DrPt[0] + 3, pt2, sizeof(sds_point));
		}
		else
		{
		gr_ucs2rp(pt1, pt1, pViewData);
		gr_ucs2rp(pt2, pt2, pViewData);
		result = gr_vect_rp2pix(pt1, pt2, (int*)&DrPt[0], pViewData);
		}
		if(!(DrPt[0].x == DrPt[1].x && DrPt[0].y == DrPt[1].y) && (result == 0 || result == 2))
		{
			pDrawDevice->SetLineColorACAD(colx);
			pDrawDevice->Polyline(DrPt, 2);
		}

		// Draw Y
		gr_pix2rp(pViewData,point.x,point.y,&pt[0],&pt[1]);
		gr_rp2ucs(pt,pt,pViewData);

		/*********Long form of code for Y looks as follows: ********
		//account for isometric pViewData
		pt3[0]=pt[0]-cs*addx;  pt3[1]=pt[1]+cs;
		//account for snap rotation...
		pt1[0]=(pt3[0]-pt[0])*snapangcos-(pt3[1]-pt[1])*snapangsin+pt[0];
		pt1[1]=(pt3[1]-pt[1])*snapangcos+(pt3[0]-pt[0])*snapangsin+pt[1];
		pt1[2]=pt[2];

		pt3[0]=pt[0]+cs*addx;  pt3[1]=pt[1]-cs;  
		pt2[0]=(pt3[0]-pt[0])*snapangcos-(pt3[1]-pt[1])*snapangsin+pt[0];
		pt2[1]=(pt3[1]-pt[1])*snapangcos+(pt3[0]-pt[0])*snapangsin+pt[1];
		pt2[2]=pt[2];
		**********short form is:*********/
		pt1[0]=-cs*addx*snapangcos-cs*snapangsin+pt[0];
		pt1[1]=cs*snapangcos-cs*addx*snapangsin+pt[1];
		pt1[2]=pt[2];
		pt2[0]=cs*addx*snapangcos+cs*snapangsin+pt[0];
		pt2[1]=-cs*snapangcos+cs*addx*snapangsin+pt[1];
		pt2[2]=pt[2];


		if(pDrawDevice->is3D())
		{
			memcpy((sds_real*)&DrPt[0], pt1, sizeof(sds_point));
			memcpy((sds_real*)&DrPt[0] + 3, pt2, sizeof(sds_point));
		}
		else
		{
		gr_ucs2rp(pt1,pt1,pViewData);
		gr_ucs2rp(pt2,pt2,pViewData);
		result = gr_vect_rp2pix(pt1, pt2, (int*)&DrPt[0], pViewData);
		}
		if(!(DrPt[0].x == DrPt[1].x && DrPt[0].y == DrPt[1].y) && (result == 0 || result == 2)) 
		{
			pDrawDevice->SetLineColorACAD(coly);
			pDrawDevice->Polyline(DrPt, 2);
		}

		// Draw Z
		gr_pix2rp(pViewData, point.x, point.y, &pt[0], &pt[1]);
		gr_rp2ucs(pt, pt, pViewData);
		pt1[0] = pt[0];  pt1[1] = pt[1];  pt1[2] = pt[2] + cs;
		pt2[0] = pt[0];  pt2[1] = pt[1];  pt2[2] = pt[2] - cs;

		if(pDrawDevice->is3D())
		{
			memcpy((sds_real*)&DrPt[0], pt1, sizeof(sds_point));
			memcpy((sds_real*)&DrPt[0] + 3, pt2, sizeof(sds_point));
		}
		else
		{
		gr_ucs2rp(pt1, pt1, pViewData);
		gr_ucs2rp(pt2, pt2, pViewData);
		result = gr_vect_rp2pix(pt1, pt2, (int*)&DrPt[0], pViewData);
		}
		if(!(DrPt[0].x == DrPt[1].x && DrPt[0].y == DrPt[1].y) && (result == 0 || result == 2))
		{
			pDrawDevice->SetLineColorACAD(colz);
			pDrawDevice->Polyline(DrPt, 2);
		}
	}
}

void IcadCursor::DrawPickbox(CPoint point, int curtype, CDrawDevice* pDrawDevice, struct gr_view* pViewData)
{
	struct resbuf rb;
	POINT DrPt[5];
	sds_point pt1, pt2;
	int result;
	
	// Draw Pickbox.
	int pb;
	
	// Don't draw the pick box if there is nothing to pick.
	if(SDS_CURDWG && SDS_CURDWG->ret_nmainents()==0)
		return;
	
	// Don't draw the pick box for IDC_ICAD_SNAP if it's not enabled.
	if(curtype == IDC_ICAD_SNAP && UserPreference::SnapApertureBoxPreference == false )
		return;
	
	if(curtype == IDC_ICAD_CROSS || curtype == IDC_ICAD_PICKBOX)
	{ 
		SDS_getvar(NULL, DB_QPICKBOX, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		if(rb.resval.rint == 0)
			return;
		pb = rb.resval.rint;
	}
	else 
	{
		SDS_getvar(NULL, DB_QAPERTURE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
		if(rb.resval.rint == 0) 
			return;
		pb = rb.resval.rint;
	}
	pDrawDevice->SetLineColorACAD(255);
	DrPt[0].x = point.x + pb; DrPt[0].y = point.y + pb;
	DrPt[1].x = point.x - pb; DrPt[1].y = point.y + pb;
	gr_pix2rp(pViewData, DrPt[0].x, DrPt[0].y, &pt1[0], &pt1[1]);
	gr_pix2rp(pViewData, DrPt[1].x, DrPt[1].y, &pt2[0], &pt2[1]);
	result = gr_vect_rp2pix(pt1, pt2, (int*)&DrPt[0], pViewData);
	if(!(DrPt[0].x == DrPt[1].x && DrPt[0].y == DrPt[1].y) && (result == 0 || result == 2))
		pDrawDevice->Polyline(&DrPt[0], 2);

	DrPt[2].x = point.x - pb; DrPt[2].y = point.y - pb;
	gr_pix2rp(pViewData, DrPt[2].x, DrPt[2].y, &pt1[0], &pt1[1]);
	result = gr_vect_rp2pix(pt1, pt2, (int*)&DrPt[1], pViewData);
	if(!(DrPt[2].x == DrPt[1].x && DrPt[2].y == DrPt[1].y) && (result == 0 || result == 2))
		pDrawDevice->Polyline(&DrPt[1], 2);

	DrPt[3].x = point.x + pb; DrPt[3].y = point.y - pb;
	gr_pix2rp(pViewData, DrPt[3].x, DrPt[3].y, &pt2[0], &pt2[1]);
	result = gr_vect_rp2pix(pt1, pt2, (int*)&DrPt[2], pViewData);
	if(!(DrPt[2].x == DrPt[3].x && DrPt[2].y == DrPt[3].y) && (result == 0 || result == 2))
		pDrawDevice->Polyline(&DrPt[2], 2);

	DrPt[4].x = point.x + pb; DrPt[4].y = point.y + pb;
	gr_pix2rp(pViewData, DrPt[4].x, DrPt[4].y, &pt1[0], &pt1[1]);
	result = gr_vect_rp2pix(pt1, pt2, (int*)&DrPt[3], pViewData);
	if(!(DrPt[4].x == DrPt[3].x && DrPt[4].y == DrPt[3].y) && (result == 0 || result == 2))
		pDrawDevice->Polyline(&DrPt[3], 2);
}

static void 
DrawCursorInCdc(CPoint point, int curtype, struct gr_view* pViewData, CDrawDevice* pDrawDevice)
{
	pDrawDevice->SetDrawMode(RasterConstants::MODE_XOR);
	switch(curtype) 
	{
		case IDC_ICAD_HAND:
		case IDC_ZOOM:
		case IDC_ICAD_OPENHAND:
		case IDC_ICAD_PENCIL:
		case IDC_ICAD_WINDOW:
		case IDC_ICAD_CROSSING:
		case IDC_ICAD_OUTSIDE:
			break;

		case IDC_ICAD_PICK1_NOCUR:
		case IDC_ICAD_PICK2_NOCUR:
		case IDC_ICAD_PICK3_NOCUR:
		case IDC_ICAD_PICK4_NOCUR:
		case IDC_ICAD_PICK5_NOCUR:
		case IDC_ICAD_PICK6_NOCUR:
		case IDC_ICAD_PICK7_NOCUR:
		case IDC_ICAD_PICK8_NOCUR:
		case IDC_ICAD_PICK9_NOCUR:
		case IDC_ICAD_PICK10_NOCUR:
								// crosshair not unconditional
			IcadCursor::AppCursor().DrawCrosshair(point, pViewData, false, pDrawDevice);
			break;

		case IDC_ICAD_SNAP:
		case IDC_ICAD_CROSS:
		case IDC_CENTER:
		case IDC_ENDPOINT:
		case IDC_INSERT:
		case IDC_INTERSECT:
		case IDC_PERPEN:
		case IDC_MIDPOINT:
		case IDC_NEAR:
		case IDC_NODE:
		case IDC_QUAD:
		case IDC_TANGENT:
		case IDC_ICAD_OS_INT3D:
		case IDC_ICAD_OS_INTVS:
		case IDC_ICAD_OS_INT2D:
								// crosshair is unconditional
			IcadCursor::AppCursor().DrawCrosshair(point, pViewData, true, pDrawDevice);
			if(curtype EQ IDC_ICAD_CROSS)
				break;

		case IDC_ICAD_PICKBOX:
		case IDC_ICAD_PICKBOX_NOCUR:
			IcadCursor::AppCursor().DrawPickbox(point, curtype, pDrawDevice, pViewData);
			break;
	}
}
#else

void
IcadCursor::DrawCrosshair( CPoint point, struct gr_view *view, CDC *cdc, bool always)
	{
	if ( always || m_alwaysUseCrosshair )
		{
		HGDIOBJ SavObj=NULL;
		struct resbuf rb;
		sds_point pt,pt1,pt2;
		POINT DrPt[5];
		sds_real cs;

		int	size = IcadCursor::AppCursor().GetCrossHairSize();
		
		cs=view->viewsize;
		if ( size EQ 100 )
			cs *= 10;
		else
			cs *= size/100.;

		static double addx,addy,lastsnapang,snapangsin,snapangcos;
		static int firstcursor,lastsnapstly,lastsnapisopair;
		static sds_point lastviewdir;

		SDS_getvar(NULL,DB_QSNAPSTYL,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		int snapstyl=rb.resval.rint;
		SDS_getvar(NULL,DB_QSNAPISOPAIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		int snapisopair=rb.resval.rint;
		SDS_getvar(NULL,DB_QSNAPANG,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		double snapang=rb.resval.rreal;
		SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		sds_point viewdir;
		ic_ptcpy(viewdir,rb.resval.rpoint);

		if(!firstcursor || 
			lastsnapstly!=snapstyl || lastsnapisopair!=snapisopair || lastsnapang!=snapang ||
    		!icadPointEqual(viewdir,lastviewdir)) 
		{
			firstcursor=1;
			lastsnapstly=snapstyl;
			lastsnapisopair=snapisopair;
			lastsnapang=snapang; 
    		ic_ptcpy(lastviewdir,viewdir);

			addx=0.0;
			addy=0.0;

			if(snapstyl==1) {
				//addx & addy will grab "offset" from std crosshair positions which
				//	are due to ISOMETRIC snap setting ONLY.  Crosshair rotation is 
				//	handled after this is done when the cursor pts are calc'd
				if(icadRealEqual(viewdir[0],0.0) && icadRealEqual(viewdir[1],0.0) && icadRealEqual(viewdir[2],1.0)) { 
					switch(snapisopair) {
						case 0:
							addy=-tan(IC_PI/6.0);
							break;
						case 1:
							addy=tan(IC_PI/6.0);
							addx=tan(IC_PI/3.0);
							break;
						case 2:
							addy=tan(IC_PI/6.0);
							break;
					}
				}
			}
			snapangsin=sin(snapang);
			snapangcos=cos(snapang);
		}

		//draw X
		gr_pix2rp(view,point.x,point.y,&pt[0],&pt[1]);
		gr_rp2ucs(pt,pt,view);

		/*********long form of code for X looks as follows:*********

		//account for isometric view
		pt3[0]=pt[0]+cs;  pt3[1]=pt[1]+cs*addy;
		//account for snap rotation...
		pt1[0]=(pt3[0]-pt[0])*snapangcos-(pt3[1]-pt[1])*snapangsin+pt[0];
		pt1[1]=(pt3[1]-pt[1])*snapangcos+(pt3[0]-pt[0])*snapangsin+pt[1];
		pt1[2]=pt[2];

		pt3[0]=pt[0]-cs;  pt3[1]=pt[1]-cs*addy;  
		pt2[0]=(pt3[0]-pt[0])*snapangcos-(pt3[1]-pt[1])*snapangsin+pt[0];
		pt2[1]=(pt3[1]-pt[1])*snapangcos+(pt3[0]-pt[0])*snapangsin+pt[1];
		pt2[2]=pt[2];

		**********short form *********/
		pt1[0]=cs*snapangcos-cs*addy*snapangsin+pt[0];
		pt1[1]=cs*addy*snapangcos+cs*snapangsin+pt[1];
		pt1[2]=pt[2];
		pt2[0]=-cs*snapangcos+cs*addy*snapangsin+pt[0];
		pt2[1]=-cs*addy*snapangcos-cs*snapangsin+pt[1];
		pt2[2]=pt[2];

		gr_ucs2rp(pt1,pt1,view);
		gr_rp2pix(view,pt1[0],pt1[1],(int *)&DrPt[0].x,(int *)&DrPt[0].y);
		gr_ucs2rp(pt2,pt2,view);
		gr_rp2pix(view,pt2[0],pt2[1],(int *)&DrPt[1].x,(int *)&DrPt[1].y);

		SDS_getvar(NULL,DB_QCOLORX,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		int colx=rb.resval.rint;
		SDS_getvar(NULL,DB_QCOLORY,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		int coly=rb.resval.rint;
		SDS_getvar(NULL,DB_QCOLORZ,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		int colz=rb.resval.rint;
		SavObj=SelectObject(cdc->m_hDC,CreatePen(PS_SOLID,1,SDS_XORPenColorFromACADColor(colx)));
		if(!(DrPt[0].x==DrPt[1].x && DrPt[0].y==DrPt[1].y)) {
			::Polyline(cdc->m_hDC,DrPt,2);
		}

		// Draw Y
		gr_pix2rp(view,point.x,point.y,&pt[0],&pt[1]);
		gr_rp2ucs(pt,pt,view);

		/*********Long form of code for Y looks as follows: ********
		//account for isometric view
		pt3[0]=pt[0]-cs*addx;  pt3[1]=pt[1]+cs;
		//account for snap rotation...
		pt1[0]=(pt3[0]-pt[0])*snapangcos-(pt3[1]-pt[1])*snapangsin+pt[0];
		pt1[1]=(pt3[1]-pt[1])*snapangcos+(pt3[0]-pt[0])*snapangsin+pt[1];
		pt1[2]=pt[2];

		pt3[0]=pt[0]+cs*addx;  pt3[1]=pt[1]-cs;  
		pt2[0]=(pt3[0]-pt[0])*snapangcos-(pt3[1]-pt[1])*snapangsin+pt[0];
		pt2[1]=(pt3[1]-pt[1])*snapangcos+(pt3[0]-pt[0])*snapangsin+pt[1];
		pt2[2]=pt[2];
		**********short form is:*********/
		pt1[0]=-cs*addx*snapangcos-cs*snapangsin+pt[0];
		pt1[1]=cs*snapangcos-cs*addx*snapangsin+pt[1];
		pt1[2]=pt[2];
		pt2[0]=cs*addx*snapangcos+cs*snapangsin+pt[0];
		pt2[1]=-cs*snapangcos+cs*addx*snapangsin+pt[1];
		pt2[2]=pt[2];


		gr_ucs2rp(pt1,pt1,view);
		gr_rp2pix(view,pt1[0],pt1[1],(int *)&DrPt[0].x,(int *)&DrPt[0].y);
		gr_ucs2rp(pt2,pt2,view);
		gr_rp2pix(view,pt2[0],pt2[1],(int *)&DrPt[1].x,(int *)&DrPt[1].y);
		if(!(DrPt[0].x==DrPt[1].x && DrPt[0].y==DrPt[1].y)) {
			DeleteObject(SelectObject(cdc->m_hDC,CreatePen(PS_SOLID,1,SDS_XORPenColorFromACADColor(coly))));
			::Polyline(cdc->m_hDC,DrPt,2);
		}

		// Draw Z
		gr_pix2rp(view,point.x,point.y,&pt[0],&pt[1]);
		gr_rp2ucs(pt,pt,view);
		pt1[0]=pt[0];  pt1[1]=pt[1];  pt1[2]=pt[2]+cs;
		pt2[0]=pt[0];  pt2[1]=pt[1];  pt2[2]=pt[2]-cs;
		gr_ucs2rp(pt1,pt1,view);
		gr_rp2pix(view,pt1[0],pt1[1],(int *)&DrPt[0].x,(int *)&DrPt[0].y);
		gr_ucs2rp(pt2,pt2,view);
		gr_rp2pix(view,pt2[0],pt2[1],(int *)&DrPt[1].x,(int *)&DrPt[1].y);
		if(!(DrPt[0].x==DrPt[1].x && DrPt[0].y==DrPt[1].y)) {
			DeleteObject(SelectObject(cdc->m_hDC,CreatePen(PS_SOLID,1,SDS_XORPenColorFromACADColor(colz))));
			::Polyline(cdc->m_hDC,DrPt,2);
		}
		
		DeleteObject(SelectObject(cdc->m_hDC,SavObj));
		}
	}


static void
DrawPickbox( CPoint point, struct gr_view *view, CDC *cdc, int curtype)
	{
	HGDIOBJ SavObj=NULL;
	struct resbuf rb;
	POINT DrPt[5];

	// Draw Pickbox.
	int pb;

	// Don't draw the pick box if there is nothing to pick.
	if(SDS_CURDWG && SDS_CURDWG->ret_nmainents()==0)
		return;
	
	// Don't draw the pick box for IDC_ICAD_SNAP if it's not enabled.
	if( curtype == IDC_ICAD_SNAP && UserPreference::SnapApertureBoxPreference == false )
		return;

	if(curtype==IDC_ICAD_CROSS || curtype==IDC_ICAD_PICKBOX) { 
		SDS_getvar(NULL,DB_QPICKBOX,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if(rb.resval.rint==0)
			return;
		pb=rb.resval.rint;
	} else {
		SDS_getvar(NULL,DB_QAPERTURE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if(rb.resval.rint==0) 
			return;
		pb=rb.resval.rint;
	}
	
	DrPt[0].x=point.x+pb; DrPt[0].y=point.y+pb;
	DrPt[1].x=point.x-pb; DrPt[1].y=point.y+pb;
	DrPt[2].x=point.x-pb; DrPt[2].y=point.y-pb;
	DrPt[3].x=point.x+pb; DrPt[3].y=point.y-pb;
	DrPt[4].x=point.x+pb; DrPt[4].y=point.y+pb;
	SavObj=SelectObject(cdc->m_hDC,CreatePen(PS_SOLID,1,SDS_PenColorFromACADColor(255)));
	::Polyline(cdc->m_hDC,DrPt,5);
	DeleteObject(SelectObject(cdc->m_hDC,SavObj));
	}


static void 
DrawCursorInCdc(CPoint point, int curtype, struct gr_view *view,CDC *cdc) {
	int rop;


	rop=SetROP2(cdc->m_hDC,R2_XORPEN);

	switch(curtype) {
		case IDC_ICAD_HAND:
		case IDC_ZOOM:
		case IDC_ICAD_OPENHAND:
		case IDC_ICAD_PENCIL:
		case IDC_ICAD_WINDOW:
		case IDC_ICAD_CROSSING:
		case IDC_ICAD_OUTSIDE:
			break;

		case IDC_ICAD_PICK1_NOCUR:
		case IDC_ICAD_PICK2_NOCUR:
		case IDC_ICAD_PICK3_NOCUR:
		case IDC_ICAD_PICK4_NOCUR:
		case IDC_ICAD_PICK5_NOCUR:
		case IDC_ICAD_PICK6_NOCUR:
		case IDC_ICAD_PICK7_NOCUR:
		case IDC_ICAD_PICK8_NOCUR:
		case IDC_ICAD_PICK9_NOCUR:
		case IDC_ICAD_PICK10_NOCUR:
								// crosshair not unconditional
			IcadCursor::AppCursor().DrawCrosshair( point, view, cdc, false); 
			break;

		case IDC_ICAD_SNAP:
		case IDC_ICAD_CROSS:
		case IDC_CENTER:
		case IDC_ENDPOINT:
		case IDC_INSERT:
		case IDC_INTERSECT:
		case IDC_PERPEN:
		case IDC_MIDPOINT:
		case IDC_NEAR:
		case IDC_NODE:
		case IDC_QUAD:
		case IDC_TANGENT:
		case IDC_ICAD_OS_INT3D:
		case IDC_ICAD_OS_INTVS:
		case IDC_ICAD_OS_INT2D:
								// crosshair is unconditional
			IcadCursor::AppCursor().DrawCrosshair( point, view, cdc, true); 
			if( curtype EQ IDC_ICAD_CROSS ) 
				break;

		case IDC_ICAD_PICKBOX:
		case IDC_ICAD_PICKBOX_NOCUR:
			DrawPickbox( point, view, cdc, curtype);
			break;
	}
	SetROP2(cdc->m_hDC,rop);
}


static int 
SDS_DrawCursor(CPoint point, int curtype, CIcadView *pView) {

	if(pView==NULL) pView=SDS_CURVIEW;
	if(pView==NULL) return(RTERROR);
	if(!IsWindow(pView->m_hWnd)) return(RTERROR);
	struct gr_view *view=pView->m_pCurDwgView;
	if(view==NULL)  return(RTERROR);

	CDC *cdc=pView->GetDC();
	CPalette *oldpal=cdc->SelectPalette(SDS_CMainWindow->m_pPalette,TRUE);
	if(pView->m_pClipRectDC) 
		cdc->IntersectClipRect(pView->m_pClipRectDC);
	DrawCursorInCdc(point,curtype,view,cdc);
//	cdc->SelectPalette(oldpal,TRUE);
	if(pView->m_pClipRectMemDC) 
		cdc->IntersectClipRect(pView->m_pClipRectMemDC);
	DrawCursorInCdc(point,curtype,view,pView->GetFrameBufferCDC());
	pView->ReleaseDC(cdc);		

	return(RTNORM);
}
#endif

int 
SDS_SetCursor( int mode) 
	{
	if (IcadCursor::AppCursor().GetCrosshairState()) {
		switch(mode) {
		case IDC_ICAD_PICK1:
			mode = IDC_ICAD_PICK1_NOCUR;
			break;
		case IDC_ICAD_PICK2:
			mode = IDC_ICAD_PICK2_NOCUR;
			break;
		case IDC_ICAD_PICK3:
			mode = IDC_ICAD_PICK3_NOCUR;
			break;
		case IDC_ICAD_PICK4:
			mode = IDC_ICAD_PICK4_NOCUR;
			break;
		case IDC_ICAD_PICK5:
			mode = IDC_ICAD_PICK5_NOCUR;
			break;
		case IDC_ICAD_PICK6:
			mode = IDC_ICAD_PICK6_NOCUR;
			break;
		case IDC_ICAD_PICK7:
			mode = IDC_ICAD_PICK7_NOCUR;
			break;
		case IDC_ICAD_PICK8:
			mode = IDC_ICAD_PICK8_NOCUR;
			break;
		case IDC_ICAD_PICK9:
			mode = IDC_ICAD_PICK9_NOCUR;
			break;
		case IDC_ICAD_PICK10:
			mode = IDC_ICAD_PICK10_NOCUR;
			break;
		case IDC_ICAD_PICKBOX:
			mode = IDC_ICAD_PICKBOX_NOCUR;
			break;
		};
	}
    return IcadCursor::AppCursor().SetType( mode);
	}

int SDS_GetPickCursor()
{
	int cur = IDC_ICAD_PICKBOX;
	struct resbuf rb;
	SDS_getvar(NULL, DB_QPICKBOX, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	switch(rb.resval.rint) 
	{
		case 1:	 cur=IDC_ICAD_PICK1;  break;
		case 2:	 cur=IDC_ICAD_PICK2;  break;
		case 3:	 cur=IDC_ICAD_PICK3;  break;
		case 4:	 cur=IDC_ICAD_PICK4;  break;
		case 5:	 cur=IDC_ICAD_PICK5;  break;
		case 6:	 cur=IDC_ICAD_PICK6;  break;
		case 7:	 cur=IDC_ICAD_PICK7;  break;
		case 8:	 cur=IDC_ICAD_PICK8;  break;
		case 9:	 cur=IDC_ICAD_PICK9;  break;
		case 10: cur=IDC_ICAD_PICK10; break;
	}
	return(cur);
}


void IcadCursor::readFromRegistry()
{
	m_alwaysUseCrosshair = UserPreference::crosshairPreference;
	m_sizePercent = UserPreference::crosshairSizePreference;
	m_bEnableGripsAttraction = 	UserPreference::GripsAttractionPreference;
	m_AttractionDistance = UserPreference::GripsAttractionDistancePreference;
}

IcadCursor& 
IcadCursor::AppCursor()
	{
	CIcadApp	*app = GetApp();
	return *app->m_cursor;
	}


bool 
IcadCursor:: AlwaysUseCrosshair( bool state)
	{
	bool	previous = m_alwaysUseCrosshair;

	UserPreference::crosshairPreference = m_alwaysUseCrosshair = state;
	return previous;
	}


bool 
IcadCursor:: CursorAt( CPoint&, class IcadView *)
	{
	return true;
	}


int 
IcadCursor::DrawCursor(CPoint point, int curtype, CIcadView *pView)
	{
#ifndef _SDS_CURSOR_
	if(pView == NULL)
		pView = SDS_CURVIEW;
	if(pView == NULL)
		return RTERROR;
	if(!IsWindow(pView->m_hWnd))
		return RTERROR;
	struct gr_view* view = pView->m_pCurDwgView;
	if(view == NULL)
		return(RTERROR);

	CDC* pDC = pView->GetDC();
	CPalette* pPrevPalette = pDC->SelectPalette(SDS_CMainWindow->m_pPalette, TRUE);
	if(pView->m_pClipRectDC) 
		pDC->IntersectClipRect(pView->m_pClipRectDC);

	CDrawDevice* pDrawDevice = pView->GetDrawDevice();
	pDrawDevice->beginDraw(pDC, view);

	int drawBuffer = pDrawDevice->setDrawBuffer(CDrawDevice::eFRONT);
	DrawCursorInCdc(point, curtype, view, pDrawDevice);
	pDrawDevice->setDrawBuffer((CDrawDevice::EDrawBuffer)drawBuffer);

//	cdc->SelectPalette(oldpal,TRUE);
	if(pView->m_pClipRectMemDC) 
		pDC->IntersectClipRect(pView->m_pClipRectMemDC);

	DrawCursorInCdc(point, curtype, view, pDrawDevice);

	pDrawDevice->endDraw();
	pView->ReleaseDrawDevice(pDrawDevice);
	pView->ReleaseDC(pDC);		
	return RTNORM;
#else
	return SDS_DrawCursor( point, curtype, pView);
#endif
	}


void 
IcadCursor::SetCrossHairSize( short percent)
	{
	UserPreference::crosshairSizePreference = m_sizePercent = percent;
	}


int	 
IcadCursor::SetType( int mode)
	{
	struct resbuf rb;
	SDS_getvar(NULL,DB_QVIEWMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(rb.resval.rint&1) { // In Perspective.
		switch(mode) {
			case IDC_ICAD_SNAP:
			case IDC_ICAD_CROSS:
			case IDC_CENTER:
			case IDC_ENDPOINT:
			case IDC_INSERT:
			case IDC_INTERSECT:
			case IDC_PERPEN:
			case IDC_MIDPOINT:
			case IDC_NEAR:
			case IDC_NODE:
			case IDC_QUAD:
			case IDC_TANGENT:
			case IDC_ICAD_OS_INT3D:
			case IDC_ICAD_OS_INTVS:
			case IDC_ICAD_OS_INT2D:
				mode=IDC_ICAD_PERSPECTIVE;
		}
	}
    if(((int)SDS_CMainWindow->m_ViewCursorType)==mode) 
		return(mode);
	int ret=SDS_CMainWindow->m_ViewCursorType;
    SDS_CMainWindow->m_ViewCursorType=mode;

	CIcadView *pView=SDS_CURVIEW;
	if ( pView )
		pView->CursorID( m_idcCursor = mode);

	return ret;
	}


bool
IcadCursor:: SetVisible( bool isVisible)
	{
	return false;
	}

bool
IcadCursor::EnableGripsAttraction(bool state)
	{
	bool	previous = m_bEnableGripsAttraction;

	UserPreference::GripsAttractionPreference = m_bEnableGripsAttraction = state;
	return previous;
	}

void 
IcadCursor::SetAttractionDistance(int points)
	{
	UserPreference::GripsAttractionDistancePreference = m_AttractionDistance = points;
	}
