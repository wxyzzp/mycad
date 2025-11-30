#include "cmds.h"
#include "cmdsViewport.h"
#include "IcadDoc.h"
#include "IcadView.h"
#include "vxtabrec.h"

db_handitem* cmd_createVIEWPORT(db_drawing* pDrawing, sds_point pt1, sds_point pt2)
{
	struct resbuf rb;
#ifdef _DEBUG
	SDS_getvar(NULL, DB_QTILEMODE, &rb, pDrawing, &SDS_CURCFG, &SDS_CURSES);
	ASSERT(rb.resval.rint == 0);
#endif
	double vsiz[2];
	sds_point pt3,pt4,pt5;
	int status = 0;
	
	if(pt1 && pt2) 
	{
		gr_ucs2rp((double *)pt1, pt3, SDS_CURGRVW);
		gr_ucs2rp((double *)pt2, pt4, SDS_CURGRVW);
		vsiz[0] = max(pt3[0], pt4[0]) - min(pt3[0], pt4[0]);
		vsiz[1] = max(pt3[1], pt4[1]) - min(pt3[1], pt4[1]);
	} 
	else 
	{
		vsiz[0] = vsiz[1] = 0.0;
	}
	
	// NOTE: Don't set the value of the second parameter to 0 because 
	// VX table can contain the deleted records(marked as deleted) after a viewport
	// creation is UNDOne, Setting the second parameter to 0 will cause those records
	// to be counted. SWH, 30/9/2004
	int	id = pDrawing->ret_ntabrecs(DB_VXTAB,1) + 1;
	// most of these ints are used for VPORT only, not DB_VIEWPORT ent
	int czoom,viewmode,fzoom,ucsicon,snapmode,gridmode,snapstyle,snapisopair;
	sds_point cp,target,viewctr,gridunit,snapbase,viewdir,snapunit;
	double snapang,lenslength,viewtwist,frontz,backz,viewsize;
	struct sds_resbuf *elist;
	
	// If this is the first viewport use extmin and extmax
	// NOTE: Don't set the value of the second parameter to 0 because 
	// VX table can contain the deleted records(marked as deleted) after a viewport
	// creation is UNDOne, Setting the second parameter to 0 will cause those records
	// to be counted. SWH, 30/9/2004
	if(pDrawing->ret_ntabrecs(DB_VXTAB,1) == 0) 
	{
		status=IC_VPENT_STATUS_UCSICON | 32768;  // adsk doc says 32768 "currently always enabled"
		target[0]=target[1]=target[2]=0.0;
		viewdir[0]=viewdir[1]=0.0; viewdir[2]=1.0;
		frontz=backz=viewtwist=0.0;
		lenslength=50.0;
		viewsize=12.0;
		viewctr[0]=6.0;
		viewctr[1]=4.5;
		viewctr[2]=0.0;
		czoom=100;
		snapang=0.0;
		snapbase[0]=0.0;
		snapbase[1]=0.0;
		snapbase[2]=0.0;
		snapunit[0]=0.5;
		snapunit[1]=0.5;
		snapunit[2]=0.0;
		gridunit[0]=0.5;
		gridunit[1]=0.5;
		gridunit[2]=0.0;
	} 
	else 
	{
		// We have to set the tilemode ON for these get the MS extents and limits.
		rb.restype=RTSHORT;
		rb.resval.rint=1;
		SDS_setvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		sds_point pt1,pt2;
		SDS_getvar(NULL,pDrawing->ret_nmainents()>pDrawing->ret_ntabrecs(DB_VXTAB,1) ? DB_QEXTMIN : DB_QLIMMIN,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(pt1,rb.resval.rpoint);
		SDS_getvar(NULL,pDrawing->ret_nmainents()>pDrawing->ret_ntabrecs(DB_VXTAB,1) ? DB_QEXTMAX : DB_QLIMMAX,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
		ic_ptcpy(pt2,rb.resval.rpoint);
		rb.restype=RTSHORT;
		rb.resval.rint=0;
		SDS_setvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
		
		//			pt1[0]=min(pt1[0],pt2[0]);
		//			pt1[1]=min(pt1[1],pt2[1]);
		//			pt2[0]=max(pt1[0],pt2[0]);
		//			pt2[1]=max(pt1[1],pt2[1]);
		
		for (db_handitem *hip=pDrawing->tblnext(DB_VPORTTABREC,1); hip!=NULL; hip=hip->next) 
		{
			if(hip->ret_deleted()) 
				continue;
			char *fcp1;
			hip->get_2(&fcp1);
			if(strisame(fcp1,"*ACTIVE"/*DNT*/)) 
				break;
		}
		
		if(hip) 
		{
			struct gr_view *view=NULL;
			
			if(SDS_CURVIEW) 
			{
				struct gr_viewvars viewvarsp;
				db_handitem *savhip=SDS_CURVIEW->m_pVportTabHip;
				SDS_CURVIEW->m_pVportTabHip=hip;
				SDS_ViewToVars2Use(SDS_CURVIEW,pDrawing,&viewvarsp);
				gr_initview(pDrawing,&SDS_CURCFG,&view,&viewvarsp);
				SDS_CURVIEW->m_pVportTabHip=savhip;
			}
			else 
			{
				gr_initview(pDrawing,&SDS_CURCFG,&view,NULL);
			}
			
			SDS_rpextents(pt1, pt2, pt3, pt4, view);
			//pt3 & pt4 now represent screen coordinates
			//of dwg's extents.  Convert center of view
			//back to UCS
			pt5[0]=(pt3[0]+pt4[0])/2.0;
			pt5[1]=(pt3[1]+pt4[1])/2.0;
			pt5[2]=0.0;
			//				gr_rp2ucs(pt5,viewctr,view);
			ic_ptcpy(viewctr,pt5);
			
			/*
			// Get center point of the main view ps viewport entity.
			double mainsize;
			sds_point maincen;
			struct resbuf *apps,*eed,*pRb;
			apps=sds_buildlist(RTSTR,"ACAD"DNT,0);
			eed=((db_vxtabrec *)pDrawing->tblnext(DB_VXTAB,1))->ret_vpehip()->get_eed(apps,NULL);
			sds_relrb(apps);
			for(pRb=eed;pRb!=NULL && pRb->restype!=1040;pRb=pRb->rbnext);
			pRb=pRb->rbnext;
			mainsize=pRb->resval.rreal;
			pRb=pRb->rbnext;
			maincen[0]=pRb->resval.rreal;
			pRb=pRb->rbnext;
			maincen[1]=pRb->resval.rreal;
			sds_relrb(eed);
			
			  double height=(pt1==NULL || pt2==NULL) ? 15.64 : max(pt1[0],pt2[0])-min(pt1[0],pt2[0]);
			  
				// Adjust this VP center for its placement in the PS viewport.
				pt5[0]=pt5[0]+(viewsize/height)*(maincen[0]-cp[0]);
				pt5[1]=pt5[1]+(viewsize/height)*(maincen[1]-cp[1]);
			*/
			
			
			gr_freeviewll(view);
			view = NULL;
			
			viewsize = pt4[1]-pt3[1];
			if(vsiz[0] && vsiz[1] && (icadRealEqual(pt2[1],pt1[1]) || ((pt2[0]-pt1[0]) / (pt2[1]-pt1[1]))>(vsiz[0]/vsiz[1]))) 
			{
				viewsize = ((pt2[0]-pt1[0]) * vsiz[1]) / vsiz[0];
			}
			viewsize *= 1.02;
			
			// Get all these values from the last active VP.
			hip->get_17(target);
			hip->get_16(viewdir);
			hip->get_51(&viewtwist);
			hip->get_42(&lenslength);
			hip->get_43(&frontz);
			hip->get_44(&backz);
			hip->get_71(&viewmode);
			hip->get_72(&czoom);
			hip->get_73(&fzoom);
			hip->get_74(&ucsicon);
			hip->get_75(&snapmode);
			hip->get_76(&gridmode);
			hip->get_77(&snapstyle);
			hip->get_78(&snapisopair);
			hip->get_50(&snapang);
			hip->get_13(snapbase);
			hip->get_14(snapunit);
			hip->get_15(gridunit);
		} 
		else 
		{
			viewsize=pt2[1]-pt1[1];
			
			if(vsiz[0] && vsiz[1] && (icadRealEqual(pt2[1],pt1[1]) || ((pt2[0]-pt1[0])/(pt2[1]-pt1[1]))>(vsiz[0]/vsiz[1]))) 
			{
				viewsize = ((pt2[0]-pt1[0])*vsiz[1])/vsiz[0];
			}
			
			viewctr[0]=(pt1[0]+pt2[0])/2.0;
			viewctr[1]=(pt1[1]+pt2[1])/2.0;
			viewctr[2]=0.0;
			
			SDS_getvar(NULL,DB_QTARGET,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
			ic_ptcpy(target,rb.resval.rpoint);
			SDS_getvar(NULL,DB_QVIEWDIR,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
			ic_ptcpy(viewdir,rb.resval.rpoint);
			SDS_getvar(NULL,DB_QVIEWTWIST,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
			viewtwist=rb.resval.rreal;
			SDS_getvar(NULL,DB_QLENSLENGTH,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
			lenslength=rb.resval.rreal;
			SDS_getvar(NULL,DB_QFRONTZ,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
			frontz=rb.resval.rreal;
			SDS_getvar(NULL,DB_QBACKZ,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
			backz=rb.resval.rreal;
			SDS_getvar(NULL,DB_QVIEWMODE,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
			viewmode=rb.resval.rint;
			SDS_getvar(NULL,DB_QZOOMPERCENT,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
			czoom=rb.resval.rint;
			SDS_getvar(NULL,DB_QFASTZOOM,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
			fzoom=rb.resval.rint;
			SDS_getvar(NULL,DB_QUCSICON,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
			ucsicon=rb.resval.rint;
			SDS_getvar(NULL,DB_QSNAPMODE,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
			snapmode=rb.resval.rint;
			SDS_getvar(NULL,DB_QGRIDMODE,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
			gridmode=rb.resval.rint;
			SDS_getvar(NULL,DB_QSNAPSTYL,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
			snapstyle=rb.resval.rint;
			SDS_getvar(NULL,DB_QSNAPISOPAIR,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
			snapisopair=rb.resval.rint;
			SDS_getvar(NULL,DB_QSNAPANG,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
			snapang=rb.resval.rreal;
			SDS_getvar(NULL,DB_QSNAPBASE,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
			ic_ptcpy(snapbase,rb.resval.rpoint);
			SDS_getvar(NULL,DB_QSNAPUNIT,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
			ic_ptcpy(snapunit,rb.resval.rpoint);
			SDS_getvar(NULL,DB_QGRIDUNIT,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
			ic_ptcpy(gridunit,rb.resval.rpoint);
		}
		}
		
		if(pt1 == NULL || pt2 == NULL) 
		{
			cp[0]=7.8; 
			cp[1]=4.5; 
			cp[2]=0.0;
		} 
		else 
		{
			struct resbuf rbucs,rbwcs;
			rbucs.restype=rbwcs.restype=RTSHORT;
			rbucs.resval.rint=1;
			rbwcs.resval.rint=0;
			cp[0]=(pt1[0]+pt2[0])/2.0;
			cp[1]=(pt1[1]+pt2[1])/2.0;
			cp[2]=0.0;
			sds_trans(cp,&rbucs,&rbwcs,1,cp);
		}
		
		sds_regapp("ACAD"/*DNT*/);
		
		SDS_getvar(NULL,DB_QMAXACTVP,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
		if(id>rb.resval.rint) 
		{
			cmd_ErrorPrompt(CMD_ERR_MAXACTVP,0);
			return(NULL);
		}
		
		if(pt1) 
			gr_ucs2rp((double *)pt1,pt1,SDS_CURGRVW);
		if(pt2) 
			gr_ucs2rp((double *)pt2,pt2,SDS_CURGRVW);
		
		elist=sds_buildlist(RTDXF0,"VIEWPORT"/*DNT*/,10,cp,69,id,68,id,70,1,
			40,(pt1==NULL || pt2==NULL) ? 15.64 : max(pt1[0],pt2[0])-min(pt1[0],pt2[0]),
			41,(pt1==NULL || pt2==NULL) ?  9.00 : max(pt1[1],pt2[1])-min(pt1[1],pt2[1]),
			17,target,16,viewdir,51,viewtwist,45,viewsize,12,viewctr,
			42,lenslength,43,frontz,44,backz,90,status,
			72,czoom,50,snapang,13,snapbase,14,snapunit,15,gridunit,0);
		
		
		
		// **************
		// Now add layers that are frozen by default
		//
		// Start by finding the end of the list
		//
		struct resbuf *prbLayerData = elist;
		while( prbLayerData->rbnext != NULL )
			prbLayerData = prbLayerData->rbnext;
		
		db_layertabrec *pLayer = (db_layertabrec *)pDrawing->tblnext(DB_LAYERTABREC, 1 );
		while( pLayer != NULL )
		{
			// Add layers that are frozen by default into the EED
			//
			if ( pLayer->isVpLayerFrozenByDefault() )
			{
				// EBATECH(CNBR) -[ 2002/6/19 341 is ename.
				sds_name name;
				name[0] = (long)pLayer;
				name[1] = (long)pDrawing;
				struct resbuf *prbNewResbuf = sds_buildlist(341,name,0);
				// get the name
				//
				//char *szLayerName = NULL;
				//pLayer->get_2( &szLayerName );
				//
				// create a new 341 element
				//
				//struct resbuf *prbNewResbuf = sds_buildlist(341,szLayerName,0);
				// EBATECH(CNBR) ]-
				ASSERT( CHECKSTRUCTPTR( prbNewResbuf ) );
				
				// Insert it into the list
				//
				prbLayerData->rbnext = prbNewResbuf;
				prbLayerData = prbNewResbuf;
			}
			
			pLayer = (db_layertabrec *)pDrawing->tblnext( DB_LAYERTABREC, 0 );
		}
		// we have now added any layers into the list
		// End of adding frozen layers
		// ****************************
		
		
		// Doing this entmake will also create the DB_VXTAB record we need
		//
		SDS_entmake(elist,pDrawing);
		sds_relrb(elist);
		
		// Because of the entmake above, we are assured that the last table record
		// in the DB_VXTAB is what we need
		//
		db_vxtabrec *hip=(db_vxtabrec *)pDrawing->ret_lasttabrec(DB_VXTAB,1);
		ASSERT( CHECKSTRUCTPTR( hip ) );
		ASSERT( hip->ret_type() == DB_VXTAB );
		
		db_viewport *pNewViewport = hip->ret_vpehip();
		ASSERT( CHECKSTRUCTPTR( pNewViewport ) );
		
		int iVportNumber;
		pNewViewport->get_69( &iVportNumber );
		// The paperspace viewport should be on layer 0 and linetype BYLAYER
		//
		if ( iVportNumber == 1 )
		{
			pNewViewport->set_8( "0"/*DNT*/,
				pDrawing );
			pNewViewport->set_6( "BYLAYER"/*DNT*/,
				pDrawing );
		}
		
		// Set the newly created viewport to the "current" one
		//
		if(iVportNumber > 1)
			SDS_CURDOC->m_nLastMsVport = iVportNumber;
		
		// ************
		// RETURN GOOD!
		//
		return pNewViewport;
}

db_handitem* cmd_createVPORT(db_drawing* pDrawing)
{
	struct resbuf rb;
#ifdef _DEBUG
	SDS_getvar(NULL, DB_QTILEMODE, &rb, pDrawing, &SDS_CURCFG, &SDS_CURSES);
	ASSERT(rb.resval.rint == 1);
#endif
	// Create a VPORT table entry.
	struct resbuf *trb;
	
	char *name="$temp"/*DNT*/;
	char *actname="*ACTIVE"/*DNT*/;
	int id = pDrawing->ret_ntabrecs(DB_VPORTTAB,0)+1; /* 69 */
	unsigned char  flags = 0; 	   /* 70 */
	
	// calculate the ll and ur
	
	// Basically this dances around the fact that this function is not given the values for the ll
	// and ur points that need to be stored in the vports.	So we take the view rectangle (createrect) and
	// and compute its extents (on a 0.0 -- 1.0 scale) within the enclosing window (winrect), flipping
	// the Y coordinate.  Note that the createrect's coordinates are relative to the enclosing window.
	CRect *winrect = &SDS_CMainWindow->m_rectMDIWin;
	CRect *createrect = SDS_CMainWindow->m_rectCreateView;
	sds_real winheight = (double)winrect->bottom-(double)winrect->top;
	sds_real winwidth = (double)winrect->right-(double)winrect->left;
	
	sds_point	   ll;			   /* 10 */
	ll[0]=ll[1]=ll[2]=0.0;
	ll[0]=(double)createrect->left/winwidth;
	ll[1]=(double)(winheight-createrect->bottom)/winheight;

	// Just make sure that the new window fits within the enclosing window
	ll[0]= (ll[0]>1.0) ? 1.0 : ll[0];
	ll[1]= (ll[1]>1.0) ? 1.0 : ll[1];
	
	sds_point	   ur;			  /* 11 */
	ur[0]=ur[1]=ur[2]=1.0;
	ur[0]=(double)createrect->right/winwidth;
	ur[1]=(double)(winheight-createrect->top)/winheight;

	// Just make sure that the new window fits within the enclosing window
	ur[0]= (ur[0]>1.0) ? 1.0 : ur[0];
	ur[1]= (ur[1]>1.0) ? 1.0 : ur[1];

	sds_point	   target;		  /* 17 */
	db_qgetvar(DB_QTARGET ,pDrawing->ret_headerbuf(),rb.resval.rpoint ,DB_3DPOINT,0);
	ic_ptcpy(target,rb.resval.rpoint);
	sds_point	   targ2cam;	  /* 16 */
	db_qgetvar(DB_QVIEWDIR ,pDrawing->ret_headerbuf(),rb.resval.rpoint ,DB_3DPOINT,0);
	ic_ptcpy(targ2cam,rb.resval.rpoint);
	sds_real	   twist;		  /* 51 */
	SDS_getvar(NULL,DB_QVIEWTWIST,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
	twist=rb.resval.rreal;
	sds_real	   height;		  /* 40 */
	SDS_getvar(NULL,DB_QVIEWSIZE,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
	height=rb.resval.rreal;
	sds_point	   center;		  /* 12 */
	db_qgetvar(DB_QVIEWCTR ,pDrawing->ret_headerbuf(),rb.resval.rpoint ,DB_3DPOINT,0);
	ic_ptcpy(center,rb.resval.rpoint);
	sds_real	   asp; 	   /* 41 */
	SDS_getvar(NULL,DB_QVIEWASPECT,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
	asp=rb.resval.rreal;
	sds_real	   lenslen;    /* 42 */
	SDS_getvar(NULL,DB_QLENSLENGTH,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
	lenslen=rb.resval.rreal;
	sds_real	   fclip;	   /* 43 */
	SDS_getvar(NULL,DB_QFRONTZ,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
	fclip=rb.resval.rreal;
	sds_real	   bclip;	   /* 44 */
	SDS_getvar(NULL,DB_QBACKZ,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
	bclip=rb.resval.rreal;
	short		   viewmd;	   /* 71 */
	SDS_getvar(NULL,DB_QVIEWMODE,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
	viewmd=rb.resval.rint;
	short		   czoom;	   /* 72 */
	SDS_getvar(NULL,DB_QZOOMPERCENT,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
	czoom=rb.resval.rint;
	short		   fzoom;	   /* 73 */
	SDS_getvar(NULL,DB_QFASTZOOM,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
	fzoom=rb.resval.rint;
	short		   ucsicon;    /* 74 */
	SDS_getvar(NULL,DB_QUCSICON,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
	ucsicon=rb.resval.rint;
	short		   snap;	   /* 75 */
	SDS_getvar(NULL,DB_QSNAPMODE,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
	snap=rb.resval.rint;
	short		   grid;	   /* 76 */
	SDS_getvar(NULL,DB_QGRIDMODE,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
	grid=rb.resval.rint;
	short		   snapstyle;  /* 77 */
	SDS_getvar(NULL,DB_QSNAPSTYL,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
	snapstyle=rb.resval.rint;
	short		   isopair;    /* 78 */
	SDS_getvar(NULL,DB_QSNAPISOPAIR,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
	isopair=rb.resval.rint;
	sds_real	   snaprot;    /* 50 */
	SDS_getvar(NULL,DB_QSNAPANG,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
	snaprot=rb.resval.rreal;
	sds_point	   snapbase;   /* 13 */
	SDS_getvar(NULL,DB_QSNAPBASE,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(snapbase,rb.resval.rpoint);
	sds_point	   snapsp;	   /* 14 */
	SDS_getvar(NULL,DB_QSNAPUNIT,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(snapsp,rb.resval.rpoint);
	sds_point	   gridsp;	   /* 15 */
	SDS_getvar(NULL,DB_QGRIDUNIT,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES);
	ic_ptcpy(gridsp,rb.resval.rpoint);
	
	if(SDS_CMainWindow->m_rectCreateView && cmd_InsideViewPorts && !cmd_DontCalcViewSize &&
		SDS_CMainWindow->m_rectCreateView->bottom-SDS_CMainWindow->m_rectCreateView->top>
		SDS_CMainWindow->m_rectCreateView->right-SDS_CMainWindow->m_rectCreateView->left) 
	{
		height*= (double)(SDS_CMainWindow->m_rectCreateView->bottom-SDS_CMainWindow->m_rectCreateView->top)/
			(double)(SDS_CMainWindow->m_rectCreateView->right-SDS_CMainWindow->m_rectCreateView->left);
		height*=1.5;
	}
	
	trb=sds_buildlist(RTDXF0,"VPORT"/*DNT*/,
					   70,flags,2,name,10,ll,11,ur,17,target,
					   16,targ2cam,51,twist,40,height,12,center,41,asp,
					   42,lenslen,43,fclip,44,bclip,69,id,71,viewmd,
					   72,czoom,73,fzoom,74,ucsicon,75,snap,76,grid,
					   77,snapstyle,78,isopair,50,snaprot,13,snapbase,
					   14,snapsp,15,gridsp,0);
	
	if(RTNORM != SDS_tblmesser(trb,IC_TBLMESSER_MAKE,pDrawing))
		id = -1;
	
	db_handitem *hip;
	if(id != -1) 
	{
		// Set the name *ACTIVE into the viewport because dwg_tblmesser() will not.
		hip=pDrawing->ret_lasttabrec(DB_VPORTTAB,1);
		hip->set_2(actname);
		rb.restype=RTSHORT;
		rb.resval.rint=id;
		SDS_setvar(NULL,DB_QCVPORT,&rb,pDrawing,&SDS_CURCFG,&SDS_CURSES,0);
	}
	sds_relrb(trb);
	return(hip);
}
