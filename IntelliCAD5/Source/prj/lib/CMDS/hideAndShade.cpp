/* \PRJ\LIB\CMDS\HIDEANDSHADE.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 */
// THIS FILE HAS BEEN GLOBALIZED!

#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/
#include "IcadView.h"
#include "HideAndShade.h"
#include "viewport.h"

#include <GL/gl.h>
#include <GL/glu.h>


////////////////////////////////////////////////////////////////////////////////////
// CHLRenderer

CHLRenderer::CHLRenderer(CIcadView* pView) : m_pView(pView), m_ImageType((ImageType)pView->m_iViewMode)
{
	SDS_ProgressStart();
	resbuf rb;
	SDS_getvar(NULL, DB_QTILEMODE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	m_tilemode = rb.resval.rint;
	ComputeImageBound();
	if(!m_tilemode)
	{
		short	renderStatus = 0;
		((db_viewport*)m_pView->m_pVportTabHip)->get_281(&renderStatus);
		m_ImageType = renderStatus == IC_RENDERMODE_HIDDENLINE ? kHiddenLine : kShaded;
	}
}

CHLRenderer::~CHLRenderer()
{
	SDS_ProgressPercent(100);
	SDS_ProgressStop();
}


void CHLRenderer::setGlViewport()
{
	if(m_pView->IsPrinting() && m_tilemode)
		glViewport(m_Bound.left, m_Bound.top, m_Bound.right - m_Bound.left, m_Bound.bottom - m_Bound.top);
	else
		glViewport(m_Bound.left, m_pView->m_iWinY - m_Bound.bottom, m_Bound.right - m_Bound.left, m_Bound.bottom - m_Bound.top);
}

#ifdef _USE_DISP_OBJ_PTS_3D_
//#pragma message("_USE_DISP_OBJ_PTS_3D_ defined in hideAndShade.cpp")
/*-------------------------------------------------------------------------*//**
Redesigned implementation of HIDE&SHADE

@author Denis Petrov
@return RTNORM
*//*------------------------------------------------------------------------*/
int CHLRenderer::CreateDisplayList(UINT nList)
{
	struct resbuf rb;
	SDS_getvar(NULL, DB_QSHADEDGE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	int shadedge = rb.resval.rint;

	SDS_getvar(NULL, DB_QSPLFRAME, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
	int splframe = rb.resval.rint;
	if(splframe == 0)
		SDS_CURGRVW->splframe = 1;

	long nentsindwg = SDS_CURDWG->ret_nmainents();
	long entcount = 0L, savprog = 0L, prog;
	SDS_ProgressStart();

	int psflag, plineflag;
	bool bIsClosed, bIsPline, bIsEdgeOn;
	struct gr_displayobject *pEntityDOListBegin = NULL, *pEntityDOListEnd = NULL, *pCurDO;
    db_handitem* pEntity;
    for(pEntity = SDS_CURDWG->entnext_noxref(NULL); pEntity != NULL; pEntity = SDS_CURDWG->entnext_noxref(pEntity)) 
	{
		pEntity->get_67(&psflag);
		if(psflag) 
			continue;

		if(splframe == 0)
		{
			db_handitem* pEntity4 = pEntity;
			if(SDS_getdispobjs(SDS_CURDWG, 
				&pEntity4,
				&pEntityDOListBegin,
				&pEntityDOListEnd,
				NULL,
				SDS_CURDWG,
				SDS_CURGRVW,
				0,
				NULL,
				true))	/*D.G.*/// 'true' to get mesh surface disp. objs for ACIS entities.
				break;
		}
		else
			pEntityDOListBegin = (struct gr_displayobject*)pEntity->ret_disp();

		if(!db_is_subentitytype(pEntity->ret_type()))
		{
			entcount++;
			prog = (int)(((double)entcount / nentsindwg) * 95);  // *95 because we need time for GL to render.
			if(savprog + 5 <= prog)
				SDS_ProgressPercent(savprog = prog);
		}

		if (pEntityDOListBegin == NULL)
			continue;

		bIsPline = bIsEdgeOn = false;
		plineflag = 0;
		sds_real thickness = 0.0;
		pEntity->get_39(&thickness);
		if(fabs(thickness) < CMD_FUZZ)
		{
			if(pEntity->ret_type() == DB_POLYLINE)
			{
				if(pEntity->get_70(&plineflag))
					if((plineflag & (IC_PLINE_POLYFACEMESH | IC_PLINE_3DMESH)) == 0)
						//flag if we're doing a pline w/o thickness
						bIsPline = true;
			}
			else
				if(pEntity->ret_type() == DB_LWPOLYLINE)
					bIsPline = true;
		}
		if(fabs(thickness) > CMD_FUZZ)
		{
			//check if entity's extru is parallel to the current viewdir and we're
			//shading; if so, make sure the edges only are drawn
			sds_point p1, p2, p3;
			struct resbuf rbucs, rbwcs;
			rbucs.restype = RTSHORT;
			rbucs.resval.rint = 1;
			rbwcs.restype = RTSHORT;
			rbwcs.resval.rint = 0;
			pEntity->get_210(p1);
			SDS_getvar(NULL, DB_QVIEWDIR, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
			sds_trans(rb.resval.rpoint, &rbucs, &rbwcs, 1, p2);
			//cross 210 and viewdir to see if we get the zero vector - edge on view
			ic_crossproduct(p1, p2, p3);
			if((p3[0] * p3[0] + p3[1] * p3[1] + p3[2] * p3[2]) < CMD_FUZZ)
				bIsEdgeOn = true;
		}

		//loop for polygons
		for(pCurDO = pEntityDOListBegin; pCurDO != NULL; pCurDO = pCurDO->next) 
		{
			if(pCurDO->chain == NULL || !(pCurDO->type & DISP_OBJ_PTS_3D) || pCurDO->type & DISP_OBJ_SPECIAL)
				continue;
			bIsClosed = (pCurDO->type & DISP_OBJ_PTS_CLOSED) != 0;

			if (m_ImageType == kHiddenLine || shadedge == 2)
				::glIndexi(GetBackgroundColorIndex());
			else
				::glIndexi(GetDrawingColorIndex(pCurDO->color - 1));

			if(!bIsEdgeOn && !bIsPline) 
			{
				::glPolygonMode(GL_FRONT_AND_BACK, bIsClosed ? GL_FILL : GL_LINE);
				::glVertexPointer(3, GL_DOUBLE, 0, (GLvoid*)pCurDO->chain);
				::glDrawArrays(bIsClosed ? GL_POLYGON : GL_LINE_STRIP, 0, pCurDO->npts);
			}
		}

		if(splframe == 0)
			gr_freedisplayobjectll(pEntityDOListBegin);
		// loop for edges
		if(shadedge > 0 || m_ImageType == kHiddenLine)
		{

			for(pCurDO = (struct gr_displayobject*)pEntity->ret_disp(); pCurDO != NULL; pCurDO = pCurDO->next) 
			{
				if(pCurDO->chain == NULL || !(pCurDO->type & DISP_OBJ_PTS_3D) || pCurDO->type & DISP_OBJ_SPECIAL)
					continue;
				bIsClosed = (pCurDO->type & DISP_OBJ_PTS_CLOSED) != 0;
				
				if(!bIsEdgeOn && !bIsPline && m_ImageType != kHiddenLine && shadedge & 0x01)
					glIndexi(GetBackgroundColorIndex());
				else
					glIndexi(GetDrawingColorIndex(pCurDO->color - 1));
				
				if(m_ImageType == kHiddenLine || bIsPline || bIsEdgeOn || shadedge > 0) 
				{
					::glVertexPointer(3, GL_DOUBLE, 0, (GLvoid*)pCurDO->chain);
					::glDrawArrays(bIsClosed ? GL_LINE_LOOP : GL_LINE_STRIP, 0, pCurDO->npts);
				}
			}
		}
	}
	if(splframe == 0)
		SDS_CURGRVW->splframe = 1;
	::glFinish();
	return RTNORM;
}

#else // _USE_DISP_OBJ_PTS_3D_

int CHLRenderer::CreateDisplayList(UINT nList)
{
	int psflag,plineflag;
	sds_point p1,p2,p3;
	struct resbuf rb,rbucs,rbwcs;
	sds_real fr1;
	struct gr_displayobject *beg=NULL,*end=NULL,*tdo1;
	struct gr_displayobject *beg2=NULL,*end2=NULL;
	int face,ndims;
    db_handitem *telp1,*telp;
	long fl2;

	SDS_getvar(NULL,DB_QSHADEDGE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	int shadedge=rb.resval.rint;

//	::glNewList(nList, GL_COMPILE_AND_EXECUTE);

	//glPushMatrix();

	long nentsindwg=SDS_CURDWG->ret_nmainents();
	long entcount=0L,savprog=0L,prog;
	bool ispline,edge_on;

	// Build a new list from the whole drawing.
    for (telp1= SDS_CURDWG->entnext_noxref(NULL);
	    telp1!=NULL;
		telp1=SDS_CURDWG->entnext_noxref(telp1)) {

		telp=telp1;

		telp1->get_67(&psflag);
		if(psflag) continue;

		int polylineFlags = 0;
		int splframe = -1;
		//resbuf sf;
		if (telp1->ret_type() == DB_POLYLINE) {
			// to get display objects of POLYLINEs with invisible edges
			telp1->get_70(&polylineFlags);
			if(polylineFlags & IC_PLINE_POLYFACEMESH) {
				//SDS_getvar(NULL, DB_QSPLFRAME, &sf, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
				//splframe = sf.resval.rint;
				//sf.resval.rint = 1;
				//SDS_setvar(NULL, DB_QSPLFRAME, &sf, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 0);
				splframe = SDS_CURGRVW->splframe;
				SDS_CURGRVW->splframe = 1;
			}
		}
		else
			if (telp1->ret_type() == DB_3DFACE) 
			{
				splframe = SDS_CURGRVW->splframe;
				SDS_CURGRVW->splframe = 1;
			}
		if (SDS_getdispobjs(SDS_CURDWG,
						   &telp,
						   &beg,
						   &end,
						   NULL,
						   SDS_CURDWG,
						   SDS_CURGRVW,
						   0,
						   NULL,
						   true))	/*D.G.*/// 'true' to get mesh surface disp. objs for ACIS entities.
			break;

		//if (polylineFlags & IC_PLINE_POLYFACEMESH) 
		if(splframe != -1)
		{
			//sf.resval.rint = splframe;
			//SDS_setvar(NULL, DB_QSPLFRAME, &sf, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES, 0);
			SDS_CURGRVW->splframe = splframe;
		}
		beg2 = end2 = NULL;
		if(m_ImageType == kHiddenLine && telp1->ret_type() == DB_POLYLINE && telp1->get_70(&plineflag) &&	plineflag&(16 | 64) // a 3D polygon mesh or a polyface mesh
			|| telp1->ret_type() == DB_3DFACE || telp1->ret_type() == DB_INSERT)
		{	// invisible edges should not be drawn. And so we get this call again
			telp=telp1;
			SDS_getdispobjs(SDS_CURDWG,
						   &telp,
						   &beg2,
						   &end2,
						   NULL,
						   SDS_CURDWG,
						   SDS_CURGRVW,
						   2,
						   NULL,
						   true);
		}

		if (!db_is_subentitytype(telp1->ret_type())) {
			entcount++;
			prog=(int)(((double)entcount/nentsindwg)*95);  // *95 because we need time for GL to render.
			if(savprog+5<=prog) SDS_ProgressPercent(savprog=prog);
		}

		if (beg==NULL ||end==NULL) continue;
		ispline=edge_on=FALSE;
		plineflag=0;
		telp1->get_39(&fr1);
		if(fabs(fr1)<CMD_FUZZ && DB_POLYLINE==telp1->ret_type()){
			if(telp1->get_70(&plineflag))//MHB;
			if(0==(plineflag&(16+64)))ispline=TRUE;//flag if we're doing a pline w/o thickness
		}
		if(fabs(fr1)>CMD_FUZZ){
			//check if entity's extru is parallel to the current viewdir and we're
			//	shading.  if so, make sure the edges only are drawn
			telp1->get_210(p1);
			if(SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
			sds_trans(rb.resval.rpoint,&rbucs,&rbwcs,1,p2);
			fr1=p2[0]*p2[0]+p2[1]*p2[1]+p2[2]*p2[2];
			fr1*=CMD_FUZZ;
			//cross 210 and viewdir to see if we get the zero vector - edge on view
			ic_crossproduct(p1,p2,p3);
			if((p3[0]*p3[0]+p3[1]*p3[1]+p3[2]*p3[2])<CMD_FUZZ)edge_on=TRUE;
		}
		for (tdo1=beg; tdo1!=NULL; tdo1=tdo1->next) {
			if(tdo1->chain == NULL || tdo1->type & DISP_OBJ_SPECIAL) 
				continue;
			ndims=((tdo1->type&0x01)!=0)+2;
			face=((tdo1->type&0x02)!=0);
			if(ndims!=3) continue;

			if (m_ImageType == kHiddenLine)
				::glIndexi(GetBackgroundColorIndex());
			else
				::glIndexi(GetDrawingColorIndex(tdo1->color-1));
			//loop for fill
			if(!edge_on && !ispline && (m_ImageType == kHiddenLine || 2!=shadedge)) {
				if(!face || tdo1->npts>4){
					::glPolygonMode(GL_FRONT_AND_BACK,(!face) ? GL_LINE : GL_FILL);
					if(face && m_ImageType == kShaded)
						::glBegin(GL_POLYGON);
					else
						::glBegin((face) ? GL_LINE_LOOP : GL_LINE_STRIP);

					for(fl2=0;fl2<tdo1->npts;fl2++){
						::glVertex3d(tdo1->chain[fl2*3],tdo1->chain[(fl2*3)+1],tdo1->chain[(fl2*3)+2]);
					}
					::glEnd();
				}
				else {
					::glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					if(tdo1->npts==4)
						::glBegin(GL_QUADS);
					else
						::glBegin(GL_TRIANGLES);

					::glVertex3d(tdo1->chain[0],tdo1->chain[1],tdo1->chain[2]);
					::glVertex3d(tdo1->chain[3],tdo1->chain[4],tdo1->chain[5]);
					::glVertex3d(tdo1->chain[6],tdo1->chain[7],tdo1->chain[8]);

					if(tdo1->npts==4)
						::glVertex3d(tdo1->chain[9],tdo1->chain[10],tdo1->chain[11]);

					::glEnd();
				}
			}
			if (beg2)
				continue;

			//loop for edges
			if((shadedge>0)|| m_ImageType == kHiddenLine){
				::glIndexi(GetDrawingColorIndex(tdo1->color-1));
				if(!edge_on && !ispline && m_ImageType == kShaded && (shadedge&1)){
					glIndexi(GetBackgroundColorIndex());	//need to set to actual background color
				}

#ifdef THIN_LINES
				if(!isOpenGLv11orHigher())
#endif
					if(m_ImageType == kHiddenLine)
						::glLineWidth((float)2.0);
					else
						::glLineWidth((float)1.0);

				if(!face || tdo1->npts>4){
					if(m_ImageType == kHiddenLine || ispline || edge_on) {
						if(tdo1->npts>4 && m_ImageType == kShaded && (plineflag&1)) {
							::glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
							::glBegin(GL_POLYGON);
						} else {
							::glBegin((face || (plineflag&1)) ? GL_LINE_LOOP : GL_LINE_STRIP);
						}
						for(fl2=0;fl2<tdo1->npts;fl2++){
							::glVertex3d(tdo1->chain[fl2*3],tdo1->chain[(3*fl2)+1],tdo1->chain[(3*fl2)+2]);
						}
						::glEnd();
					}
				}
				else {
					::glBegin(GL_LINE_LOOP);
					::glVertex3d(tdo1->chain[0],tdo1->chain[1],tdo1->chain[2]);
					::glVertex3d(tdo1->chain[3],tdo1->chain[4],tdo1->chain[5]);
					::glVertex3d(tdo1->chain[6],tdo1->chain[7],tdo1->chain[8]);
					if(tdo1->npts==4)
						::glVertex3d(tdo1->chain[9],tdo1->chain[10],tdo1->chain[11]);
					::glEnd();
				}
			}
		}
		gr_freedisplayobjectll(beg);
		if(beg2)
		{	
			// loop for edges. invisible edges will not be drawn!
			for (tdo1=beg2; tdo1!=NULL; tdo1=tdo1->next) 
			{
				if(tdo1->chain == NULL || tdo1->type & DISP_OBJ_SPECIAL) 
					continue;
				ndims=((tdo1->type&0x01)!=0)+2;
				face=((tdo1->type&0x02)!=0);
				if(ndims!=3) 
					continue;

				if((shadedge>0)||m_ImageType == kHiddenLine)
				{
					if(!edge_on && !ispline && m_ImageType != kHiddenLine && (shadedge&1))
						glIndexi(GetBackgroundColorIndex());	//need to set to actual background color
					else
						glIndexi(GetDrawingColorIndex(tdo1->color-1));

#ifdef THIN_LINES
					if(!isOpenGLv11orHigher())
#endif
						if(m_ImageType == kHiddenLine)
							glLineWidth(2.0f);
						else
							glLineWidth(1.0f);

	//				if(!face) glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
					if(!face || tdo1->npts>4)
					{
						if(m_ImageType == kHiddenLine || ispline || edge_on || shadedge>0) 
						{
							/* next strings are commented because Dome polygon mesh has plineflag&1 
								and drawn as GL_LINE_LOOP. It is incorrect.
							if(tdo1->npts>4 && !hidemode && (plineflag&1)) {
								glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
								glBegin(GL_POLYGON);
							} else {
								glBegin((face || (plineflag&1)) ? GL_LINE_LOOP : GL_LINE_STRIP);
							} */
							glBegin((face) ? GL_LINE_LOOP : GL_LINE_STRIP);
							for(fl2=0;fl2<tdo1->npts;fl2++){
								glVertex3d(tdo1->chain[fl2*3],tdo1->chain[(3*fl2)+1],tdo1->chain[(3*fl2)+2]);
							}
							glEnd();
						}
					}else{
						glBegin(GL_LINE_LOOP);
						glVertex3d(tdo1->chain[0],tdo1->chain[1],tdo1->chain[2]);
						glVertex3d(tdo1->chain[3],tdo1->chain[4],tdo1->chain[5]);
						glVertex3d(tdo1->chain[6],tdo1->chain[7],tdo1->chain[8]);
						if(tdo1->npts==4)
							glVertex3d(tdo1->chain[9],tdo1->chain[10],tdo1->chain[11]);
						glEnd();
					}
				}	
			}
			gr_freedisplayobjectll(beg2);
		}
	}
	::glEndList();
	::glFinish();
	return RTNORM;
}
#endif // _USE_DISP_OBJ_PTS_3D_


bool CHLRenderer::SetDCPixelFormat(HDC hDC, DWORD dwFlags)
{
	PIXELFORMATDESCRIPTOR pfd = {	sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
									1,                              // version number
									dwFlags,
									PFD_TYPE_COLORINDEX,			// INDEX mode (temporary)
									24,                             // 24-bit color depth
									0, 0, 0, 0, 0, 0,               // color bits ignored
									0,                              // no alpha buffer
									0,                              // shift bit ignored
									0,                              // no accumulation buffer
									0, 0, 0, 0,                     // accum bits ignored
									32,                             // 32-bit z-buffer
									0,                              // no stencil buffer
									0,                              // no auxiliary buffer
									PFD_MAIN_PLANE,                 // main layer
									0,                              // reserved
									0, 0, 0
								};

	int nPixelIndex = ::ChoosePixelFormat(hDC, &pfd);
	if (nPixelIndex == 0) {// Choose default
		nPixelIndex = 1;
		if (::DescribePixelFormat(hDC, nPixelIndex, sizeof(PIXELFORMATDESCRIPTOR), &pfd) == 0)
			return false;
	}

	if (!::SetPixelFormat(hDC, nPixelIndex, &pfd))
		return false;

	return true;
}


int CHLRenderer::SetOpenGLState()
{
	sds_point viewdir,target,p1,p2,p3;
	sds_real vsize,cameradist,maxdist,fr1,fr2,fr3;
	struct resbuf rb,rbucs,rbwcs;
	int vmode;

	rbucs.restype=rbwcs.restype=RTSHORT;
	rbucs.resval.rint=1;
	rbwcs.resval.rint=0;

	SDS_getvar(NULL,DB_QVIEWMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	vmode=rb.resval.rint;
	SDS_getvar(NULL,DB_QVIEWDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	//sds_trans(viewdir,&rbucs,&rbwcs,1,viewdir);
	ic_ptcpy(viewdir,rb.resval.rpoint);
	if(vmode&1){
		SDS_getvar(NULL,DB_QTARGET,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	}else{
		SDS_getvar(NULL,DB_QVIEWCTR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	}
	ic_ptcpy(target,rb.resval.rpoint);
	//sds_trans(rb.resval.rpoint,&rbucs,&rbwcs,0,target);
	SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	vsize=rb.resval.rreal;
	//calc dist betw extmin & extmax
	SDS_getvar(NULL,DB_QEXTMIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(p1,rb.resval.rpoint);
	SDS_getvar(NULL,DB_QEXTMAX,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(p2,rb.resval.rpoint);
	//put target within extents cube.  first move z to midpt of cube z
	p3[2]=(p1[2]+p2[2])*0.5;
	//move target allong viewdir vector until it's at cube's Z midpoint
	cmd_elev_correct(p3[2],target,1,NULL);
	//now push out extents in X & Y directions
	if(target[0]<p1[0])p1[0]=target[0];
	if(target[0]>p2[0])p2[0]=target[0];
	if(target[1]<p1[1])p1[1]=target[1];
	if(target[1]>p2[1])p2[1]=target[1];
	//get max distance we'll need for extents (diagonal of cube)
	maxdist=sds_distance(p1,p2);
	p3[0]=(p1[0]+p2[0])*0.5;
	p3[1]=(p1[1]+p2[1])*0.5;
	cameradist=sqrt(viewdir[0]*viewdir[0]+viewdir[1]*viewdir[1]+viewdir[2]*viewdir[2]);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(vmode&1){	//perspective projection
		SDS_getvar(NULL,DB_QLENSLENGTH,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		if((fr1=rb.resval.rreal)<=0.0){
//			ReleaseDC(m_pView->m_hWnd,m_ghDC);
			return(RTERROR);
		}
		//fr3 is ratio of height to length of view.
		fr3=(17.5/fr1);	//assume 35mm film height
		fr3*=180.0/IC_PI;
		if((vmode&18)==18){
			SDS_getvar(NULL,DB_QFRONTZ,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			fr1=cameradist-rb.resval.rreal;
		}else{
			fr1=1.0e-5;//just set front clip real real close....
		}
		if(vmode&4){
			SDS_getvar(NULL,DB_QBACKZ,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			fr2=cameradist-rb.resval.rreal;
		}else{
			fr2=maxdist+cameradist;//just set back clip way way back....
		}
		gluPerspective(fr3,(m_Bound.right-m_Bound.left)/(m_Bound.bottom-m_Bound.top),fr1,fr2);	/*DG - 31.1.2003*/// m_Bound.left/top can be non zero (when m_Bound has got with SDS_GetRectForVP), so take them in account
	}else{			//orthographic projection
		glOrtho((-0.5*vsize*(m_Bound.right-m_Bound.left)/(m_Bound.bottom-m_Bound.top)),	/*DG - 31.1.2003*/// m_Bound.left/top can be non zero (when m_Bound has got with SDS_GetRectForVP),
				(0.5*vsize*(m_Bound.right-m_Bound.left)/(m_Bound.bottom-m_Bound.top)),	// so take them in account
				(-0.5*vsize),
				(0.5*vsize),
				0.0,	//near clip plane
				maxdist+2.0*(maxdist+cameradist));//far clip plane
				//NOTE: far clip plane dist depends on camera movement below
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	setGlViewport();

	glClearIndex((GLfloat)GetBackgroundColorIndex());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	HPALETTE oldpal=SelectPalette(GetTargetDC(),(HPALETTE)SDS_CMainWindow->m_pPalette->GetSafeHandle(),TRUE);

	//glFlush();
	//***********************************
	//Note: in GL, when you move the camera, the axes move w/it so that the camera is
	//	always plan view on the GL coordinate system.
	//Always apply translations & rotations in reverse order!
	//***********************************

	//fifth, offset the camera by the correct distance from the origin(target)
	if(vmode&1){
		glTranslated(0,0,-cameradist);
	}else{
		//camera need not be within extents cube, but target is
		glTranslated(0,0,-2.0*(maxdist+cameradist));
	}
	//fourth, twist the view
	if(!icadRealEqual(viewdir[0],0.0) || !icadRealEqual(viewdir[1],0.0)){
		rbwcs.resval.rint=2;//dcs
		p1[0]=p1[1]=0.0;
		p1[2]=1.0;
		sds_trans(p1,&rbucs,&rbwcs,1,p1);
		rbwcs.resval.rint=0;
		fr2=atan2(p1[1],p1[0]);
		fr2*=180.0/IC_PI;
		fr2-=90.0;  //measure from 12 o'clock, not 3 o'clock
		glRotated(fr2,0,0,1);
		//Do NOT rotate around viewdir.  To apply view twist, GL wants a twist around the CURRENT
		//	orientation of the Z axis, not around the axis as it was originally defined!!!
	}

	//third, rotate camera down around newly oriented X axis to correct angle above/below X-Y plane from plan view
	fr2=sqrt(viewdir[0]*viewdir[0]+viewdir[1]*viewdir[1]);
	fr1=atan2(viewdir[2],fr2);
	fr1*=180.0/IC_PI;
	glRotated((90.0-fr1),-1,0,0);
	//second, rotate around z-axis in XY plane
	if(icadRealEqual(viewdir[0],0.0) && icadRealEqual(viewdir[1],0.0)){
		rbwcs.resval.rint=2;//dcs
		p1[1]=p1[2]=0.0;
		p1[0]=1.0;
		sds_trans(p1,&rbucs,&rbwcs,1,p1);
		rbwcs.resval.rint=0;//back to wcs
		fr2=atan2(p1[1],p1[0]);
		fr2*=180.0/IC_PI;
	}else{
		fr2=atan2(viewdir[1],viewdir[0]);
		fr2*=180.0/IC_PI;
		fr2+=90.0;
	}
	glRotated(fr2,0,0,-1);

	//first, translate everything so that target (viewctr) is at origin
	glTranslated(-target[0],-target[1],-target[2]);

#ifdef THIN_LINES
	if(isOpenGLv11orHigher())
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0f, 1.0f);
		glLineWidth(1.0f);
	}
#endif
	glShadeModel(GL_SMOOTH);
	glEnableClientState(GL_VERTEX_ARRAY);

	return RTNORM;
}


void CHLRenderer::ComputeImageBound()
{
	if(m_tilemode)
	{
		m_Bound.left = m_Bound.top = 0L;
		m_Bound.right = m_pView->m_iWinX;
		m_Bound.bottom = m_pView->m_iWinY;
	}
	else
	{
		int	left, right, top, bottom;
		SDS_GetRectForVP(m_pView, m_pView->m_pVportTabHip, &left, &right, &top, &bottom);
		m_Bound.left = left;
		m_Bound.right = right;
		m_Bound.top = top;
		m_Bound.bottom = bottom;
	}
}


#ifdef THIN_LINES
/*-------------------------------------------------------------------------*//**
This function checks OpenGL version at run-time.

@author	Dmitry Gavrilov
@return 'true' if OpenGL is of version 1.1 or higher, otherwise 'false'
*//*--------------------------------------------------------------------------*/
bool CHLRenderer::isOpenGLv11orHigher()
{
	static bool	b11orHigher = false,
				bCheckedOnce = false;

	if(!bCheckedOnce)
	{
		char* pVer = (char*)glGetString(GL_VERSION);
		b11orHigher = !!strncmp(pVer, "1.0"/*DNT*/, 3);	// I hope we never use version less than 1.0
		bCheckedOnce = true;
	}

	return b11orHigher;
}
#endif


////////////////////////////////////////////////////////////////////////////////////
// CWindowHLRenderer

CWindowHLRenderer::CWindowHLRenderer(CIcadView *pView) : CHLRenderer(pView), m_ghDC(NULL), m_hRC(NULL)
{
}


CWindowHLRenderer::~CWindowHLRenderer()
{
	if (m_hRC) {
		wglDeleteContext (m_hRC);
		m_hRC = NULL;
	}
	if (m_ghDC) {
		ReleaseDC(m_pView->m_hWnd, m_ghDC);
		m_ghDC = NULL;
	}
}


bool CWindowHLRenderer::Initialize()
{
	ASSERT(m_pView);

	SDS_SetUndo(IC_UNDO_REDRAW_VP_ON_UNDO,NULL,NULL,NULL,SDS_CURDWG);
	SDS_SetUndo(m_ImageType == CHLRenderer::kHiddenLine ?
				IC_UNDO_HIDE_ON_REDO :
				IC_UNDO_SHADE_ON_REDO,NULL,NULL,NULL,SDS_CURDWG);

    m_ghDC = GetDC(m_pView->m_hWnd);

    ASSERT(m_ghDC);

	if (!SetDCPixelFormat(m_ghDC, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER)) {
		ReleaseDC(m_pView->m_hWnd, m_ghDC);
		m_ghDC = NULL;
		cmd_ErrorPrompt(CMD_ERR_OPENGLCHOOSE,0);
		return false;
	}

	m_hRC = ::wglCreateContext(m_ghDC);
	ASSERT(m_hRC);
	if (!::wglMakeCurrent(m_ghDC, m_hRC))
		return false;

	return true;
}


int CWindowHLRenderer::render()
{
	if (!Initialize())
		return RTERROR;

	if (SetOpenGLState() == RTERROR || CreateDisplayList(1) == RTERROR) {
		if (m_hRC) {
			wglDeleteContext (m_hRC);
			m_hRC = NULL;
		}
		if (m_ghDC) {
			ReleaseDC(m_pView->m_hWnd, m_ghDC);
			m_ghDC = NULL;
		}
		return RTERROR;
	}
	return RenderSense();
}


int CWindowHLRenderer::RenderSense()
{
	SwapBuffers(m_ghDC);
	wglMakeCurrent (NULL, NULL) ;
	wglDeleteContext (m_hRC);
	m_hRC = NULL;

	// ************************************
	// Blit from on screen DC to off-screen.
	// NOTE: this is the reverse of what we usually do, but OpenGL
	// will not work directly in the offscreen bitmap
	//
	CDC *cdc = m_pView->GetDC();
	CDC *Wincdc = m_pView->GetFrameBufferCDC();
	CRect Rect;
	m_pView->GetClientRect(&Rect);
	BitBlt(Wincdc->m_hDC,0,0,Rect.Width(),Rect.Height(),cdc->m_hDC,0,0,SRCCOPY);


	m_pView->ReleaseDC(cdc);

	ReleaseDC(m_pView->m_hWnd, m_ghDC);
	m_ghDC = NULL;

	/*DG - 31.1.2003*/// draw other viewports too when tilemode is off
	if(!m_tilemode)
		SDS_BitBlt2View(0, m_pView);

    return(RTNORM);
}


////////////////////////////////////////////////////////////////////////////////////
// CBitmapHLRenderer

CBitmapHLRenderer::CBitmapHLRenderer(CIcadView *pView, CDC* pDC, CPrintInfo* pInfo /*= NULL*/)
						: CHLRenderer(pView),
						 m_pDC(pDC), m_pPrintInfo(pInfo), m_hDib(NULL), m_hOldDC(NULL), m_hMemDC(NULL), m_hOldRC(NULL),
						 m_hMemRC(NULL), m_hOldDib(NULL), m_pBitmapBits(NULL)
{
}


CBitmapHLRenderer::~CBitmapHLRenderer()
{
	Cleanup();
}


void CBitmapHLRenderer::setGlViewport()
{
	if(m_pPrintInfo && m_tilemode)
		glViewport(m_Bound.left, m_Bound.top, m_Bound.right - m_Bound.left, m_Bound.bottom - m_Bound.top);
	else
	{
		if(m_pPrintInfo)
			glViewport(m_Bound.left, m_pView->m_iWinY - m_Bound.bottom, m_Bound.right - m_Bound.left, m_Bound.bottom - m_Bound.top);
		else
			glViewport(0, 0, m_Bound.right - m_Bound.left, m_Bound.bottom - m_Bound.top);
	}
}


bool CBitmapHLRenderer::Initialize()
{
	long	width = m_Bound.right - m_Bound.left,
			height = m_Bound.bottom - m_Bound.top;

	if(m_pPrintInfo)
	{
		// Get page size
		m_szPage.cx  = m_pDC->GetDeviceCaps(HORZRES);
		m_szPage.cy = m_pDC->GetDeviceCaps(VERTRES);

		CSize szDIB;
		if (m_pPrintInfo->m_bPreview)
		{
			// Use screen resolution for preview.
			szDIB.cx = width;
			szDIB.cy = height;
		}
		else
		{	// Printing
			// Use higher resolution for printing.
			// Adjust size according screen's ratio.
			double fClientRatio = ((double)height) / width;
			if (m_szPage.cy > fClientRatio*m_szPage.cx)
			{
				// View area is wider than Printer area
				szDIB.cx = m_szPage.cx;
				szDIB.cy = long(fClientRatio*m_szPage.cx);
			}
			else
			{
				// View area is narrower than Printer area
				szDIB.cx = long(double(m_szPage.cy)/fClientRatio);
				szDIB.cy = m_szPage.cy;
			}
			// Reduce the Resolution if the Bitmap size is too big.
			// Ajdust the maximum value, which is depends on printer's memory.
			// I use 20 MB.
			while (szDIB.cx*szDIB.cy > 20e6)
			{
				szDIB.cx = szDIB.cx/2;
				szDIB.cy = szDIB.cy/2;
			}
			m_Bound.right = m_Bound.left + szDIB.cx;
			m_Bound.bottom = m_Bound.top + szDIB.cy;

			width = m_Bound.right - m_Bound.left;
			height = m_Bound.bottom - m_Bound.top;
		}
	}

	TRACE("Buffer size: %d x %d = %6.2f MB\n", width, height, width * height * 0.000001);

	// Initialize the bitmap header info.
	memset(&m_bmi, 0, sizeof(BITMAPINFO));
	m_bmi.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	m_bmi.bmiHeader.biWidth			= width;
	m_bmi.bmiHeader.biHeight		= height;
	m_bmi.bmiHeader.biPlanes		= 1;
	m_bmi.bmiHeader.biBitCount		= 24;
	m_bmi.bmiHeader.biCompression	= BI_RGB;
	m_bmi.bmiHeader.biSizeImage		= width * height * 3;

	// Create DIB
	m_hDib = CreateDIBSection(m_pDC->GetSafeHdc(), &m_bmi, DIB_RGB_COLORS, &m_pBitmapBits, NULL, (DWORD)0);

	// Create memory DC
	m_hMemDC = ::CreateCompatibleDC(NULL);
	if (!m_hMemDC) {
		DeleteObject(m_hDib);
		m_hDib = NULL;
		return false;
	}

	m_hOldDib = SelectObject(m_hMemDC, m_hDib);

	// Setup memory DC's pixel format.
	if (!SetDCPixelFormat(m_hMemDC, PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL | PFD_STEREO_DONTCARE)) {
		SelectObject(m_hMemDC, m_hOldDib);
		DeleteObject(m_hDib);
		m_hDib = NULL;
		DeleteDC(m_hMemDC);
		m_hMemDC = NULL;
		return false;
	}

	// Create memory RC
	m_hMemRC = ::wglCreateContext(m_hMemDC);
	if (!m_hMemRC) {
		SelectObject(m_hMemDC, m_hOldDib);
		DeleteObject(m_hDib);
		m_hDib = NULL;
		DeleteDC(m_hMemDC);
		m_hMemDC = NULL;
		return false;
	}

	// Store old DC and RC
	m_hOldDC = ::wglGetCurrentDC();
	m_hOldRC = ::wglGetCurrentContext();

	// Make m_hMemRC the current RC.
	::wglMakeCurrent(m_hMemDC, m_hMemRC);

	return true;
}


int CBitmapHLRenderer::render()
{
	if (!Initialize())
		return RTERROR;

	if (SetOpenGLState() == RTERROR || CreateDisplayList(1) == RTERROR) {
		Cleanup();
		return RTERROR;
	}
	return RenderSense();
}


int CBitmapHLRenderer::RenderSense()
{
	::wglMakeCurrent(NULL, NULL);
	::wglDeleteContext(m_hMemRC);

	// Restore last DC and RC
	::wglMakeCurrent(m_hOldDC, m_hOldRC);

	if(m_pPrintInfo)
	{// TO DO: stretch image KEEPING its aspect ratio!
		// Size of printing image
		float fBmiRatio = float(m_bmi.bmiHeader.biHeight) / m_bmi.bmiHeader.biWidth;
		CSize szImageOnPage;
		if (m_szPage.cx > m_szPage.cy) {	 // Landscape page
			if(fBmiRatio<1)	{  // Landscape image
				szImageOnPage.cx = m_szPage.cx;
				szImageOnPage.cy = long(fBmiRatio * m_szPage.cy);
			}
			else {			  // Portrait image
				szImageOnPage.cx = long(m_szPage.cy/fBmiRatio);
				szImageOnPage.cy = m_szPage.cy;
			}
		}
		else {		    // Portrait page
			if (fBmiRatio<1) {	  // Landscape image
				szImageOnPage.cx = m_szPage.cx;
				szImageOnPage.cy = long(fBmiRatio * m_szPage.cx);
			}
			else {			  // Portrait image
				szImageOnPage.cx = long(m_szPage.cx/fBmiRatio);
				szImageOnPage.cy = m_szPage.cy;
			}
		}

		CSize szOffset((m_szPage.cx - szImageOnPage.cx) / 2, (m_szPage.cy - szImageOnPage.cy) / 2);

		// Stretch the Dib to fit the image size.
		StretchDIBits(m_pDC->GetSafeHdc(),
					  szOffset.cx, szOffset.cy, szImageOnPage.cx, szImageOnPage.cy,
					  0, 0, m_bmi.bmiHeader.biWidth, m_bmi.bmiHeader.biHeight,
					  (GLubyte*) m_pBitmapBits, &m_bmi, DIB_RGB_COLORS, SRCCOPY);
	}
	else
		BitBlt(m_pDC->GetSafeHdc(), m_Bound.left, m_Bound.top, m_Bound.right - m_Bound.left, m_Bound.bottom - m_Bound.top, m_hMemDC, 0, 0, SRCCOPY);

	if (m_hOldDib)
		SelectObject(m_hMemDC, m_hOldDib);

	DeleteObject(m_hDib);
	m_hDib = NULL;
	DeleteDC(m_hMemDC);
	m_hMemDC = NULL;
	m_hOldDC = NULL;

	return RTNORM;
}


void CBitmapHLRenderer::Cleanup()
{
	if (::wglGetCurrentContext())
		::wglMakeCurrent(NULL, NULL);


	if (m_hMemRC != NULL)
	{
		::wglDeleteContext(m_hMemRC);
		m_hMemRC = NULL;
	}

	if (m_hMemDC != NULL)
	{
		DeleteDC(m_hMemDC);
		m_hMemDC = NULL;
	}

	if (m_hDib != NULL)
	{
		DeleteObject(m_hDib);
		m_hDib = NULL;
	}

	m_hOldDC = NULL;
	m_hOldRC = NULL;
	m_hOldDib = NULL;
}


////////////////////////////////////////////////////////////////////////////////////
// cmd_shade and cmd_hide

short cmd_shade(void)
{
	SDS_MultibleMode=0;
	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	if (cmd_isinpspace() == RTERROR) return RTERROR;
	if (cmd_istransparent() == RTERROR) return RTERROR;

	resbuf rb;
	//temporary error stmt below to prohibit shade in perspective mode
	if(SDS_getvar(NULL,DB_QVIEWMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
	if(rb.resval.rint&1)
	{
		cmd_ErrorPrompt(CMD_ERR_SHADEPERSP,0);
		return(RTERROR);
	}

	CIcadView*	pView = SDS_CURVIEW;
	if(SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
	if(rb.resval.rint)
		pView->m_iViewMode = CIcadView::kShaded;
	else
	{
#ifdef HIDESHADE_IN_TILEMODE_OFF_MSPACE
 #ifdef BMP_USE_HIDESHADE_STATUS
		if(pView->m_pVportTabHip && pView->m_pVportTabHip->ret_type() == DB_VIEWPORT)
			((db_viewport*)pView->m_pVportTabHip)->set_281(IC_RENDERMODE_FLAT_WITH_WIREFRAME);	// TO DO: undo/redo of the change
 #else
		pView->m_iViewMode = CIcadView::kShaded;
 #endif
#else
		cmd_ErrorPrompt(CMD_ERR_TILEMODE1,0);
		return RTERROR;
#endif
	}

	SDS_CallUserCallbackFunc(SDS_CBVIEWCHANGE,(void *)2,NULL,NULL);

	CWindowHLRenderer hlRenderer(SDS_CURVIEW);
	return hlRenderer.render();
}


short cmd_hide(void)
{
	SDS_MultibleMode=0;
	if (cmd_iswindowopen() == RTERROR) return RTERROR;
	if (cmd_isinpspace() == RTERROR) return RTERROR;
	if (cmd_istransparent() == RTERROR) return RTERROR;

	resbuf rb;
	//temporary error stmt below to prohibit hide in perspective mode
	if(SDS_getvar(NULL,DB_QVIEWMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
	if(rb.resval.rint&1)
	{
		cmd_ErrorPrompt(CMD_ERR_HIDEPERSP,0);
		return(RTERROR);
	}

	CIcadView*	pView = SDS_CURVIEW;
	if(SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM)return(RTERROR);
	if(rb.resval.rint)
		pView->m_iViewMode = CIcadView::kHiddenLine;
	else
	{
#ifdef HIDESHADE_IN_TILEMODE_OFF_MSPACE
 #ifdef BMP_USE_HIDESHADE_STATUS
		if(pView->m_pVportTabHip && pView->m_pVportTabHip->ret_type() == DB_VIEWPORT)
			((db_viewport*)pView->m_pVportTabHip)->set_281(IC_RENDERMODE_HIDDENLINE);	// TO DO: undo/redo of the change
 #else
		pView->m_iViewMode = CIcadView::kHiddenLine;
 #endif
#else
		cmd_ErrorPrompt(CMD_ERR_TILEMODE1,0);
		return(RTERROR);
#endif
	}

	SDS_CallUserCallbackFunc(SDS_CBVIEWCHANGE,(void *)3,NULL,NULL);
	
	CWindowHLRenderer hlRenderer(pView);
	return hlRenderer.render();
}
