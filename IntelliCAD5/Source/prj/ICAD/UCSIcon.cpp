#include "icad.h"
#include "UCSIcon.h"
#include "IcadView.h"
#include "IcadDoc.h"
#include "DrawDevice.h"
#include "cmds.h"
#include "IcadEntityRenderer.h"

static int 
SDS_UcsGrDraw(CDrawDevice* pBufferDrawDevice, sds_point frompt, sds_point topt, int color, sds_point basept,
				   sds_point tempxdir, sds_point tempydir,CIcadView *pView) {
	//this f'n is just like sds_grdraw except basept is added
	//to both frompt and topt.
	//if tempxdir!=NULL, we are assuming to draw in the temporary coordinate
	//system defined by tempxdir & tempydir.
	sds_point p1,p2;
	int pix[4],ret;

	struct gr_view *view=pView->m_pCurDwgView;
	if(view==NULL) 
		return(RTERROR);
	if (pView->IsPrinting())
		return RTERROR;

	if(basept==NULL) {
		ic_ptcpy(p1,frompt);
		ic_ptcpy(p2,topt);
	} else {
		if(tempxdir==NULL) {
			p1[0]=frompt[0]+basept[0];
			p1[1]=frompt[1]+basept[1];
			p1[2]=frompt[2]+basept[2];
			p2[0]=topt[0]+basept[0];
			p2[1]=topt[1]+basept[1];
			p2[2]=topt[2]+basept[2];
		} else {
			p1[0]=frompt[0]*tempxdir[0]+frompt[1]*tempxdir[1]+frompt[2]*tempxdir[2];
			p1[1]=frompt[0]*tempydir[0]+frompt[1]*tempydir[1]+frompt[2]*tempydir[2];
			p2[0]=topt[0]*tempxdir[0]+topt[1]*tempxdir[1]+topt[2]*tempxdir[2];
			p2[1]=topt[0]*tempydir[0]+topt[1]*tempydir[1]+topt[2]*tempydir[2];
			p1[0]+=basept[0];
			p1[1]+=basept[1];
			p2[0]+=basept[0];
			p2[1]+=basept[1];
			gr_rp2ucs(p1,p1,view);
			gr_rp2ucs(p2,p2,view);
		}
	}

	if(pBufferDrawDevice->is3D())
	{
		sds_point segment[2];
		memcpy(segment[0], p1, sizeof(sds_point));
		memcpy(segment[1], p2, sizeof(sds_point));
		pBufferDrawDevice->SetLineColorACAD(color, 1);
		pBufferDrawDevice->Polyline((POINT*)segment, 2);
		return RTNORM;
	}

	gr_ucs2rp(p1,p1,view);
	gr_ucs2rp(p2,p2,view);
	ret=gr_vect_rp2pix(p1,p2,pix,view);

	if(ret==0 || ret==2) 
		{
		POINT pixpt[2];
		pixpt[0].x=pix[0];
		pixpt[0].y=pix[1];
		pixpt[1].x=pix[2];
		pixpt[1].y=pix[3];


		// This code could be faster than drawing into off-screen and always blitting
		// but it doesn't get colors right...
		//	Don't delete -- I'll try and fix it 
		//
//		pScreenDrawDevice->SetDrawMode( RasterConstants::MODE_COPY );
//		pScreenDrawDevice->SetLineColorACAD( color );
//		pScreenDrawDevice->Polyline( pixpt, 2 );

		pBufferDrawDevice->SetDrawMode(RasterConstants::MODE_COPY);
		pBufferDrawDevice->SetLineColorACAD(color, 1);
		pBufferDrawDevice->Polyline(pixpt, 2);
		}

	return(RTNORM);
}

static int 
SDS_DrawAltUcsIcon(CDrawDevice* pBufferDrawDevice, const sds_real *scale, const sds_point rpadd, const sds_point viewdir,CIcadView *pView,int unpaint) {
	
	sds_point center;
	struct gr_view *view=pView->m_pCurDwgView;
	struct resbuf rb;
	if(view==NULL) return(RTERROR);

	if(viewdir==NULL) return(RTERROR);
	if(icadRealEqual(viewdir[2],0.0)) return(RTNORM);

	if(pView->m_pViewsDoc==NULL) return(RTNORM);
	db_drawing *flp=pView->m_pViewsDoc->m_dbDrawing;
	if(flp==NULL) return(RTNORM);

	if(pView->m_pVportTabHip==NULL) return(RTERROR);
	SDS_getvar(NULL,DB_QTILEMODE,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
	if(!rb.resval.rint) {
		pView->m_pVportTabHip->get_12(center);	//I think this is rp in this case, so convert to ucs
		gr_rp2ucs(center,center,view);
	} else {
		SDS_getvar(NULL,DB_QVIEWCTR,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(center,rb.resval.rpoint);
	}

	//rpadd is X & Y to offset pt in REAL PROJECTION COORDINATES!
	sds_real pts[]={0.1495, 0.2592, 0.8609, 0.2114,
					0.4852, 0.4958, 0.9930, 0.4617,
					0.7457, 0.6794, 1.0956, 0.6559,
					0.9046, 0.7914, 1.1581, 0.7744,
					0.9990, 0.8602, 1.1964, 0.8470,
					0.0059, 0.1580, 1.1303, 0.9505,
					1.2388, 0.9273, 0.7942, 0.0849,
					1.0785, 0.9140, 1.2265, 0.9040,
					1.2337, 0.9246, 0.7520, 0.0890,
					0.0302, 0.1231, 1.1314, 0.9450};

	sds_point p1,p2,p3,p4;
	int ret,fi1;
	int textcolor;
	
	if(0==unpaint)
		textcolor=7;
	else
		textcolor=256;

	p1[2]=p2[2]=0.0;
	for(fi1=0;fi1<10;fi1++){
		p1[0]=pts[(4*fi1)]*(*scale)+rpadd[0];
		p1[1]=pts[(4*fi1+1)]*(*scale)+rpadd[1];
		p2[0]=pts[(4*fi1+2)]*(*scale)+rpadd[0];
		p2[1]=pts[(4*fi1+3)]*(*scale)+rpadd[1];
		gr_rp2ucs(p1,p3,view);
		gr_rp2ucs(p2,p4,view);
		if((ret=SDS_UcsGrDraw(pBufferDrawDevice, p3,p4,textcolor,NULL,NULL,NULL,pView))!=RTNORM) return(ret);
	}
	return(RTNORM);
}


int SDS_DrawUcsIcon(sds_point vdir, const sds_real *sclfact,CIcadView *pView,int unpaint) 
{
#ifdef _SDS_UCSICON_
	//This f'n draws the ucs icon as viewed from NEGATIVE vdir(remember,
	//	viewdir always goes away from viewctr, we're looking toward it).
	//If vdir==NULL, current setting of VIEWDIR var is used
	//If sclfact==NULL, then 1 is used
	//If vdir!=NULL, icon is always drawn in center of screen.
	//If unpaint!=0, icon will be painted OUT!!!
	//NOTE:  if we're in paper space AND vdir==NULL, f'n draws the paper space icon
	int ret=RTNORM;
	int omitlet,omitplane,vmode,xcolor,ycolor,zcolor,axiscolor,pspace,
		a,b,c,fi1,tilemode=1;
	sds_resbuf rb,rb2;
	sds_real fr1,fr2,scale,iconsize,leglen,textsize,*frp1;
	sds_point viewdir,basept,p1,p2,p3,p4,tempxdir,tempydir;

	if(pView==NULL) pView=SDS_CURVIEW;
	if(pView==NULL) return(RTERROR);
	struct gr_view *view=pView->m_pCurDwgView;
	if(view==NULL)  return(RTERROR);
	
	if(pView->m_pViewsDoc==NULL) return(RTERROR);
	db_drawing *flp=pView->m_pViewsDoc->m_dbDrawing;
	if(flp==NULL) return(RTERROR);


	if(pView->m_pVportTabHip==NULL) return(RTERROR);
	
	// Make sure the corect setvars are in this table entry.
	// Because this function looks at the table entry directly.
//XX	if(pView==SDS_CURVIEW) SDS_VarToTab(SDS_CURDWG,pView->m_pVportTabHip);

	SDS_getvar(NULL,DB_QTILEMODE,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
	tilemode=rb.resval.rint;

	if(tilemode) {
		pView->m_pVportTabHip->get_74(&a);
	} else {
		SDS_getvar(NULL,DB_QUCSICON,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
		a=rb.resval.rint;
	}
	if((a&1)==0 && vdir==NULL) 
		{
		return(SDS_RTNONE);
		}
	
	if(sclfact==NULL)scale=1.0;
	else scale=*sclfact;
	if(scale<=0.0) return(RTERROR);

	iconsize=0.035;	//how big icon is relative to size of screen
	leglen=2.5;		//axis leg length relative to cube length (NOTE: if 
					//we're in pspace, this will get adjusted
	textsize=0.65;	//text ht relative to cube length
	
	pspace=0;
	//Check to see if we're in paper space....
	if(!tilemode) {
		SDS_getvar(NULL,DB_QCVPORT,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
		//NOTE if in pspace, ICAD will return correct UCSXDIR & YDIR for pspace
		if(1==rb.resval.rint)pspace=1;
	}

	if(tilemode) {
		pView->m_pVportTabHip->get_71(&vmode);
	} else {
		SDS_getvar(NULL,DB_QVIEWMODE,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
		vmode=rb.resval.rint;
	}

	if(0==(vmode&1)){	//don't need colors if we draw perspective icon
		if(unpaint){
			xcolor=ycolor=zcolor=axiscolor=256;
		}else{
			SDS_getvar(NULL,DB_QCOLORX,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			xcolor=rb.resval.rint;
			SDS_getvar(NULL,DB_QCOLORY,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			ycolor=rb.resval.rint;
			SDS_getvar(NULL,DB_QCOLORZ,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			zcolor=rb.resval.rint;
			axiscolor=-1;
		}
	}

	// ************************
	// DrawDevices
	//
	CDrawDevice* pBufferDrawDevice = pView->GetDrawDevice();

	if(vdir==NULL){
		if(tilemode) {
			pView->m_pVportTabHip->get_16(viewdir);
			rb.restype=rb2.restype=RTSHORT;
			rb.resval.rint=0;
			rb2.resval.rint=1;
			sds_trans(viewdir,&rb,&rb2,1,viewdir);
		} else {
			SDS_getvar(NULL,DB_QVIEWDIR,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			ic_ptcpy(viewdir,rb.resval.rpoint);
		}
		frp1=NULL;
	}else{
		ic_ptcpy(viewdir,vdir);
		//now we'll have to generate two temporary axes to use when viewing
		//the desired points.  
		tempxdir[0]=-1.0*vdir[1];
		tempxdir[1]=vdir[0];
		tempxdir[2]=0.0;
		fr1=sqrt(tempxdir[0]*tempxdir[0]+tempxdir[1]+tempxdir[1]);
		frp1=tempxdir;
		if(fr1>IC_ZRO){
			tempxdir[0]*=fr1;
			tempxdir[1]*=fr1;
			tempxdir[2]=0.0;
			tempydir[0]=-tempxdir[1]*vdir[2];
			tempydir[1]=tempxdir[0]*vdir[2];
			tempydir[2]=vdir[0]*tempxdir[1]-vdir[1]*tempxdir[0];
		}else{
			if(vdir[2]>0.0){
				tempxdir[0]=tempydir[1]=1.0;
				tempxdir[1]=tempxdir[2]=tempydir[0]=tempydir[2]=0.0;
			}else{
				tempxdir[0]=-1.0;
				tempydir[1]=1.0;
				tempxdir[1]=tempxdir[2]=tempydir[0]=tempydir[2]=0.0;
			}
		}
	}
	
	//get magnitude of viewdir & "unitize" the vector
	fr1=sqrt(viewdir[0]*viewdir[0]+viewdir[1]*viewdir[1]+viewdir[2]*viewdir[2]);
	//keep the magnitude, because it's used to calc viewsize in perspective views
	if(fr1<IC_ZRO) { ret=RTERROR; goto out; }
	viewdir[0]/=fr1;
	viewdir[1]/=fr1;
	viewdir[2]/=fr1;

	//First, we have to calc where to put the icon on the screen.
	if(tilemode) {
		if(vmode&1){
			pView->m_pVportTabHip->get_17(p1);//target
		}else{
			pView->m_pVportTabHip->get_12(p1);//viewctr
			pView->m_pVportTabHip->get_40(&fr1);
		}
	} else {
		if(vmode&1){
			SDS_getvar(NULL,DB_QTARGET,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			ic_ptcpy(p1,rb.resval.rpoint);			
		}else{
			SDS_getvar(NULL,DB_QVIEWCTR,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			ic_ptcpy(p1,rb.resval.rpoint);
			SDS_getvar(NULL,DB_QVIEWSIZE,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			fr1=rb.resval.rreal;
		}
	}
	if(vmode&1){
		//viewsize is meaningless, but we still have fr1 as camera-target distance
		if(tilemode)pView->m_pVportTabHip->get_42(&fr2);
		else{
			SDS_getvar(NULL,DB_QLENSLENGTH,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			fr2=rb.resval.rreal;
		}
		//fr2 is lens length
		fr2=35.0*fr1/fr2;//fr2 is now width of screen.  we need to get height
		SDS_getvar(NULL,DB_QSCREENSIZE,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
		if(icadRealEqual(rb.resval.rpoint[0],0.0)){ ret=RTERROR; goto out; }
		fr1=fr2*rb.resval.rpoint[1]/rb.resval.rpoint[0];
	}

	//calculate p3 as icon origin in rb coords
	scale=scale*iconsize*fr1;
	if(vdir!=NULL){	   //just draw icon in center of screen
		/*if(tilemode)
			ic_ptcpy(basept,p1);
		else*/
			gr_ucs2rp(p1,basept,view);
	}else{
		//  x/y ratio

		if(tilemode) {
			if(pView->m_iWinY==0) { ret=RTERROR; goto out; }
			fr2=(double)pView->m_iWinX/(double)pView->m_iWinY;  
		} else {
			SDS_getvar(NULL,DB_QCVPORT,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			if(1==rb.resval.rint) {
				if(pView->m_iWinY==0) { ret=RTERROR; goto out; }
				fr2=(double)pView->m_iWinX/(double)pView->m_iWinY;  
			} else {
				double ht,wd;
				pView->m_pVportTabHip->get_41(&ht);
				pView->m_pVportTabHip->get_40(&wd);
				if(icadRealEqual(ht,0.0)) { ret=RTERROR; goto out; }
				fr2=wd/ht;  
//				pView->m_pVportTabHip->get_41(&fr2);
			}
		}
		if(tilemode)
			ic_ptcpy(p2,p1);
		else
			gr_ucs2rp(p1,p2,view);

		//if we're in standard mode & vdir not passed to f'n
		if(tilemode) {
			pView->m_pVportTabHip->get_74(&fi1);
		} else {
			SDS_getvar(NULL,DB_QUCSICON,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			fi1=rb.resval.rint;
		}
		if(3==fi1 && !(vmode&1)){
			//check to see if origin is within proper area
			//so that icon can be displayed at origin
			//check to see if max dist along x axis is within spec
			fi1=1;	//default to we can't display at origin
			p4[0]=0.98*0.5*fr2*fr1;
			p4[1]=0.98*0.5*fr1;
			basept[1]=basept[2]=0.0;
			basept[0]=scale*(leglen+2.0*textsize);
			gr_ucs2rp(basept,p1,view);
			if((fabs(p2[0]-p1[0])<=(p4[0]))&&
			   (fabs(p2[1]-p1[1])<=(p4[1]))){
				//check along y axis
				basept[1]=basept[0];
				basept[0]=0.0;
				gr_ucs2rp(basept,p1,view);
				if((fabs(p2[0]-p1[0])<=(p4[0]))&&
				   (fabs(p2[1]-p1[1])<=(p4[1]))){
					basept[2]=basept[1];
					basept[1]=0.0;
					gr_ucs2rp(basept,p1,view);
					if((fabs(p2[0]-p1[0])<=(p4[0]))&&
					   (fabs(p2[1]-p1[1])<=(p4[1]))){
						basept[2]=0.0;
						gr_ucs2rp(basept,p1,view);
						if((fabs(p2[0]-p1[0])<=(p4[0]))&&
						   (fabs(p2[1]-p1[1])<=(p4[1]))){
							ic_ptcpy(p3,p1);
							fi1=3;//yes, display at origin
						}
					}
				}
			}
		}
		if(fi1!=3 || (vmode&1)){
			//back off of corner distance 2.0 for axes, 1.1 safety factor, 1.414 for 
			//	longest possible diagonal view
			p3[0]=p2[0]+(0.5*fr2*fr1)-(2.5*1.414*scale);
			if(vmode&1)	p3[1]=p2[1]-(0.45*fr1);
			else		p3[1]=p2[1]-(0.5*fr1)+(2.5*1.414*scale);
			p3[2]=0.0;
		}

		//if we're in perspective, use alternate icon
		if(vmode&1){
			scale*=leglen;
			SDS_DrawAltUcsIcon(pBufferDrawDevice, &scale,p3,viewdir,pView,0);
			goto out;
		}else{
			gr_rp2ucs(p3,basept,view);
		}	
	
		//check to see if view matches world coordinates, & draw the W 
		p3[0]=p3[1]=p3[2]=0.0;
		SDS_getvar(NULL,DB_QUCSORG,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
		if(icadPointEqual(rb.resval.rpoint,p3)){
			SDS_getvar(NULL,DB_QUCSXDIR,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			ic_ptcpy(p1,rb.resval.rpoint);
			SDS_getvar(NULL,DB_QUCSYDIR,&rb,flp,&SDS_CURCFG,&SDS_CURSES);
			ic_ptcpy(p2,rb.resval.rpoint);
			p3[0]=1.0;
			p4[0]=p4[2]=0.0;
			p4[1]=1.0;
			if(icadPointEqual(p1,p3)&&icadPointEqual(p2,p4)){
				//if we're looking substantially along XY plane
				if(!pspace && fabs(viewdir[2]<0.30)){
					if(fabs(viewdir[1])>fabs(viewdir[0])){
						a=0;
						b=2;
						c=1;
					}else{
						a=1;
						b=2;
						c=0;
					}
				}else{
					a=0;
					b=1;
					c=2;
				}

				if(!pspace && viewdir[c]>0.0){
					p1[c]=p2[c]=scale;
				}else{
					//in paper space, always draw the W in the z=0 plane
					p1[c]=p2[c]=0.0;
				}
				//draw the W marker
				p1[a]=0.35*scale;
				p1[b]=0.75*scale;
				p2[a]=0.5*scale;
				p2[b]=0.25*scale;
				SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,axiscolor,basept,NULL,NULL,pView);
				p1[a]=0.6*scale;
				p1[b]=0.625*scale;
				SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,axiscolor,basept,NULL,NULL,pView);
				p2[a]=0.7*scale;
				SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,axiscolor,basept,NULL,NULL,pView);
				p1[a]=0.85*scale;
				p1[b]=0.75*scale;
				SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,axiscolor,basept,NULL,NULL,pView);
			}
		}
	}
	//We'll break this down into case statements - one for pspace viewing
	//	and one for each possible octant of mspace viewing & one for 
	//  viewing along each axis	from either direction (only 2 real possibilities)
	p1[0]=p1[1]=p1[2]=p2[0]=p2[1]=p2[2]=0.0;
	if(pspace){
		//draw the edges of the paper in x & Y colors
		p2[1]=2.0*scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,ycolor,basept,frp1,tempydir,pView);
		p2[0]=1.5*scale;
		p2[1]=0.0;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,xcolor,basept,frp1,tempydir,pView);
		p1[0]=1.5*scale;
		p1[1]=1.7*scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,ycolor,basept,frp1,tempydir,pView);
		p1[0]=1.2*scale;
		p1[1]=p2[1]=2.0*scale;
		p2[0]=0.0;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,xcolor,basept,frp1,tempydir,pView);
		//now draw the folded corner of the page
		p2[0]=1.2*scale;
		p2[1]=1.7*scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,axiscolor,basept,frp1,tempydir,pView);
		p1[0]=1.5*scale;
		p1[1]=1.7*scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,axiscolor,basept,frp1,tempydir,pView);
		p2[0]=1.2*scale;
		p2[1]=2.0*scale;
		//draw the fold itself as dashed...
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,axiscolor,basept,frp1,tempydir,pView);
		//draw X axis
		p1[0]=1.5*scale;
		p2[0]=leglen*scale;
		p1[1]=p1[2]=p2[1]=p2[2]=0.0;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,axiscolor,basept,frp1,tempydir,pView);
		//draw Y axis
		p1[1]=2.0*scale;
		p2[1]=leglen*scale;
		p1[0]=p2[0]=0.0;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,axiscolor,basept,frp1,tempydir,pView);
		//draw z axis
		p1[0]=p1[1]=p1[2]=p2[0]=p2[1]=0.0;
		p2[2]=scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,axiscolor,basept,frp1,tempydir,pView);
		//don't draw in the letter Z
		omitlet=4;
		goto letters;
	}else if(fabs(viewdir[1])<IC_ZRO && fabs(viewdir[2])<IC_ZRO){	
		//view along X axis 
		p2[1]=scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,ycolor,basept,frp1,tempydir,pView);
		p1[2]=p2[2]=scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,ycolor,basept,frp1,tempydir,pView);
		p2[1]=p2[2]=0.0;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,zcolor,basept,frp1,tempydir,pView);
		p1[1]=p2[1]=scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,zcolor,basept,frp1,tempydir,pView);
		//draw Y axis
		p1[1]=scale;
		p2[1]=leglen*scale;
		p1[0]=p1[2]=p2[0]=p2[2]=0.0;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,axiscolor,basept,frp1,tempydir,pView);
		//draw Z axis
		p1[1]=p2[1]=0.0;
		p1[2]=scale;
		p2[2]=leglen*scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,axiscolor,basept,frp1,tempydir,pView);
		if(viewdir[0]<0.0)
			omitlet=1;
		else
			omitlet=0;
		goto letters;
	}else if(fabs(viewdir[0])<IC_ZRO && fabs(viewdir[2])<IC_ZRO){
		p2[0]=scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,xcolor,basept,frp1,tempydir,pView);
		p2[2]=p1[2]=scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,xcolor,basept,frp1,tempydir,pView);
		p1[0]=scale;
		p1[2]=0.0;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,zcolor,basept,frp1,tempydir,pView);
		p1[0]=p2[0]=0.0;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,zcolor,basept,frp1,tempydir,pView);
		//draw in the X axis
		p1[1]=p2[1]=p1[2]=p2[2]=0.0;
		p1[0]=scale;
		p2[0]=leglen*scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,axiscolor,basept,frp1,tempydir,pView);
		//draw in the Z axis
		p1[0]=p2[0]=0.0;
		p1[2]=scale;
		p2[2]=leglen*scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,axiscolor,basept,frp1,tempydir,pView);
		if(viewdir[1]<0.0)
			omitlet=2;
		else
			omitlet=0;
		goto letters;
	}else if(fabs(viewdir[0])<IC_ZRO && fabs(viewdir[1])<IC_ZRO){
		if(icadRealEqual(viewdir[2],1.0)){
			omitlet=0;
		}else{
			omitlet=4;
		}
		//draw the cube
		p2[0]=scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,xcolor,basept,frp1,tempydir,pView);
		p1[0]=p1[1]=scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,ycolor,basept,frp1,tempydir,pView);
		p2[1]=scale;
		p2[0]=0.0;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,xcolor,basept,frp1,tempydir,pView);
		p1[0]=p1[1]=0.0;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,ycolor,basept,frp1,tempydir,pView);
		//draw the axes
		p1[2]=p2[2]=p1[1]=p2[1]=0.0;
		p1[0]=scale;
		p2[0]=leglen*scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,axiscolor,basept,frp1,tempydir,pView);
		p1[0]=p2[0]=0.0;
		p1[1]=scale;
		p2[1]=leglen*scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,axiscolor,basept,frp1,tempydir,pView);
		if(icadRealEqual(viewdir[2],1.0))
			omitlet=0;
		else
			omitlet=4;
		goto letters;
	}
	//Now account for views in a plane.  3 cases possible: XY, XZ, and YZ.
	//in these cases we can't draw entire top, bottom, or side of cube
	omitplane=0;
	if(fabs(viewdir[2])<0.02)//view substantially in XY plane
		omitplane=1;
	else{
		//draw entire top or bottom of cube, depending upon Z component
		if(viewdir[2]>=0.0){
			//draw entire top of the cube 
			p1[2]=p2[2]=scale;
		}else {
			//draw entire bottom of the cube
			p1[2]=p2[2]=0.0;
		}
		//NOTE:  if viewdir[2]==0.0 we still must draw the bottom of the cube!
		p2[0]=scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,xcolor,basept,frp1,tempydir,pView);
		p1[0]=p1[1]=scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,ycolor,basept,frp1,tempydir,pView);
		p2[1]=scale;
		p2[0]=0.0;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,xcolor,basept,frp1,tempydir,pView);
		p1[0]=p1[1]=0.0;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,ycolor,basept,frp1,tempydir,pView);
	}
	if((viewdir[0]>=0.0)&&(viewdir[1]>=0.0)){
		//1st quadrant -- X & Y positive
		//draw sides of cube....
		p1[0]=p2[0]=p2[2]=scale;
		p1[1]=p1[2]=p2[1]=0.0;
		if(viewdir[0]>0.02)SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,zcolor,basept,frp1,tempydir,pView);
		p1[1]=p2[1]=scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,zcolor,basept,frp1,tempydir,pView);
		p1[0]=p2[0]=0.0;
		if(viewdir[1]>0.02)SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,zcolor,basept,frp1,tempydir,pView);
		//draw bottom or top
		if(viewdir[2]>=0.0 || omitplane){
			//draw bottom
			p1[2]=p2[2]=0.0;
			p2[0]=scale;
			if(viewdir[1]>0.02)SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,xcolor,basept,frp1,tempydir,pView);
			p1[1]=0.0;
			p1[0]=scale;
			if(viewdir[0]>0.02)SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,ycolor,basept,frp1,tempydir,pView);
		}
		if(viewdir[2]<0.0 || omitplane){
			//draw top
			p1[2]=p2[2]=p1[1]=p2[1]=p2[0]=scale;
			p1[0]=0.0;
			if(viewdir[1]>0.02)SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,xcolor,basept,frp1,tempydir,pView);
			p1[1]=0.0;
			p1[0]=scale;
			if(viewdir[0]>0.02)SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,ycolor,basept,frp1,tempydir,pView);
		}
		
		//now draw the 3 axes in
		//X
		p1[1]=p2[1]=p1[2]=p2[2]=0.0;
		p1[0]=scale;
		p2[0]=leglen*scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,axiscolor,basept,frp1,tempydir,pView);
		//Y
		p1[0]=p2[0]=p1[2]=p2[2]=0.0;
		p1[1]=scale;
		p2[1]=leglen*scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,axiscolor,basept,frp1,tempydir,pView);
		//Z
		p1[0]=p2[0]=p1[1]=p2[1]=0.0;
		//if Z negative, part of Z axis may be obscured...
		if(viewdir[2]<0.0){
			//now part of the z axis may be obscured
			if(viewdir[1]>viewdir[0])
				fr1=1.0+(-viewdir[2]/viewdir[1]);
			else
				fr1=1.0+(-viewdir[2]/viewdir[0]);
		}else{
			fr1=1.0;
		}
		if(fr1<leglen){
			p1[2]=fr1*scale;
			p2[2]=leglen*scale;
			SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,axiscolor,basept,frp1,tempydir,pView);
			omitlet=0;
		}else{
			omitlet=4;
		}
	}else if((viewdir[0]<0.0)&&(viewdir[1]>=0.0)){
		//2nd quadrant -- X negative
		p1[0]=p1[1]=p1[2]=p2[0]=p2[1]=0.0;
		p2[2]=scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,zcolor,basept,frp1,tempydir,pView);
		p2[1]=p1[1]=scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,zcolor,basept,frp1,tempydir,pView);
		p2[0]=p1[0]=scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,zcolor,basept,frp1,tempydir,pView);
		if(viewdir[2]>=0.0 || omitplane){
			//draw bottom
			p1[2]=p2[2]=0.0;
			p2[0]=0.0;
			SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,xcolor,basept,frp1,tempydir,pView);
			p1[0]=p1[1]=0.0;
			SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,ycolor,basept,frp1,tempydir,pView);
		}
		if(viewdir[2]<0.0 || omitplane){
			//draw top
			p1[2]=p2[2]=p2[1]=scale;
			p1[0]=p1[1]=p2[0]=0.0;
			SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,ycolor,basept,frp1,tempydir,pView);
			p1[0]=p1[1]=scale;
			SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,xcolor,basept,frp1,tempydir,pView);
		}
		//draw in the z and y axes
		p1[0]=p1[1]=p2[0]=p2[1]=0.0;
		p1[2]=scale;
		p2[2]=leglen*scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,axiscolor,basept,frp1,tempydir,pView);
		p1[2]=p2[2]=0.0;
		p1[1]=scale;
		p2[1]=leglen*scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,axiscolor,basept,frp1,tempydir,pView);
		if(viewdir[2]>=0.0){
			//now part of the x axis will be obscured
			if(viewdir[2]>viewdir[1])
				fr1=1.0+(-viewdir[0]/viewdir[2]);
			else
				fr1=1.0+(-viewdir[0]/viewdir[1]);
		}else{
			fr1=1.0;
		}
		if(fr1<leglen){
			p1[1]=p1[2]=p2[1]=p2[2]=0.0;
			p1[0]=fr1*scale;
			p2[0]=leglen*scale;
			SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,axiscolor,basept,frp1,tempydir,pView);
			omitlet=0;
		}else{
			omitlet=1;
		}
	}else if((viewdir[0]<0.0)&&(viewdir[1]<0.0)){
		//3rd quadrant - both X and Y negative
		//draw sides of cube....
		p1[0]=p2[0]=p2[2]=scale;
		p1[1]=p2[1]=p1[2]=0.0;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,zcolor,basept,frp1,tempydir,pView);
		p1[0]=p2[0]=0.0;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,zcolor,basept,frp1,tempydir,pView);
		p1[1]=p2[1]=scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,zcolor,basept,frp1,tempydir,pView);
		//now draw top or bottom
		if(viewdir[2]>=0.0 || omitplane){
			//draw the bottom 
			p1[2]=p2[2]=0.0;
			p2[1]=0.0;
			SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,ycolor,basept,frp1,tempydir,pView);
			p1[0]=scale;
			p1[1]=0.0;
			SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,xcolor,basept,frp1,tempydir,pView);
		}
		if(viewdir[2]<0.0  || omitplane){
			//draw the top
			p1[2]=p2[2]=p1[1]=scale;
			p1[0]=p2[0]=p2[1]=0.0;
			SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,ycolor,basept,frp1,tempydir,pView);
			p1[0]=scale;
			p1[1]=0.0;
			SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,xcolor,basept,frp1,tempydir,pView);
		}
		//draw in the z axis
		p1[0]=p1[1]=p2[0]=p2[1]=0.0;
		p1[2]=scale;
		p2[2]=leglen*scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,axiscolor,basept,frp1,tempydir,pView);
		//draw the y axis
		p1[2]=p2[2]=0.0;
		p1[1]=scale;
		p2[1]=leglen*scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,axiscolor,basept,frp1,tempydir,pView);
		//draw the x axis
		p1[1]=p2[1]=0.0;
		p1[0]=scale;
		p2[0]=leglen*scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,axiscolor,basept,frp1,tempydir,pView);
		omitlet=0;
	}else if((viewdir[0]>=0.0)&&(viewdir[1]<0.0)){
		//4th quadrant -- only Y component negative
		//draw sides of cube....
		p1[0]=p2[0]=p1[1]=p2[1]=p1[2]=0.0;
		p2[2]=scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,zcolor,basept,frp1,tempydir,pView);
		p1[0]=p2[0]=scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,zcolor,basept,frp1,tempydir,pView);
		p1[1]=p2[1]=scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,zcolor,basept,frp1,tempydir,pView);
		//draw the bottom
		if(viewdir[2]>=0.0 || omitplane){
			//draw the bottom
			p1[2]=p2[2]=0.0;
			p1[0]=p1[1]=p2[1]=0.0;
			p2[0]=scale;
			SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,xcolor,basept,frp1,tempydir,pView);
			p1[0]=p1[1]=scale;
			SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,ycolor,basept,frp1,tempydir,pView);
		}
		if(viewdir[2]<0.0 || omitplane){
			//draw the top
			p1[2]=p2[2]=p2[0]=scale;
			p1[0]=p1[1]=p2[1]=0.0;
			SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,xcolor,basept,frp1,tempydir,pView);
			p1[0]=p1[1]=scale;
			SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,ycolor,basept,frp1,tempydir,pView);
		}

		//draw the x axis
		p1[1]=p2[1]=p1[2]=p2[2]=0.0;
		p1[0]=scale;
		p2[0]=leglen*scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,axiscolor,basept,frp1,tempydir,pView);
		//draw the Z axis
		p1[0]=p2[0]=0.0;
		p1[2]=scale;
		p2[2]=leglen*scale;
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,axiscolor,basept,frp1,tempydir,pView);
		if(viewdir[2]>=0.0){
			//now part of the Y axis will be obscured
			if(viewdir[2]>viewdir[0])
				fr1=1.0+(-viewdir[1]/viewdir[2]);
			else
				fr1=1.0+(-viewdir[1]/viewdir[0]);
		}else{
			fr1=1.0;
		}
		if(fr1<leglen){
			p1[0]=p1[2]=p2[0]=p2[2]=0.0;
			p1[1]=fr1*scale;
			p2[1]=leglen*scale;
			SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,axiscolor,basept,frp1,tempydir,pView);
			omitlet=0;
		}else{
			omitlet=2;
		}
	}
letters:
	//viewdir is no longer needed for drawing the axes in appropriate perspective
	if((omitlet&1)==0){
		//draw the x
		if(frp1!=NULL){
			p2[0]=(leglen+textsize)*scale;
			p3[0]=p2[0]*tempxdir[0];//+p1[1]*tempxdir[1]+p1[2]*tempxdir[2];
			p3[1]=p2[0]*tempydir[0];//+p1[1]*tempydir[1]+p1[2]*tempydir[2];
			p3[0]+=basept[0];
			p3[1]+=basept[1];
		}else{
			ic_ptcpy(p1,basept);
			p1[0]+=(leglen+textsize)*scale;
			gr_ucs2rp(p1,p3,view);
		}
		p3[0]-=(0.4*textsize*scale);
		p4[0]=p3[0]+0.8*textsize*scale;
		p3[1]-=(0.5*textsize*scale);
		p4[1]=p3[1]+textsize*scale;
		gr_rp2ucs(p3,p1,view);
		gr_rp2ucs(p4,p2,view);
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,xcolor,NULL,NULL,NULL,pView);
		p4[1]=p3[1];
		p3[1]+=(textsize*scale);
		gr_rp2ucs(p3,p1,view);
		gr_rp2ucs(p4,p2,view);
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,xcolor,NULL,NULL,NULL,pView);
	}
	if((omitlet&2)==0){
		//draw the y
		if(frp1!=NULL){
			p2[1]=(leglen+textsize)*scale;
			p3[0]=p2[1]*tempxdir[1];
			p3[1]=p2[1]*tempydir[1];
			p3[0]+=basept[0];
			p3[1]+=basept[1];
		}else{
			ic_ptcpy(p1,basept);
			p1[1]+=(leglen+textsize)*scale;
			gr_ucs2rp(p1,p3,view);
		}
		p4[0]=p3[0]+0.4*textsize*scale;
		p4[1]=p3[1]+0.5*textsize*scale;
		gr_rp2ucs(p4,p2,view);
		gr_rp2ucs(p3,p1,view);
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,ycolor,NULL,NULL,NULL,pView);
		p4[0]-=(0.8*textsize*scale);
		gr_rp2ucs(p4,p2,view);
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,ycolor,NULL,NULL,NULL,pView);
		p4[0]=p3[0];
		p4[1]=p3[1]-0.5*textsize*scale;
		gr_rp2ucs(p4,p2,view);
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,ycolor,NULL,NULL,NULL,pView);
	}
	if((omitlet&4)==0){
		//draw the z
		if(frp1!=NULL){
			p2[2]=(leglen+textsize)*scale;
			p3[0]=p2[2]*tempxdir[2];
			p3[1]=p2[2]*tempydir[2];
			p3[0]+=basept[0];
			p3[1]+=basept[1];
		}else{
			ic_ptcpy(p1,basept);
			p1[2]+=(leglen+textsize)*scale;
			gr_ucs2rp(p1,p3,view);
		}
		p3[0]-=(0.4*textsize*scale);
		p3[1]+=(0.5*textsize*scale);
		p4[0]=p3[0]+0.8*textsize*scale;
		p4[1]=p3[1];
		gr_rp2ucs(p3,p1,view);
		gr_rp2ucs(p4,p2,view);
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,zcolor,NULL,NULL,NULL,pView);
		p3[1]-=(textsize*scale);
		gr_rp2ucs(p3,p1,view);
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,zcolor,NULL,NULL,NULL,pView);
		p4[1]=p3[1];
		gr_rp2ucs(p4,p2,view);
		SDS_UcsGrDraw(pBufferDrawDevice,p1,p2,zcolor,NULL,NULL,NULL,pView);
	}
	out: ;

	pView->ReleaseDrawDevice(pBufferDrawDevice);
	SDS_BitBlt2View(0, pView);

	return(ret);
#else
	if(pView == NULL)
		pView = SDS_CURVIEW;
	CIcadEntityRenderer renderer(pView->m_pViewsDoc->m_dbDrawing, pView);
	int result = renderer.drawUCSIcon(vdir, sclfact, unpaint);
	SDS_BitBlt2View(0, pView);
	return result;
#endif
}

CUCSIcon::CUCSIcon(db_drawing* pDrawing):
m_pDrawing(pDrawing), m_pView(NULL)
{
}

int CUCSIcon::draw(sds_point vdir, const sds_real* sclfact, int unpaint, CDrawDevice* pDrawDevice, CIcadView* pView)
{
	//This f'n draws the ucs icon as viewed from NEGATIVE vdir(remember,
	//	viewdir always goes away from viewctr, we're looking toward it).
	//If vdir==NULL, current setting of VIEWDIR var is used
	//If sclfact==NULL, then 1 is used
	//If vdir!=NULL, icon is always drawn in center of screen.
	//If unpaint!=0, icon will be painted OUT!!!
	//NOTE:  if we're in paper space AND vdir==NULL, f'n draws the paper space icon
	int ret=RTNORM;
	int omitlet,omitplane,vmode,xcolor,ycolor,zcolor,axiscolor,pspace,
		a,b,c,fi1,tilemode=1;
	sds_resbuf rb,rb2;
	sds_real fr1,fr2,scale,iconsize,leglen,textsize,*frp1;
	sds_point viewdir,basept,p1,p2,p3,p4,tempxdir,tempydir;

	if (pView->m_pViewsDoc != NULL)
	{
		bool test = m_pDrawing == pView->m_pViewsDoc->m_dbDrawing;
		ASSERT(test);
	}

	m_pView = pView;
	struct gr_view* view = pView->m_pCurDwgView;
	if(view == NULL)
		return(RTERROR);

	if(pView->m_pVportTabHip==NULL) return(RTERROR);
	
	// Make sure the corect setvars are in this table entry.
	// Because this function looks at the table entry directly.
//XX	if(pView==SDS_CURVIEW) SDS_VarToTab(SDS_CURDWG,pView->m_pVportTabHip);

	SDS_getvar(NULL,DB_QTILEMODE,&rb,m_pDrawing,&SDS_CURCFG,&SDS_CURSES);
	tilemode=rb.resval.rint;

	if(tilemode) {
		pView->m_pVportTabHip->get_74(&a);
	} else {
		SDS_getvar(NULL,DB_QUCSICON,&rb,m_pDrawing,&SDS_CURCFG,&SDS_CURSES);
		a=rb.resval.rint;
	}
	if((a&1)==0 && vdir==NULL) 
		{
		return(SDS_RTNONE);
		}
	
	if(sclfact==NULL)scale=1.0;
	else scale=*sclfact;
	if(scale<=0.0) return(RTERROR);

	iconsize=0.035;	//how big icon is relative to size of screen
	leglen=2.5;		//axis leg length relative to cube length (NOTE: if 
					//we're in pspace, this will get adjusted
	textsize=0.65;	//text ht relative to cube length
	
	pspace=0;
	//Check to see if we're in paper space....
	if(!tilemode) {
		SDS_getvar(NULL,DB_QCVPORT,&rb,m_pDrawing,&SDS_CURCFG,&SDS_CURSES);
		//NOTE if in pspace, ICAD will return correct UCSXDIR & YDIR for pspace
		if(1==rb.resval.rint)pspace=1;
	}

	if(tilemode) {
		pView->m_pVportTabHip->get_71(&vmode);
	} else {
		SDS_getvar(NULL,DB_QVIEWMODE,&rb,m_pDrawing,&SDS_CURCFG,&SDS_CURSES);
		vmode=rb.resval.rint;
	}

	if(0==(vmode&1)){	//don't need colors if we draw perspective icon
		if(unpaint){
			xcolor=ycolor=zcolor=axiscolor=256;
		}else{
			SDS_getvar(NULL,DB_QCOLORX,&rb,m_pDrawing,&SDS_CURCFG,&SDS_CURSES);
			xcolor=rb.resval.rint;
			SDS_getvar(NULL,DB_QCOLORY,&rb,m_pDrawing,&SDS_CURCFG,&SDS_CURSES);
			ycolor=rb.resval.rint;
			SDS_getvar(NULL,DB_QCOLORZ,&rb,m_pDrawing,&SDS_CURCFG,&SDS_CURSES);
			zcolor=rb.resval.rint;
			axiscolor=-1;
		}
	}

	if(vdir==NULL){
		if(tilemode) {
			pView->m_pVportTabHip->get_16(viewdir);
			rb.restype=rb2.restype=RTSHORT;
			rb.resval.rint=0;
			rb2.resval.rint=1;
			sds_trans(viewdir,&rb,&rb2,1,viewdir);
		} else {
			SDS_getvar(NULL,DB_QVIEWDIR,&rb,m_pDrawing,&SDS_CURCFG,&SDS_CURSES);
			ic_ptcpy(viewdir,rb.resval.rpoint);
		}
		frp1=NULL;
	}else{
		ic_ptcpy(viewdir,vdir);
		//now we'll have to generate two temporary axes to use when viewing
		//the desired points.  
		tempxdir[0]=-1.0*vdir[1];
		tempxdir[1]=vdir[0];
		tempxdir[2]=0.0;
		fr1=sqrt(tempxdir[0]*tempxdir[0]+tempxdir[1]+tempxdir[1]);
		frp1=tempxdir;
		if(fr1>IC_ZRO){
			tempxdir[0]*=fr1;
			tempxdir[1]*=fr1;
			tempxdir[2]=0.0;
			tempydir[0]=-tempxdir[1]*vdir[2];
			tempydir[1]=tempxdir[0]*vdir[2];
			tempydir[2]=vdir[0]*tempxdir[1]-vdir[1]*tempxdir[0];
		}else{
			if(vdir[2]>0.0){
				tempxdir[0]=tempydir[1]=1.0;
				tempxdir[1]=tempxdir[2]=tempydir[0]=tempydir[2]=0.0;
			}else{
				tempxdir[0]=-1.0;
				tempydir[1]=1.0;
				tempxdir[1]=tempxdir[2]=tempydir[0]=tempydir[2]=0.0;
			}
		}
	}
	
	//get magnitude of viewdir & "unitize" the vector
	fr1=sqrt(viewdir[0]*viewdir[0]+viewdir[1]*viewdir[1]+viewdir[2]*viewdir[2]);
	//keep the magnitude, because it's used to calc viewsize in perspective views
	if(fr1<IC_ZRO) { ret=RTERROR; goto out; }
	viewdir[0]/=fr1;
	viewdir[1]/=fr1;
	viewdir[2]/=fr1;

	//First, we have to calc where to put the icon on the screen.
	if(tilemode) {
		if(vmode&1){
			pView->m_pVportTabHip->get_17(p1);//target
		}else{
			pView->m_pVportTabHip->get_12(p1);//viewctr
			pView->m_pVportTabHip->get_40(&fr1);
		}
	} else {
		if(vmode&1){
			SDS_getvar(NULL,DB_QTARGET,&rb,m_pDrawing,&SDS_CURCFG,&SDS_CURSES);
			ic_ptcpy(p1,rb.resval.rpoint);			
		}else{
			SDS_getvar(NULL,DB_QVIEWCTR,&rb,m_pDrawing,&SDS_CURCFG,&SDS_CURSES);
			ic_ptcpy(p1,rb.resval.rpoint);
			SDS_getvar(NULL,DB_QVIEWSIZE,&rb,m_pDrawing,&SDS_CURCFG,&SDS_CURSES);
			fr1=rb.resval.rreal;
		}
	}
	if(vmode&1){
		//viewsize is meaningless, but we still have fr1 as camera-target distance
		if(tilemode)pView->m_pVportTabHip->get_42(&fr2);
		else{
			SDS_getvar(NULL,DB_QLENSLENGTH,&rb,m_pDrawing,&SDS_CURCFG,&SDS_CURSES);
			fr2=rb.resval.rreal;
		}
		//fr2 is lens length
		fr2=35.0*fr1/fr2;//fr2 is now width of screen.  we need to get height
		SDS_getvar(NULL,DB_QSCREENSIZE,&rb,m_pDrawing,&SDS_CURCFG,&SDS_CURSES);
		if(icadRealEqual(rb.resval.rpoint[0],0.0)){ ret=RTERROR; goto out; }
		fr1=fr2*rb.resval.rpoint[1]/rb.resval.rpoint[0];
	}

	//calculate p3 as icon origin in rb coords
	scale=scale*iconsize*fr1;
	if(vdir!=NULL){	   //just draw icon in center of screen
		/*if(tilemode)
			ic_ptcpy(basept,p1);
		else*/
			gr_ucs2rp(p1,basept,view);
	}else{
		//  x/y ratio

		if(tilemode) {
			if(pView->m_iWinY==0) { ret=RTERROR; goto out; }
			fr2=(double)pView->m_iWinX/(double)pView->m_iWinY;  
		} else {
			SDS_getvar(NULL,DB_QCVPORT,&rb,m_pDrawing,&SDS_CURCFG,&SDS_CURSES);
			if(1==rb.resval.rint) {
				if(pView->m_iWinY==0) { ret=RTERROR; goto out; }
				fr2=(double)pView->m_iWinX/(double)pView->m_iWinY;  
			} else {
				double ht,wd;
				pView->m_pVportTabHip->get_41(&ht);
				pView->m_pVportTabHip->get_40(&wd);
				if(icadRealEqual(ht,0.0)) { ret=RTERROR; goto out; }
				fr2=wd/ht;  
//				pView->m_pVportTabHip->get_41(&fr2);
			}
		}
		if(tilemode)
			ic_ptcpy(p2,p1);
		else
			gr_ucs2rp(p1,p2,view);

		//if we're in standard mode & vdir not passed to f'n
		if(tilemode) {
			pView->m_pVportTabHip->get_74(&fi1);
		} else {
			SDS_getvar(NULL,DB_QUCSICON,&rb,m_pDrawing,&SDS_CURCFG,&SDS_CURSES);
			fi1=rb.resval.rint;
		}
		if(3==fi1 && !(vmode&1)){
			//check to see if origin is within proper area
			//so that icon can be displayed at origin
			//check to see if max dist along x axis is within spec
			fi1=1;	//default to we can't display at origin
			p4[0]=0.98*0.5*fr2*fr1;
			p4[1]=0.98*0.5*fr1;
			basept[1]=basept[2]=0.0;
			basept[0]=scale*(leglen+2.0*textsize);
			gr_ucs2rp(basept,p1,view);
			if((fabs(p2[0]-p1[0])<=(p4[0]))&&
			   (fabs(p2[1]-p1[1])<=(p4[1]))){
				//check along y axis
				basept[1]=basept[0];
				basept[0]=0.0;
				gr_ucs2rp(basept,p1,view);
				if((fabs(p2[0]-p1[0])<=(p4[0]))&&
				   (fabs(p2[1]-p1[1])<=(p4[1]))){
					basept[2]=basept[1];
					basept[1]=0.0;
					gr_ucs2rp(basept,p1,view);
					if((fabs(p2[0]-p1[0])<=(p4[0]))&&
					   (fabs(p2[1]-p1[1])<=(p4[1]))){
						basept[2]=0.0;
						gr_ucs2rp(basept,p1,view);
						if((fabs(p2[0]-p1[0])<=(p4[0]))&&
						   (fabs(p2[1]-p1[1])<=(p4[1]))){
							ic_ptcpy(p3,p1);
							fi1=3;//yes, display at origin
						}
					}
				}
			}
		}
		if(fi1!=3 || (vmode&1)){
			//back off of corner distance 2.0 for axes, 1.1 safety factor, 1.414 for 
			//	longest possible diagonal view
			p3[0]=p2[0]+(0.5*fr2*fr1)-(2.5*1.414*scale);
			if(vmode&1)	p3[1]=p2[1]-(0.45*fr1);
			else		p3[1]=p2[1]-(0.5*fr1)+(2.5*1.414*scale);
			p3[2]=0.0;
		}

		//if we're in perspective, use alternate icon
		if(vmode&1){
			scale*=leglen;
			drawAlternate(&scale, p3, viewdir, 0, pDrawDevice);
			goto out;
		}else{
			gr_rp2ucs(p3,basept,view);
		}	
	
		//check to see if view matches world coordinates, & draw the W 
		p3[0]=p3[1]=p3[2]=0.0;
		SDS_getvar(NULL,DB_QUCSORG,&rb,m_pDrawing,&SDS_CURCFG,&SDS_CURSES);
		if(icadPointEqual(rb.resval.rpoint,p3)){
			SDS_getvar(NULL,DB_QUCSXDIR,&rb,m_pDrawing,&SDS_CURCFG,&SDS_CURSES);
			ic_ptcpy(p1,rb.resval.rpoint);
			SDS_getvar(NULL,DB_QUCSYDIR,&rb,m_pDrawing,&SDS_CURCFG,&SDS_CURSES);
			ic_ptcpy(p2,rb.resval.rpoint);
			p3[0]=1.0;
			p4[0]=p4[2]=0.0;
			p4[1]=1.0;
			if(icadPointEqual(p1,p3)&&icadPointEqual(p2,p4)){
				//if we're looking substantially along XY plane
				if(!pspace && fabs(viewdir[2]<0.30)){
					if(fabs(viewdir[1])>fabs(viewdir[0])){
						a=0;
						b=2;
						c=1;
					}else{
						a=1;
						b=2;
						c=0;
					}
				}else{
					a=0;
					b=1;
					c=2;
				}

				if(!pspace && viewdir[c]>0.0){
					p1[c]=p2[c]=scale;
				}else{
					//in paper space, always draw the W in the z=0 plane
					p1[c]=p2[c]=0.0;
				}
				//draw the W marker
				p1[a]=0.35*scale;
				p1[b]=0.75*scale;
				p2[a]=0.5*scale;
				p2[b]=0.25*scale;
				drawElement(p1,p2,axiscolor,basept,NULL,NULL,pDrawDevice);
				p1[a]=0.6*scale;
				p1[b]=0.625*scale;
				drawElement(p1,p2,axiscolor,basept,NULL,NULL,pDrawDevice);
				p2[a]=0.7*scale;
				drawElement(p1,p2,axiscolor,basept,NULL,NULL,pDrawDevice);
				p1[a]=0.85*scale;
				p1[b]=0.75*scale;
				drawElement(p1,p2,axiscolor,basept,NULL,NULL,pDrawDevice);
			}
		}
	}
	//We'll break this down into case statements - one for pspace viewing
	//	and one for each possible octant of mspace viewing & one for 
	//  viewing along each axis	from either direction (only 2 real possibilities)
	p1[0]=p1[1]=p1[2]=p2[0]=p2[1]=p2[2]=0.0;
	if(pspace){
		//draw the edges of the paper in x & Y colors
		p2[1]=2.0*scale;
		drawElement(p1,p2,ycolor,basept,frp1,tempydir,pDrawDevice);
		p2[0]=1.5*scale;
		p2[1]=0.0;
		drawElement(p1,p2,xcolor,basept,frp1,tempydir,pDrawDevice);
		p1[0]=1.5*scale;
		p1[1]=1.7*scale;
		drawElement(p1,p2,ycolor,basept,frp1,tempydir,pDrawDevice);
		p1[0]=1.2*scale;
		p1[1]=p2[1]=2.0*scale;
		p2[0]=0.0;
		drawElement(p1,p2,xcolor,basept,frp1,tempydir,pDrawDevice);
		//now draw the folded corner of the page
		p2[0]=1.2*scale;
		p2[1]=1.7*scale;
		drawElement(p1,p2,axiscolor,basept,frp1,tempydir,pDrawDevice);
		p1[0]=1.5*scale;
		p1[1]=1.7*scale;
		drawElement(p1,p2,axiscolor,basept,frp1,tempydir,pDrawDevice);
		p2[0]=1.2*scale;
		p2[1]=2.0*scale;
		//draw the fold itself as dashed...
		drawElement(p1,p2,axiscolor,basept,frp1,tempydir,pDrawDevice);
		//draw X axis
		p1[0]=1.5*scale;
		p2[0]=leglen*scale;
		p1[1]=p1[2]=p2[1]=p2[2]=0.0;
		drawElement(p1,p2,axiscolor,basept,frp1,tempydir,pDrawDevice);
		//draw Y axis
		p1[1]=2.0*scale;
		p2[1]=leglen*scale;
		p1[0]=p2[0]=0.0;
		drawElement(p1,p2,axiscolor,basept,frp1,tempydir,pDrawDevice);
		//draw z axis
		p1[0]=p1[1]=p1[2]=p2[0]=p2[1]=0.0;
		p2[2]=scale;
		drawElement(p1,p2,axiscolor,basept,frp1,tempydir,pDrawDevice);
		//don't draw in the letter Z
		omitlet=4;
		goto letters;
	}else if(fabs(viewdir[1])<IC_ZRO && fabs(viewdir[2])<IC_ZRO){	
		//view along X axis 
		p2[1]=scale;
		drawElement(p1,p2,ycolor,basept,frp1,tempydir,pDrawDevice);
		p1[2]=p2[2]=scale;
		drawElement(p1,p2,ycolor,basept,frp1,tempydir,pDrawDevice);
		p2[1]=p2[2]=0.0;
		drawElement(p1,p2,zcolor,basept,frp1,tempydir,pDrawDevice);
		p1[1]=p2[1]=scale;
		drawElement(p1,p2,zcolor,basept,frp1,tempydir,pDrawDevice);
		//draw Y axis
		p1[1]=scale;
		p2[1]=leglen*scale;
		p1[0]=p1[2]=p2[0]=p2[2]=0.0;
		drawElement(p1,p2,axiscolor,basept,frp1,tempydir,pDrawDevice);
		//draw Z axis
		p1[1]=p2[1]=0.0;
		p1[2]=scale;
		p2[2]=leglen*scale;
		drawElement(p1,p2,axiscolor,basept,frp1,tempydir,pDrawDevice);
		if(viewdir[0]<0.0)
			omitlet=1;
		else
			omitlet=0;
		goto letters;
	}else if(fabs(viewdir[0])<IC_ZRO && fabs(viewdir[2])<IC_ZRO){
		p2[0]=scale;
		drawElement(p1,p2,xcolor,basept,frp1,tempydir,pDrawDevice);
		p2[2]=p1[2]=scale;
		drawElement(p1,p2,xcolor,basept,frp1,tempydir,pDrawDevice);
		p1[0]=scale;
		p1[2]=0.0;
		drawElement(p1,p2,zcolor,basept,frp1,tempydir,pDrawDevice);
		p1[0]=p2[0]=0.0;
		drawElement(p1,p2,zcolor,basept,frp1,tempydir,pDrawDevice);
		//draw in the X axis
		p1[1]=p2[1]=p1[2]=p2[2]=0.0;
		p1[0]=scale;
		p2[0]=leglen*scale;
		drawElement(p1,p2,axiscolor,basept,frp1,tempydir,pDrawDevice);
		//draw in the Z axis
		p1[0]=p2[0]=0.0;
		p1[2]=scale;
		p2[2]=leglen*scale;
		drawElement(p1,p2,axiscolor,basept,frp1,tempydir,pDrawDevice);
		if(viewdir[1]<0.0)
			omitlet=2;
		else
			omitlet=0;
		goto letters;
	}else if(fabs(viewdir[0])<IC_ZRO && fabs(viewdir[1])<IC_ZRO){
		if(icadRealEqual(viewdir[2],1.0)){
			omitlet=0;
		}else{
			omitlet=4;
		}
		//draw the cube
		p2[0]=scale;
		drawElement(p1,p2,xcolor,basept,frp1,tempydir,pDrawDevice);
		p1[0]=p1[1]=scale;
		drawElement(p1,p2,ycolor,basept,frp1,tempydir,pDrawDevice);
		p2[1]=scale;
		p2[0]=0.0;
		drawElement(p1,p2,xcolor,basept,frp1,tempydir,pDrawDevice);
		p1[0]=p1[1]=0.0;
		drawElement(p1,p2,ycolor,basept,frp1,tempydir,pDrawDevice);
		//draw the axes
		p1[2]=p2[2]=p1[1]=p2[1]=0.0;
		p1[0]=scale;
		p2[0]=leglen*scale;
		drawElement(p1,p2,axiscolor,basept,frp1,tempydir,pDrawDevice);
		p1[0]=p2[0]=0.0;
		p1[1]=scale;
		p2[1]=leglen*scale;
		drawElement(p1,p2,axiscolor,basept,frp1,tempydir,pDrawDevice);
		if(icadRealEqual(viewdir[2],1.0))
			omitlet=0;
		else
			omitlet=4;
		goto letters;
	}
	//Now account for views in a plane.  3 cases possible: XY, XZ, and YZ.
	//in these cases we can't draw entire top, bottom, or side of cube
	omitplane=0;
	if(fabs(viewdir[2])<0.02)//view substantially in XY plane
		omitplane=1;
	else{
		//draw entire top or bottom of cube, depending upon Z component
		if(viewdir[2]>=0.0){
			//draw entire top of the cube 
			p1[2]=p2[2]=scale;
		}else {
			//draw entire bottom of the cube
			p1[2]=p2[2]=0.0;
		}
		//NOTE:  if viewdir[2]==0.0 we still must draw the bottom of the cube!
		p2[0]=scale;
		drawElement(p1,p2,xcolor,basept,frp1,tempydir,pDrawDevice);
		p1[0]=p1[1]=scale;
		drawElement(p1,p2,ycolor,basept,frp1,tempydir,pDrawDevice);
		p2[1]=scale;
		p2[0]=0.0;
		drawElement(p1,p2,xcolor,basept,frp1,tempydir,pDrawDevice);
		p1[0]=p1[1]=0.0;
		drawElement(p1,p2,ycolor,basept,frp1,tempydir,pDrawDevice);
	}
	if((viewdir[0]>=0.0)&&(viewdir[1]>=0.0)){
		//1st quadrant -- X & Y positive
		//draw sides of cube....
		p1[0]=p2[0]=p2[2]=scale;
		p1[1]=p1[2]=p2[1]=0.0;
		if(viewdir[0]>0.02)drawElement(p1,p2,zcolor,basept,frp1,tempydir,pDrawDevice);
		p1[1]=p2[1]=scale;
		drawElement(p1,p2,zcolor,basept,frp1,tempydir,pDrawDevice);
		p1[0]=p2[0]=0.0;
		if(viewdir[1]>0.02)drawElement(p1,p2,zcolor,basept,frp1,tempydir,pDrawDevice);
		//draw bottom or top
		if(viewdir[2]>=0.0 || omitplane){
			//draw bottom
			p1[2]=p2[2]=0.0;
			p2[0]=scale;
			if(viewdir[1]>0.02)drawElement(p1,p2,xcolor,basept,frp1,tempydir,pDrawDevice);
			p1[1]=0.0;
			p1[0]=scale;
			if(viewdir[0]>0.02)drawElement(p1,p2,ycolor,basept,frp1,tempydir,pDrawDevice);
		}
		if(viewdir[2]<0.0 || omitplane){
			//draw top
			p1[2]=p2[2]=p1[1]=p2[1]=p2[0]=scale;
			p1[0]=0.0;
			if(viewdir[1]>0.02)drawElement(p1,p2,xcolor,basept,frp1,tempydir,pDrawDevice);
			p1[1]=0.0;
			p1[0]=scale;
			if(viewdir[0]>0.02)drawElement(p1,p2,ycolor,basept,frp1,tempydir,pDrawDevice);
		}
		
		//now draw the 3 axes in
		//X
		p1[1]=p2[1]=p1[2]=p2[2]=0.0;
		p1[0]=scale;
		p2[0]=leglen*scale;
		drawElement(p1,p2,axiscolor,basept,frp1,tempydir,pDrawDevice);
		//Y
		p1[0]=p2[0]=p1[2]=p2[2]=0.0;
		p1[1]=scale;
		p2[1]=leglen*scale;
		drawElement(p1,p2,axiscolor,basept,frp1,tempydir,pDrawDevice);
		//Z
		p1[0]=p2[0]=p1[1]=p2[1]=0.0;
		//if Z negative, part of Z axis may be obscured...
		if(viewdir[2]<0.0){
			//now part of the z axis may be obscured
			if(viewdir[1]>viewdir[0])
				fr1=1.0+(-viewdir[2]/viewdir[1]);
			else
				fr1=1.0+(-viewdir[2]/viewdir[0]);
		}else{
			fr1=1.0;
		}
		if(fr1<leglen){
			p1[2]=fr1*scale;
			p2[2]=leglen*scale;
			drawElement(p1,p2,axiscolor,basept,frp1,tempydir,pDrawDevice);
			omitlet=0;
		}else{
			omitlet=4;
		}
	}else if((viewdir[0]<0.0)&&(viewdir[1]>=0.0)){
		//2nd quadrant -- X negative
		p1[0]=p1[1]=p1[2]=p2[0]=p2[1]=0.0;
		p2[2]=scale;
		drawElement(p1,p2,zcolor,basept,frp1,tempydir,pDrawDevice);
		p2[1]=p1[1]=scale;
		drawElement(p1,p2,zcolor,basept,frp1,tempydir,pDrawDevice);
		p2[0]=p1[0]=scale;
		drawElement(p1,p2,zcolor,basept,frp1,tempydir,pDrawDevice);
		if(viewdir[2]>=0.0 || omitplane){
			//draw bottom
			p1[2]=p2[2]=0.0;
			p2[0]=0.0;
			drawElement(p1,p2,xcolor,basept,frp1,tempydir,pDrawDevice);
			p1[0]=p1[1]=0.0;
			drawElement(p1,p2,ycolor,basept,frp1,tempydir,pDrawDevice);
		}
		if(viewdir[2]<0.0 || omitplane){
			//draw top
			p1[2]=p2[2]=p2[1]=scale;
			p1[0]=p1[1]=p2[0]=0.0;
			drawElement(p1,p2,ycolor,basept,frp1,tempydir,pDrawDevice);
			p1[0]=p1[1]=scale;
			drawElement(p1,p2,xcolor,basept,frp1,tempydir,pDrawDevice);
		}
		//draw in the z and y axes
		p1[0]=p1[1]=p2[0]=p2[1]=0.0;
		p1[2]=scale;
		p2[2]=leglen*scale;
		drawElement(p1,p2,axiscolor,basept,frp1,tempydir,pDrawDevice);
		p1[2]=p2[2]=0.0;
		p1[1]=scale;
		p2[1]=leglen*scale;
		drawElement(p1,p2,axiscolor,basept,frp1,tempydir,pDrawDevice);
		if(viewdir[2]>=0.0){
			//now part of the x axis will be obscured
			if(viewdir[2]>viewdir[1])
				fr1=1.0+(-viewdir[0]/viewdir[2]);
			else
				fr1=1.0+(-viewdir[0]/viewdir[1]);
		}else{
			fr1=1.0;
		}
		if(fr1<leglen){
			p1[1]=p1[2]=p2[1]=p2[2]=0.0;
			p1[0]=fr1*scale;
			p2[0]=leglen*scale;
			drawElement(p1,p2,axiscolor,basept,frp1,tempydir,pDrawDevice);
			omitlet=0;
		}else{
			omitlet=1;
		}
	}else if((viewdir[0]<0.0)&&(viewdir[1]<0.0)){
		//3rd quadrant - both X and Y negative
		//draw sides of cube....
		p1[0]=p2[0]=p2[2]=scale;
		p1[1]=p2[1]=p1[2]=0.0;
		drawElement(p1,p2,zcolor,basept,frp1,tempydir,pDrawDevice);
		p1[0]=p2[0]=0.0;
		drawElement(p1,p2,zcolor,basept,frp1,tempydir,pDrawDevice);
		p1[1]=p2[1]=scale;
		drawElement(p1,p2,zcolor,basept,frp1,tempydir,pDrawDevice);
		//now draw top or bottom
		if(viewdir[2]>=0.0 || omitplane){
			//draw the bottom 
			p1[2]=p2[2]=0.0;
			p2[1]=0.0;
			drawElement(p1,p2,ycolor,basept,frp1,tempydir,pDrawDevice);
			p1[0]=scale;
			p1[1]=0.0;
			drawElement(p1,p2,xcolor,basept,frp1,tempydir,pDrawDevice);
		}
		if(viewdir[2]<0.0  || omitplane){
			//draw the top
			p1[2]=p2[2]=p1[1]=scale;
			p1[0]=p2[0]=p2[1]=0.0;
			drawElement(p1,p2,ycolor,basept,frp1,tempydir,pDrawDevice);
			p1[0]=scale;
			p1[1]=0.0;
			drawElement(p1,p2,xcolor,basept,frp1,tempydir,pDrawDevice);
		}
		//draw in the z axis
		p1[0]=p1[1]=p2[0]=p2[1]=0.0;
		p1[2]=scale;
		p2[2]=leglen*scale;
		drawElement(p1,p2,axiscolor,basept,frp1,tempydir,pDrawDevice);
		//draw the y axis
		p1[2]=p2[2]=0.0;
		p1[1]=scale;
		p2[1]=leglen*scale;
		drawElement(p1,p2,axiscolor,basept,frp1,tempydir,pDrawDevice);
		//draw the x axis
		p1[1]=p2[1]=0.0;
		p1[0]=scale;
		p2[0]=leglen*scale;
		drawElement(p1,p2,axiscolor,basept,frp1,tempydir,pDrawDevice);
		omitlet=0;
	}else if((viewdir[0]>=0.0)&&(viewdir[1]<0.0)){
		//4th quadrant -- only Y component negative
		//draw sides of cube....
		p1[0]=p2[0]=p1[1]=p2[1]=p1[2]=0.0;
		p2[2]=scale;
		drawElement(p1,p2,zcolor,basept,frp1,tempydir,pDrawDevice);
		p1[0]=p2[0]=scale;
		drawElement(p1,p2,zcolor,basept,frp1,tempydir,pDrawDevice);
		p1[1]=p2[1]=scale;
		drawElement(p1,p2,zcolor,basept,frp1,tempydir,pDrawDevice);
		//draw the bottom
		if(viewdir[2]>=0.0 || omitplane){
			//draw the bottom
			p1[2]=p2[2]=0.0;
			p1[0]=p1[1]=p2[1]=0.0;
			p2[0]=scale;
			drawElement(p1,p2,xcolor,basept,frp1,tempydir,pDrawDevice);
			p1[0]=p1[1]=scale;
			drawElement(p1,p2,ycolor,basept,frp1,tempydir,pDrawDevice);
		}
		if(viewdir[2]<0.0 || omitplane){
			//draw the top
			p1[2]=p2[2]=p2[0]=scale;
			p1[0]=p1[1]=p2[1]=0.0;
			drawElement(p1,p2,xcolor,basept,frp1,tempydir,pDrawDevice);
			p1[0]=p1[1]=scale;
			drawElement(p1,p2,ycolor,basept,frp1,tempydir,pDrawDevice);
		}

		//draw the x axis
		p1[1]=p2[1]=p1[2]=p2[2]=0.0;
		p1[0]=scale;
		p2[0]=leglen*scale;
		drawElement(p1,p2,axiscolor,basept,frp1,tempydir,pDrawDevice);
		//draw the Z axis
		p1[0]=p2[0]=0.0;
		p1[2]=scale;
		p2[2]=leglen*scale;
		drawElement(p1,p2,axiscolor,basept,frp1,tempydir,pDrawDevice);
		if(viewdir[2]>=0.0){
			//now part of the Y axis will be obscured
			if(viewdir[2]>viewdir[0])
				fr1=1.0+(-viewdir[1]/viewdir[2]);
			else
				fr1=1.0+(-viewdir[1]/viewdir[0]);
		}else{
			fr1=1.0;
		}
		if(fr1<leglen){
			p1[0]=p1[2]=p2[0]=p2[2]=0.0;
			p1[1]=fr1*scale;
			p2[1]=leglen*scale;
			drawElement(p1,p2,axiscolor,basept,frp1,tempydir,pDrawDevice);
			omitlet=0;
		}else{
			omitlet=2;
		}
	}
letters:
	//viewdir is no longer needed for drawing the axes in appropriate perspective
	if((omitlet&1)==0){
		//draw the x
		if(frp1!=NULL){
			p2[0]=(leglen+textsize)*scale;
			p3[0]=p2[0]*tempxdir[0];//+p1[1]*tempxdir[1]+p1[2]*tempxdir[2];
			p3[1]=p2[0]*tempydir[0];//+p1[1]*tempydir[1]+p1[2]*tempydir[2];
			p3[0]+=basept[0];
			p3[1]+=basept[1];
		}else{
			ic_ptcpy(p1,basept);
			p1[0]+=(leglen+textsize)*scale;
			gr_ucs2rp(p1,p3,view);
		}
		p3[0]-=(0.4*textsize*scale);
		p4[0]=p3[0]+0.8*textsize*scale;
		p3[1]-=(0.5*textsize*scale);
		p4[1]=p3[1]+textsize*scale;
		gr_rp2ucs(p3,p1,view);
		gr_rp2ucs(p4,p2,view);
		drawElement(p1,p2,xcolor,NULL,NULL,NULL,pDrawDevice);
		p4[1]=p3[1];
		p3[1]+=(textsize*scale);
		gr_rp2ucs(p3,p1,view);
		gr_rp2ucs(p4,p2,view);
		drawElement(p1,p2,xcolor,NULL,NULL,NULL,pDrawDevice);
	}
	if((omitlet&2)==0){
		//draw the y
		if(frp1!=NULL){
			p2[1]=(leglen+textsize)*scale;
			p3[0]=p2[1]*tempxdir[1];
			p3[1]=p2[1]*tempydir[1];
			p3[0]+=basept[0];
			p3[1]+=basept[1];
		}else{
			ic_ptcpy(p1,basept);
			p1[1]+=(leglen+textsize)*scale;
			gr_ucs2rp(p1,p3,view);
		}
		p4[0]=p3[0]+0.4*textsize*scale;
		p4[1]=p3[1]+0.5*textsize*scale;
		gr_rp2ucs(p4,p2,view);
		gr_rp2ucs(p3,p1,view);
		drawElement(p1,p2,ycolor,NULL,NULL,NULL,pDrawDevice);
		p4[0]-=(0.8*textsize*scale);
		gr_rp2ucs(p4,p2,view);
		drawElement(p1,p2,ycolor,NULL,NULL,NULL,pDrawDevice);
		p4[0]=p3[0];
		p4[1]=p3[1]-0.5*textsize*scale;
		gr_rp2ucs(p4,p2,view);
		drawElement(p1,p2,ycolor,NULL,NULL,NULL,pDrawDevice);
	}
	if((omitlet&4)==0){
		//draw the z
		if(frp1!=NULL){
			p2[2]=(leglen+textsize)*scale;
			p3[0]=p2[2]*tempxdir[2];
			p3[1]=p2[2]*tempydir[2];
			p3[0]+=basept[0];
			p3[1]+=basept[1];
		}else{
			ic_ptcpy(p1,basept);
			p1[2]+=(leglen+textsize)*scale;
			gr_ucs2rp(p1,p3,view);
		}
		p3[0]-=(0.4*textsize*scale);
		p3[1]+=(0.5*textsize*scale);
		p4[0]=p3[0]+0.8*textsize*scale;
		p4[1]=p3[1];
		gr_rp2ucs(p3,p1,view);
		gr_rp2ucs(p4,p2,view);
		drawElement(p1,p2,zcolor,NULL,NULL,NULL,pDrawDevice);
		p3[1]-=(textsize*scale);
		gr_rp2ucs(p3,p1,view);
		drawElement(p1,p2,zcolor,NULL,NULL,NULL,pDrawDevice);
		p4[1]=p3[1];
		gr_rp2ucs(p4,p2,view);
		drawElement(p1,p2,zcolor,NULL,NULL,NULL,pDrawDevice);
	}
	out: ;

	return(ret);
}

int CUCSIcon::drawAlternate(const sds_real* scale, const sds_point rpadd, const sds_point viewdir, int unpaint, CDrawDevice* pDrawDevice)
{
	ASSERT(m_pView != NULL);
	return SDS_DrawAltUcsIcon(pDrawDevice, scale, rpadd, viewdir, m_pView, unpaint);
}

int CUCSIcon::drawElement(sds_point frompt, sds_point topt, int color, sds_point basept, sds_point tempxdir, sds_point tempydir, CDrawDevice* pDrawDevice)
{
	ASSERT(m_pView != NULL);
	return SDS_UcsGrDraw(pDrawDevice, frompt, topt, color, basept, tempxdir, tempydir, m_pView);
}
