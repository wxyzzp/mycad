/* C:\DEV\PRJ\ICAD\ICADGRIDICONSNAP.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 


// ** Includes
#include "Icad.h"/*DNT*/
#include "icadgridiconsnap.h"
#include "IcadDoc.h"
#include "DrawDevice.h"  /*DNT*/
#include "cmds.h"/*DNT*/
#include "IcadView.h"
#include "IcadEntityRenderer.h"

/**************************************************************************/
/***************** Snap/Grid/Cursor/Icon/Blip functions *******************/
/**************************************************************************/ 

int SDS_SnapPt(const sds_point startpt, sds_point snappt)
{
	//This is a 2D f'n.  The Z of snappt will be the Z of startpt
	//F'n returns RTNONE if snap mode is off, RTNORM otherwise

	sds_real fr1,fr2,fr3,gridxunit,gridyunit,angsin,angcos;
	sds_point snapbase,p0,p1;
	int snapstyl;
	sds_resbuf rb;

	SDS_getvar(NULL,DB_QSNAPMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if(rb.resval.rint==0){
		ic_ptcpy(snappt,startpt);
		return(RTNONE);
	}

	SDS_getvar(NULL,DB_QSNAPSTYL,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	snapstyl=rb.resval.rint;

	SDS_getvar(NULL,DB_QSNAPUNIT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	if((icadRealEqual(rb.resval.rpoint[0],0.0))||(icadRealEqual(rb.resval.rpoint[1],0.0))){
		SDS_getvar(NULL,DB_QGRIDUNIT,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		gridxunit=rb.resval.rpoint[0];
		gridyunit=rb.resval.rpoint[1];
		if(snapstyl)gridyunit*=0.5;	
	}else{
		gridxunit=rb.resval.rpoint[0];
		gridyunit=rb.resval.rpoint[1];
	}

	SDS_getvar(NULL,DB_QSNAPANG,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	fr1=rb.resval.rreal;
	angsin=sin(fr1);
	angcos=cos(fr1);

	SDS_getvar(NULL,DB_QSNAPBASE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(snapbase,rb.resval.rpoint);


	//if snap style is isometric, adjust the grid so that it draws for isometric
	//projection - but don't reset any of the variables
	//NOTE:Any changes to this will affect how we calculate
	//	near point in isometric mode below
	if((snapstyl)&&(fabs((gridyunit*2.0*1.732050807568877/gridxunit)-1.0)>0.00001)){
		gridxunit=gridyunit*2.0*1.732050807568877;
	}

	//We set up a ucs w/origin at snapbase & X axis along snapang
	//This makes the math a WHOLE lot easier.
	//convert point we're starting with into our ucs
	
	p0[0]=(startpt[0]-snapbase[0])*angcos+(startpt[1]-snapbase[1])*angsin;
	p0[1]=(snapbase[0]-startpt[0])*angsin+(startpt[1]-snapbase[1])*angcos;

	if((icadRealEqual(p0[0],0.0))&&(icadRealEqual(p0[1],0.0))) goto convert;
	fr3=modf((p0[0]/gridxunit),&fr1);
	p1[0]=fr1*gridxunit;
	fr2=modf((p0[1]/gridyunit),&fr1);
	p1[1]=fr1*gridyunit;
	p1[2]=0.0;
	//At this pt, p1 is corner of surrounding cell closest to origin
	if(snapstyl==0){	//just go to the nearest grid corner
		//set the y
		if(fr2<=-0.5)	 p0[1]=p1[1]-gridyunit;
		else if(fr2>=0.5)p0[1]=p1[1]+gridyunit;
		else			 p0[1]=p1[1];
		//set the x
		if(fr3<=-0.5)	 p0[0]=p1[0]-gridxunit;
		else if(fr3>=0.5)p0[0]=p1[0]+gridxunit;
		else			 p0[0]=p1[0];
	}else{				//for isometric snap every other row is offset half cell in X dir	
		if(!icadRealEqual(fmod(fr1,2.0),0.0)){	//an odd-numbered Y grid cell
			if((fr3*fr3*3.0+fabs(1.0-fr2)*fabs(1.0-fr2))<=(0.25*(3.0*0.5*0.5+1.0))){
				p0[0]=p1[0];
				if(p0[1]<0.0)p0[1]=p1[1]-gridyunit;
				else		 p0[1]=p1[1]+gridyunit;
			}else if((fabs(1.0-fr3)*fabs(1.0-fr3)*3.0+fabs(1.0-fr2)*fabs(1.0-fr2))<=(0.25*(3.0*0.5*0.5+1.0))){
				if(p0[0]<0.0)p0[0]=p1[0]-gridxunit;
				else		 p0[0]=p1[0]+gridxunit;
				if(p0[1]<0.0)p0[1]=p1[1]-gridyunit;
				else		 p0[1]=p1[1]+gridyunit;
			}else{
				p0[1]=p1[1];
 				if(p0[0]<0.0)p0[0]=p1[0]-0.5*gridxunit;
				else		 p0[0]=p1[0]+0.5*gridxunit;
			}
	 	}else{					//an even numbered Y cell
			if((fr3*fr3*3.0+fr2*fr2)<=(0.25*(3.0*0.5*0.5+1.0))){
				ic_ptcpy(p0,p1);
			}else if(((1.0-fabs(fr3))*(1.0-fabs(fr3))*3.0+fr2*fr2)<=(0.25*(3.0*0.5*0.5+1.0))){
				p0[1]=p1[1];
				if(p0[0]<0.0)p0[0]=p1[0]-gridxunit;
				else		 p0[0]=p1[0]+gridxunit;
			}else{
				if(p0[0]<0.0)p0[0]=p1[0]-0.5*gridxunit;
				else		 p0[0]=p1[0]+0.5*gridxunit;
				if(p0[1]<0.0)p0[1]=p1[1]-gridyunit;
				else		 p0[1]=p1[1]+gridyunit;
			}
		}
	}
convert:
	//now convert p0 back into the coordinate system it came from
	snappt[0]=p0[0]*angcos-p0[1]*angsin+snapbase[0];
	snappt[1]=p0[0]*angsin+p0[1]*angcos+snapbase[1];
	snappt[2]=startpt[2];

	return(RTNORM);
}

int SDS_DrawGrid(CIcadView* pView, struct gr_view* view)
{
#ifdef _SDS_GRID_
	//This f'n draws in the grid using sds_grdraw.  It returns RTNORM
	//if able to display the grid (even if no grid points lie
	//within current view), and RTERROR if the grid is too dense
	//to display.
	//NOTE: If SNAPSTYL == 1, the grid is ALWAYS drawn in isometric fashion,
	//forcing the X distance to isometrically match the Y.  Y is retrieved
	//from GRIDUNIT[1](or SNAPUNIT[1] if GRIDUNIT is (0,0)).  THIS IS DIFFERENT
	//FROM AUTOCAD - acad doesn't force. Regardless of SNAPSTYL, grid is drawn at
	//angle of SNAPANG.

	struct resbuf rb;
	int snapstyl,flag,toobig,tilemode,PixelX,PixelY;
	sds_real gridxunit,gridyunit,griddensity,fr1,fr2,angsin,angcos;
	sds_point limptur,limptll,snapbase,p1,p2,p3,p4,p5,p6;

	if(pView==NULL) pView=SDS_CURVIEW;
	if(pView==NULL) return(RTERROR);
	if(view==NULL) view=pView->m_pCurDwgView;
	if(view==NULL) return(RTERROR);

	if(pView->m_pViewsDoc==NULL) return(RTERROR);
	db_drawing *flp=pView->m_pViewsDoc->m_dbDrawing;
	if(flp==NULL) return(RTERROR);
	if(pView->m_pVportTabHip==NULL) return(RTERROR);

	if(pView->GetRealTime() != NULL)
		return RTNORM;
	
	// Make sure the corect setvars are in this table entry.
	// Because this function looks at the table entry directly.
//XX	if(pView==SDS_CURVIEW) SDS_VarToTab(flp,pView->m_pVportTabHip);

	griddensity=0.33;	//maximum part of screen width or height which
						//may be taken up by pixels for drawing grid
	//NOTE:  Acad uses 0.33, meaning 1/3 of pixels can be used for grid.
	//	We don't want to display too dense a grid

	toobig=1;		//is grid so big that none of the grid points show up?

	SDS_getvar(NULL, DB_QTILEMODE, &rb, flp, &SDS_CURCFG, &SDS_CURSES);
	tilemode = rb.resval.rint;

	if(tilemode == 0) 
	{
		SDS_getvar(NULL, DB_QGRIDMODE, &rb, flp, &SDS_CURCFG, &SDS_CURSES);
		flag = rb.resval.rint;
	} 
	else
	{
		pView->m_pVportTabHip->get_76(&flag);
	}
	if(flag == 0)
		return RTNORM;

	if(tilemode==0)
	{
		SDS_getvar(NULL, DB_QSNAPSTYL, &rb, flp, &SDS_CURCFG, &SDS_CURSES);
		snapstyl = rb.resval.rint;
		SDS_getvar(NULL, DB_QGRIDUNIT, &rb, flp, &SDS_CURCFG, &SDS_CURSES);
		ic_ptcpy(p1, rb.resval.rpoint);
	}
	else
	{
		pView->m_pVportTabHip->get_77(&snapstyl);
		pView->m_pVportTabHip->get_15(p1);
	}
	if((icadRealEqual(p1[0],0.0))||(icadRealEqual(p1[1],0.0)))
	{
		if(tilemode == 0)
		{
			SDS_getvar(NULL, DB_QSNAPUNIT,&rb, flp, &SDS_CURCFG, &SDS_CURSES);
			ic_ptcpy(p1, rb.resval.rpoint);
		} else {
			pView->m_pVportTabHip->get_14(p1);
		}
		gridxunit = p1[0];
		gridyunit = p1[1];
		if(snapstyl)
			gridyunit *= 0.5;	//don't ask me why!
	}
	else
	{
		gridxunit=p1[0];
		gridyunit=p1[1];
	}

	if(tilemode == 0)
	{
		SDS_getvar(NULL, DB_QSNAPANG, &rb, flp, &SDS_CURCFG, &SDS_CURSES);
		fr1 = rb.resval.rreal;
	}
	else 
	{
		pView->m_pVportTabHip->get_50(&fr1);
	}
	angsin = sin(fr1);
	angcos = cos(fr1);

	//if snap style is isometric, adjust the grid so that it draws for isometric
	//projection - but don't reset any of the variables
	if(snapstyl && fabs((gridyunit * 2.0 * 1.732050807568877 / gridxunit) - 1.0) > 0.00001)
	{
		gridxunit = gridyunit * 2.0 * 1.732050807568877;
	}

	//Get viewctr & convert from ucs to rp to calculate corners of
	//screen.  Then project corners back from rp to UCS.  
	//pView->m_pVportTabHip->get_12(p2);	//I think we changed this so it's in RP, so next line not needed
	//gr_ucs2rp(p2,p1,view);
	if(tilemode ==0 ) 
	{
		SDS_getvar(NULL, DB_QVIEWCTR, &rb, flp, &SDS_CURCFG, &SDS_CURSES);
		ic_ptcpy(p1, rb.resval.rpoint);
		struct resbuf rb2;
		rb.restype = rb2.restype = RTSHORT;
		rb.resval.rint = 1; rb2.resval.rint = 2;
		sds_trans(p1, &rb, &rb2, 1, p1);
		SDS_getvar(NULL, DB_QVIEWSIZE, &rb, flp, &SDS_CURCFG, &SDS_CURSES);
		fr1 = rb.resval.rreal;
		SDS_getvar(NULL,DB_QSCREENSIZE, &rb, flp, &SDS_CURCFG, &SDS_CURSES);
		fr2 =(double)rb.resval.rpoint[0] / (double)rb.resval.rpoint[1];
		PixelX =(int)rb.resval.rpoint[0];
		PixelY =(int)rb.resval.rpoint[1];
		SDS_getvar(NULL,DB_QVIEWDIR, &rb,flp, &SDS_CURCFG, &SDS_CURSES);
		ic_ptcpy(p6, rb.resval.rpoint);
	} 
	else 
	{
		pView->m_pVportTabHip->get_12(p1);
		pView->m_pVportTabHip->get_40(&fr1);
		fr2 = (double)pView->m_iWinX / (double)pView->m_iWinY;
		PixelX =pView->m_iWinX;
		PixelY= pView->m_iWinY;
		pView->m_pVportTabHip->get_16(p6);
		struct resbuf rb2;
		rb.restype = rb2.restype = RTSHORT;
		rb.resval.rint = 0;rb2.resval.rint = 1;
		sds_trans(p6, &rb, &rb2, 1, p6);
	}
	//p6 is viewdir.  Let p5[0] be sine of angle that viewdir makes w/XY plane
	p5[0] = (p6[2] / sqrt(p6[0] * p6[0] + p6[1] * p6[1] + p6[2] * p6[2]));

	//Now check to see if the grid is too dense to display...
	if(fabs(p5[0]) < sin(IC_PI / 180) || 
		fabs(fr1 / (p5[0] * gridyunit)) > double(PixelY * griddensity) ||
		fabs(fr1 * fr2 / (p5[0] * gridxunit)) > double(PixelX * griddensity))
	{
		cmd_ErrorPrompt(CMD_ERR_GRIDTOOSMALL, -1);
		return RTERROR;
	}

	p2[0] = p1[0]+0.5 * fr1 * fr2;
	p2[1] = p1[1] + 0.5 * fr1;
	gr_rp2ucs(p2, p3, view);
	p2[0] = p1[0] - 0.5 * fr1 * fr2;
	p2[1] = p1[1] - 0.5 * fr1;
	gr_rp2ucs(p2, p4, view);
	p2[0] = p1[0] + 0.5 * fr1 * fr2;
	p2[1] = p1[1] - 0.5 * fr1;
	gr_rp2ucs(p2, p5, view);
	p2[0] = p1[0] -0.5 * fr1 * fr2;
	p2[1] = p1[1] +0.5 * fr1;
	gr_rp2ucs(p2, p6, view);
	limptur[0] = max(p4[0], p3[0]);
	limptur[0] = max(limptur[0], p5[0]);
	limptur[0] = max(limptur[0], p6[0]);
	limptur[1] = max(p4[1], p3[1]);
	limptur[1] = max(limptur[1], p5[1]);
	limptur[1] = max(limptur[1], p6[1]);
	limptll[0] = min(p4[0], p3[0]);
	limptll[0] = min(limptll[0], p5[0]);
	limptll[0] = min(limptll[0], p6[0]);
	limptll[1] = min(p4[1], p3[1]);
	limptll[1] = min(limptll[1], p5[1]);
	limptll[1] = min(limptll[1], p6[1]);
	limptll[2] = 0.0;
	limptur[2] = 0.0;

	//Now, our limits are all expressed	in UCS coordinates.....
	//Check to see if we're in WCS coordinates, in which case the
	//	limits of the drawing define a second boundary which may
	//	at some pts be within screen
	SDS_getvar(NULL, DB_QUCSORG, &rb, flp, &SDS_CURCFG, &SDS_CURSES);
	if(icadRealEqual(rb.resval.rpoint[0],0.0) && icadRealEqual(rb.resval.rpoint[1],0.0) && 
		icadRealEqual(rb.resval.rpoint[2],0.0))
	{
		SDS_getvar(NULL, DB_QUCSXDIR, &rb, flp, &SDS_CURCFG, &SDS_CURSES);
		if(icadRealEqual(rb.resval.rpoint[0],1.0) && 
			icadRealEqual(rb.resval.rpoint[1],0.0) && 
			icadRealEqual(rb.resval.rpoint[2],0.0))
		{
			SDS_getvar(NULL, DB_QUCSYDIR, &rb, flp, &SDS_CURCFG, &SDS_CURSES);
			if(icadRealEqual(rb.resval.rpoint[0],0.0) && 
				icadRealEqual(rb.resval.rpoint[1],1.0) && 
				icadRealEqual(rb.resval.rpoint[2],0.0))
			{
				//at this point, we're using WCS coordinates, so
				//we'll apply the grid to at most the dwg limits
				SDS_getvar(NULL, DB_QLIMMIN, &rb, flp, &SDS_CURCFG, &SDS_CURSES);
				ic_ptcpy(p1, rb.resval.rpoint);
				SDS_getvar(NULL, DB_QLIMMAX, &rb, flp, &SDS_CURCFG, &SDS_CURSES);
				ic_ptcpy(p2, rb.resval.rpoint);
				//check to see if any of limits is within screen
				//or can we just exit now
				if(p1[0] >= limptur[0] || p1[1] >= limptur[1] || 
					p2[0] <= limptll[0] || p2[1] <= limptll[1])
					return RTNORM;
				if(p1[1] > limptll[1])
					limptll[1] = p1[1];
				if(p1[0] > limptll[0])
					limptll[0] = p1[0];
				if(p2[1] < limptur[1])
					limptur[1] = p2[1];
				if(p2[0] < limptur[0])
					limptur[0] = p2[0];
			}
		}
	}

	//Now we want to draw the grid between limpur & limptll,
	//drawing based upon snapbase , snapang & spacing for X & Y.

	//Now, we're going to setup a new coordinate system w/origin @SNAPORG
	//and an X axis along the snap angle (Z unchanged, thus we get Y).
	
	//Take rectangle w/corners limptll & limptur and project into new coords
	//to find min & max for rectangular bdry in that system.

	if(tilemode == 0) 
	{
		SDS_getvar(NULL, DB_QSNAPBASE, &rb, flp, &SDS_CURCFG, &SDS_CURSES);
		ic_ptcpy(snapbase, rb.resval.rpoint);
	} 
	else 
	{
		pView->m_pVportTabHip->get_13(snapbase);
	}

	//map limptll to p3
	p3[0] = (limptll[0] - snapbase[0]) * angcos + (limptll[1] - snapbase[1]) * angsin;
	p3[1] = (snapbase[0] - limptll[0]) * angsin + (limptll[1] - snapbase[1]) * angcos;
	
	//map limptur to p4
	p4[0] = (limptur[0] - snapbase[0]) * angcos + (limptur[1] - snapbase[1]) * angsin;
	p4[1] = (snapbase[0] - limptur[0]) * angsin + (limptur[1] - snapbase[1]) * angcos;

	//we'll use p1 & p2 as outer RECTANGULAR limits of limit corner projections
	p1[0] = min(p3[0], p4[0]);
	p1[1] = min(p3[1], p4[1]);
	p2[0] = max(p3[0], p4[0]);
	p2[1] = max(p3[1], p4[1]);

	//check upper left pt of limits rectangle
	ic_ptcpy(p5, limptll);
	p5[1] = limptur[1];
	//map p5 to p3
	p3[0] = (p5[0] - snapbase[0]) * angcos + (p5[1] - snapbase[1]) * angsin;
	p3[1] = (snapbase[0] - p5[0]) * angsin + (p5[1] - snapbase[1]) * angcos;
	p1[0] = min(p1[0], p3[0]);
	p1[1] = min(p1[1], p3[1]);
	p2[0] = max(p2[0], p3[0]);
	p2[1] = max(p2[1], p3[1]);

	//check lower right pt of limits rectangle
	ic_ptcpy(p5, limptll);
	p5[0] = limptur[0];
	//map p5 to p3
	p3[0] = (p5[0] - snapbase[0]) * angcos + (p5[1] - snapbase[1]) * angsin;
	p3[1] = (snapbase[0] - p5[0]) * angsin + (p5[1] - snapbase[1]) * angcos;
	p1[0] = min(p1[0], p3[0]);
	p1[1] = min(p1[1], p3[1]);
	p2[0] = max(p2[0], p3[0]);
	p2[1] = max(p2[1], p3[1]);

	//p1 & p2 now are in our coordinate system and are the ll and ur corners
	//of the rectangle.  Snapbase is the origin of the coord system.  
	//Adjust p1 & p2 so they lie right on snap points
	modf(p1[0] / gridxunit, &fr1);
	p1[0] = fr1 * gridxunit;
	modf(p2[0] / gridxunit, &fr1);
	p2[0] = fr1 * gridxunit;
	modf(p1[1]/gridyunit, &fr1);
	p1[1] = fr1 * gridyunit;
	if(icadRealEqual(fmod(fr1, 2.0),0.0)) //find out if we're starting w/even or odd # row
		flag = 0;
	else
		flag = 1;
	modf(p2[1] / gridyunit, &fr1);
	p2[1]=fr1*gridyunit;
	
	//Now, we can loop thru the points.  At each one, we'll convert it back into
	//UCS coordinates and verify that it's in the range we want to display
	SDS_getvar(NULL, DB_QELEVATION, &rb, flp, &SDS_CURCFG, &SDS_CURSES);
	p4[2] = rb.resval.rreal;
	ic_ptcpy(p3, p1);
	
	// Now Draw the grid on all the views
	int pix[2];
	COLORREF color = SDS_PenColorFromACADColor(-1);
	CDC *Memdc = pView->GetFrameBufferCDC();
	CDC* pDC = pView->GetDC();
	CPalette* pPrevPalette = pDC->SelectPalette(SDS_CMainWindow->m_pPalette, TRUE);
	if(pView->m_pClipRectDC)
		pDC->IntersectClipRect(pView->m_pClipRectDC);

	for(;p3[1] <= p2[1]; p3[1] += gridyunit)
	{
		if(snapstyl && flag)//offset every alternate X row by 1/2 the x spacing
			p3[0] -= 0.5 * gridxunit;
		flag = (!flag);
		for(;p3[0] <= p2[0]; p3[0] += gridxunit)
		{
			p4[0] = p3[0] * angcos - p3[1] * angsin + snapbase[0];
			p4[1] = p3[0] * angsin + p3[1] * angcos + snapbase[1];
			if((p4[0] >= limptll[0]) && (p4[0] <= limptur[0]) &&
			   (p4[1] >= limptll[1]) && (p4[1] <= limptur[1]))
			{
				//Draw the pt!
				gr_ucs2rp(p4, p4, view);
				gr_rp2pix(view, p4[0], p4[1], &pix[0], &pix[1]);
				
				pDC->SetPixel(pix[0], pix[1], color);
				Memdc->SetPixel(pix[0], pix[1], color);	
				toobig = 0;
			}
		}
		p3[0] = p1[0];
	}
	//pDC->SelectPalette(pPrevPalette,TRUE);
	pView->ReleaseDC(pDC);		

	if(toobig)
	{
		cmd_ErrorPrompt(CMD_ERR_GRIDTOOLARGE,-1);
		return(RTERROR);
	}
	else 
		return(RTNORM);
#else
	if(pView == NULL)
		pView = SDS_CURVIEW;

	ASSERT(pView != NULL);
	ASSERT(pView->m_pViewsDoc != NULL);
	ASSERT(pView->m_pViewsDoc->m_dbDrawing != NULL);

	CIcadEntityRenderer renderer(pView->m_pViewsDoc->m_dbDrawing, pView);
	return renderer.drawGrid();
#endif
}

int SDS_DrawBlip(const sds_point pt) 
{
    struct resbuf rb;
	SDS_getvar(NULL, DB_QBLIPMODE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
    if(!rb.resval.rint) 
		return RTERROR;

	SDS_getvar(NULL, DB_QVIEWSIZE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
    sds_real size = rb.resval.rreal;
	SDS_getvar(NULL, DB_QSCREENSIZE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	// Change this 4.625 to change the size of the blips.
    size = (size / rb.resval.rreal) * 4.625; // was 5.0 -- changed 6/15/98

    sds_point pt1, pt2, cp;
	gr_ucs2rp((double *)pt, cp, SDS_CURGRVW);

    pt1[0] = cp[0]; 
	pt1[1] = cp[1] + size;
	pt1[2] = 0.0;
    pt2[0] = cp[0]; 
	pt2[1] = cp[1] - size * 1.333;  // added the *1.333 to balance the cross on the center
	pt2[2] = 0.0;  
	gr_rp2ucs(pt1, pt1, SDS_CURGRVW);
	gr_rp2ucs(pt2, pt2, SDS_CURGRVW);
    SDS_DrawLine(pt1, pt2, 7, 0, 0, SDS_CURDWG, 0); // DP ???: change last to 1 and remove blitting

    pt1[0] = cp[0] + size * 1.333;
	pt1[1] = cp[1];	
	pt1[2] = 0.0; // same here
    pt2[0] = cp[0] - size;
	pt2[1] = cp[1];
	pt2[2] = 0.0;
	gr_rp2ucs(pt1, pt1, SDS_CURGRVW);
	gr_rp2ucs(pt2, pt2, SDS_CURGRVW);
    SDS_DrawLine(pt1, pt2, 7, 0, 0, SDS_CURDWG, 0); // DP ???: change last to 1 and remove blitting

	SDS_BitBlt2Scr(1);  // DP ???: change last to 1 and remove blitting
    return RTNORM;
}

CGrid::CGrid(db_drawing* pDrawing): m_pDrawing(pDrawing)
{
}

int CGrid::draw(gr_view* pViewData, CDrawDevice* pDrawDevice, CIcadView* pView)
{
	//This f'n draws in the grid using sds_grdraw.  It returns RTNORM
	//if able to display the grid (even if no grid points lie
	//within current view), and RTERROR if the grid is too dense
	//to display.
	//NOTE: If SNAPSTYL == 1, the grid is ALWAYS drawn in isometric fashion,
	//forcing the X distance to isometrically match the Y.  Y is retrieved
	//from GRIDUNIT[1](or SNAPUNIT[1] if GRIDUNIT is (0,0)).  THIS IS DIFFERENT
	//FROM AUTOCAD - acad doesn't force. Regardless of SNAPSTYL, grid is drawn at
	//angle of SNAPANG.

	struct resbuf rb;
	int snapstyl,flag,toobig,tilemode,PixelX,PixelY;
	sds_real gridxunit,gridyunit,griddensity,fr1,fr2,angsin,angcos;
	sds_point limptur,limptll,snapbase,p1,p2,p3,p4,p5,p6;

	if(pView->m_pVportTabHip == NULL)
		return RTERROR;

	if(pView->GetRealTime() != NULL)
		return RTNORM;
	
	// Make sure the corect setvars are in this table entry.
	// Because this function looks at the table entry directly.
//XX	if(pView==SDS_CURVIEW) SDS_VarToTab(m_pDrawing,pView->m_pVportTabHip);

	griddensity=0.33;	//maximum part of screen width or height which
						//may be taken up by pixels for drawing grid
	//NOTE:  Acad uses 0.33, meaning 1/3 of pixels can be used for grid.
	//	We don't want to display too dense a grid

	toobig=1;		//is grid so big that none of the grid points show up?

	SDS_getvar(NULL, DB_QTILEMODE, &rb, m_pDrawing, &SDS_CURCFG, &SDS_CURSES);
	tilemode = rb.resval.rint;

	if(tilemode == 0) 
	{
		SDS_getvar(NULL, DB_QGRIDMODE, &rb, m_pDrawing, &SDS_CURCFG, &SDS_CURSES);
		flag = rb.resval.rint;
	} 
	else
	{
		pView->m_pVportTabHip->get_76(&flag);
	}
	if(flag == 0)
		return RTNORM;

	if(tilemode==0)
	{
		SDS_getvar(NULL, DB_QSNAPSTYL, &rb, m_pDrawing, &SDS_CURCFG, &SDS_CURSES);
		snapstyl = rb.resval.rint;
		SDS_getvar(NULL, DB_QGRIDUNIT, &rb, m_pDrawing, &SDS_CURCFG, &SDS_CURSES);
		ic_ptcpy(p1, rb.resval.rpoint);
	}
	else
	{
		pView->m_pVportTabHip->get_77(&snapstyl);
		pView->m_pVportTabHip->get_15(p1);
	}
	if((icadRealEqual(p1[0],0.0))||(icadRealEqual(p1[1],0.0)))
	{
		if(tilemode == 0)
		{
			SDS_getvar(NULL, DB_QSNAPUNIT,&rb, m_pDrawing, &SDS_CURCFG, &SDS_CURSES);
			ic_ptcpy(p1, rb.resval.rpoint);
		} else {
			pView->m_pVportTabHip->get_14(p1);
		}
		gridxunit = p1[0];
		gridyunit = p1[1];
		if(snapstyl)
			gridyunit *= 0.5;	//don't ask me why!
	}
	else
	{
		gridxunit=p1[0];
		gridyunit=p1[1];
	}

	if(tilemode == 0)
	{
		SDS_getvar(NULL, DB_QSNAPANG, &rb, m_pDrawing, &SDS_CURCFG, &SDS_CURSES);
		fr1 = rb.resval.rreal;
	}
	else 
	{
		pView->m_pVportTabHip->get_50(&fr1);
	}
	angsin = sin(fr1);
	angcos = cos(fr1);

	//if snap style is isometric, adjust the grid so that it draws for isometric
	//projection - but don't reset any of the variables
	if(snapstyl && fabs((gridyunit * 2.0 * 1.732050807568877 / gridxunit) - 1.0) > 0.00001)
	{
		gridxunit = gridyunit * 2.0 * 1.732050807568877;
	}

	//Get viewctr & convert from ucs to rp to calculate corners of
	//screen.  Then project corners back from rp to UCS.  
	//pView->m_pVportTabHip->get_12(p2);	//I think we changed this so it's in RP, so next line not needed
	//gr_ucs2rp(p2,p1,pViewData);
	if(tilemode ==0 ) 
	{
		SDS_getvar(NULL, DB_QVIEWCTR, &rb, m_pDrawing, &SDS_CURCFG, &SDS_CURSES);
		ic_ptcpy(p1, rb.resval.rpoint);
		struct resbuf rb2;
		rb.restype = rb2.restype = RTSHORT;
		rb.resval.rint = 1; rb2.resval.rint = 2;
		sds_trans(p1, &rb, &rb2, 1, p1);
		SDS_getvar(NULL, DB_QVIEWSIZE, &rb, m_pDrawing, &SDS_CURCFG, &SDS_CURSES);
		fr1 = rb.resval.rreal;
		SDS_getvar(NULL,DB_QSCREENSIZE, &rb, m_pDrawing, &SDS_CURCFG, &SDS_CURSES);
		fr2 =(double)rb.resval.rpoint[0] / (double)rb.resval.rpoint[1];
		PixelX =(int)rb.resval.rpoint[0];
		PixelY =(int)rb.resval.rpoint[1];
		SDS_getvar(NULL,DB_QVIEWDIR, &rb,m_pDrawing, &SDS_CURCFG, &SDS_CURSES);
		ic_ptcpy(p6, rb.resval.rpoint);
	} 
	else 
	{
		pView->m_pVportTabHip->get_12(p1);
		pView->m_pVportTabHip->get_40(&fr1);
		fr2 = (double)pView->m_iWinX / (double)pView->m_iWinY;
		PixelX =pView->m_iWinX;
		PixelY= pView->m_iWinY;
		pView->m_pVportTabHip->get_16(p6);
		struct resbuf rb2;
		rb.restype = rb2.restype = RTSHORT;
		rb.resval.rint = 0;rb2.resval.rint = 1;
		sds_trans(p6, &rb, &rb2, 1, p6);
	}
	//p6 is viewdir.  Let p5[0] be sine of angle that viewdir makes w/XY plane
	p5[0] = (p6[2] / sqrt(p6[0] * p6[0] + p6[1] * p6[1] + p6[2] * p6[2]));

	//Now check to see if the grid is too dense to display...
	if(fabs(p5[0]) < sin(IC_PI / 180) || 
		fabs(fr1 / (p5[0] * gridyunit)) > double(PixelY * griddensity) ||
		fabs(fr1 * fr2 / (p5[0] * gridxunit)) > double(PixelX * griddensity))
	{
		cmd_ErrorPrompt(CMD_ERR_GRIDTOOSMALL, -1);
		return RTERROR;
	}

	p2[0] = p1[0]+0.5 * fr1 * fr2;
	p2[1] = p1[1] + 0.5 * fr1;
	gr_rp2ucs(p2, p3, pViewData);
	p2[0] = p1[0] - 0.5 * fr1 * fr2;
	p2[1] = p1[1] - 0.5 * fr1;
	gr_rp2ucs(p2, p4, pViewData);
	p2[0] = p1[0] + 0.5 * fr1 * fr2;
	p2[1] = p1[1] - 0.5 * fr1;
	gr_rp2ucs(p2, p5, pViewData);
	p2[0] = p1[0] -0.5 * fr1 * fr2;
	p2[1] = p1[1] +0.5 * fr1;
	gr_rp2ucs(p2, p6, pViewData);
	limptur[0] = max(p4[0], p3[0]);
	limptur[0] = max(limptur[0], p5[0]);
	limptur[0] = max(limptur[0], p6[0]);
	limptur[1] = max(p4[1], p3[1]);
	limptur[1] = max(limptur[1], p5[1]);
	limptur[1] = max(limptur[1], p6[1]);
	limptll[0] = min(p4[0], p3[0]);
	limptll[0] = min(limptll[0], p5[0]);
	limptll[0] = min(limptll[0], p6[0]);
	limptll[1] = min(p4[1], p3[1]);
	limptll[1] = min(limptll[1], p5[1]);
	limptll[1] = min(limptll[1], p6[1]);
	limptll[2] = 0.0;
	limptur[2] = 0.0;

	//Now, our limits are all expressed	in UCS coordinates.....
	//Check to see if we're in WCS coordinates, in which case the
	//	limits of the drawing define a second boundary which may
	//	at some pts be within screen
	SDS_getvar(NULL, DB_QUCSORG, &rb, m_pDrawing, &SDS_CURCFG, &SDS_CURSES);
	if(icadRealEqual(rb.resval.rpoint[0],0.0) && 
		icadRealEqual(rb.resval.rpoint[1],0.0) && 
		icadRealEqual(rb.resval.rpoint[2],0.0))
	{
		SDS_getvar(NULL, DB_QUCSXDIR, &rb, m_pDrawing, &SDS_CURCFG, &SDS_CURSES);
		if(icadRealEqual(rb.resval.rpoint[0],1.0) && 
			icadRealEqual(rb.resval.rpoint[1],0.0) && 
			icadRealEqual(rb.resval.rpoint[2],0.0))
		{
			SDS_getvar(NULL, DB_QUCSYDIR, &rb, m_pDrawing, &SDS_CURCFG, &SDS_CURSES);
			if(icadRealEqual(rb.resval.rpoint[0],0.0) && 
				icadRealEqual(rb.resval.rpoint[1],1.0) && 
				icadRealEqual(rb.resval.rpoint[2],0.0))
			{
				//at this point, we're using WCS coordinates, so
				//we'll apply the grid to at most the dwg limits
				SDS_getvar(NULL, DB_QLIMMIN, &rb, m_pDrawing, &SDS_CURCFG, &SDS_CURSES);
				ic_ptcpy(p1, rb.resval.rpoint);
				SDS_getvar(NULL, DB_QLIMMAX, &rb, m_pDrawing, &SDS_CURCFG, &SDS_CURSES);
				ic_ptcpy(p2, rb.resval.rpoint);
				//check to see if any of limits is within screen
				//or can we just exit now
				if(p1[0] >= limptur[0] || p1[1] >= limptur[1] || 
					p2[0] <= limptll[0] || p2[1] <= limptll[1])
					return RTNORM;
				if(p1[1] > limptll[1])
					limptll[1] = p1[1];
				if(p1[0] > limptll[0])
					limptll[0] = p1[0];
				if(p2[1] < limptur[1])
					limptur[1] = p2[1];
				if(p2[0] < limptur[0])
					limptur[0] = p2[0];
			}
		}
	}

	//Now we want to draw the grid between limpur & limptll,
	//drawing based upon snapbase , snapang & spacing for X & Y.

	//Now, we're going to setup a new coordinate system w/origin @SNAPORG
	//and an X axis along the snap angle (Z unchanged, thus we get Y).
	
	//Take rectangle w/corners limptll & limptur and project into new coords
	//to find min & max for rectangular bdry in that system.

	if(tilemode == 0) 
	{
		SDS_getvar(NULL, DB_QSNAPBASE, &rb, m_pDrawing, &SDS_CURCFG, &SDS_CURSES);
		ic_ptcpy(snapbase, rb.resval.rpoint);
	} 
	else 
	{
		pView->m_pVportTabHip->get_13(snapbase);
	}

	//map limptll to p3
	p3[0] = (limptll[0] - snapbase[0]) * angcos + (limptll[1] - snapbase[1]) * angsin;
	p3[1] = (snapbase[0] - limptll[0]) * angsin + (limptll[1] - snapbase[1]) * angcos;
	
	//map limptur to p4
	p4[0] = (limptur[0] - snapbase[0]) * angcos + (limptur[1] - snapbase[1]) * angsin;
	p4[1] = (snapbase[0] - limptur[0]) * angsin + (limptur[1] - snapbase[1]) * angcos;

	//we'll use p1 & p2 as outer RECTANGULAR limits of limit corner projections
	p1[0] = min(p3[0], p4[0]);
	p1[1] = min(p3[1], p4[1]);
	p2[0] = max(p3[0], p4[0]);
	p2[1] = max(p3[1], p4[1]);

	//check upper left pt of limits rectangle
	ic_ptcpy(p5, limptll);
	p5[1] = limptur[1];
	//map p5 to p3
	p3[0] = (p5[0] - snapbase[0]) * angcos + (p5[1] - snapbase[1]) * angsin;
	p3[1] = (snapbase[0] - p5[0]) * angsin + (p5[1] - snapbase[1]) * angcos;
	p1[0] = min(p1[0], p3[0]);
	p1[1] = min(p1[1], p3[1]);
	p2[0] = max(p2[0], p3[0]);
	p2[1] = max(p2[1], p3[1]);

	//check lower right pt of limits rectangle
	ic_ptcpy(p5, limptll);
	p5[0] = limptur[0];
	//map p5 to p3
	p3[0] = (p5[0] - snapbase[0]) * angcos + (p5[1] - snapbase[1]) * angsin;
	p3[1] = (snapbase[0] - p5[0]) * angsin + (p5[1] - snapbase[1]) * angcos;
	p1[0] = min(p1[0], p3[0]);
	p1[1] = min(p1[1], p3[1]);
	p2[0] = max(p2[0], p3[0]);
	p2[1] = max(p2[1], p3[1]);

	//p1 & p2 now are in our coordinate system and are the ll and ur corners
	//of the rectangle.  Snapbase is the origin of the coord system.  
	//Adjust p1 & p2 so they lie right on snap points
	modf(p1[0] / gridxunit, &fr1);
	p1[0] = fr1 * gridxunit;
	modf(p2[0] / gridxunit, &fr1);
	p2[0] = fr1 * gridxunit;
	modf(p1[1]/gridyunit, &fr1);
	p1[1] = fr1 * gridyunit;
	if(icadRealEqual(fmod(fr1, 2.0),0.0)) //find out if we're starting w/even or odd # row
		flag = 0;
	else
		flag = 1;
	modf(p2[1] / gridyunit, &fr1);
	p2[1]=fr1*gridyunit;
	
	//Now, we can loop thru the points.  At each one, we'll convert it back into
	//UCS coordinates and verify that it's in the range we want to display
	SDS_getvar(NULL, DB_QELEVATION, &rb, m_pDrawing, &SDS_CURCFG, &SDS_CURSES);
	p4[2] = rb.resval.rreal;
	ic_ptcpy(p3, p1);
	
	// Now Draw the grid on all the views
	int pix[2];
	COLORREF color = SDS_PenColorFromACADColor(-1);
	for(;p3[1] <= p2[1]; p3[1] += gridyunit)
	{
		if(snapstyl && flag)//offset every alternate X row by 1/2 the x spacing
			p3[0] -= 0.5 * gridxunit;
		flag = (!flag);
		for(;p3[0] <= p2[0]; p3[0] += gridxunit)
		{
			p4[0] = p3[0] * angcos - p3[1] * angsin + snapbase[0];
			p4[1] = p3[0] * angsin + p3[1] * angcos + snapbase[1];
			if((p4[0] >= limptll[0]) && (p4[0] <= limptur[0]) &&
			   (p4[1] >= limptll[1]) && (p4[1] <= limptur[1]))
			{
				//Draw the pt!
				gr_ucs2rp(p4, p4, pViewData);
				gr_rp2pix(pViewData, p4[0], p4[1], &pix[0], &pix[1]);
				
				pDrawDevice->SetPixel(pix[0], pix[1], color);
				toobig = 0;
			}
		}
		p3[0] = p1[0];
	}

	if(toobig)
	{
		cmd_ErrorPrompt(CMD_ERR_GRIDTOOLARGE,-1);
		return(RTERROR);
	}
	else 
		return(RTNORM);
}
