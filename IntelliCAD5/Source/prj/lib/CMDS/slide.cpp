/* F:\DEV\PRJ\LIB\CMDS\SLIDE.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 
// THIS FILE HAS BEEN GLOBALIZED!
#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/
#include "commandqueue.h" /*DNT*/
#include "IcadView.h"
#include "DrawDevice.h"
#if defined(SDS_NEWDB)
	#define gra_displayobject gr_displayobject
	#define gra_view      gr_view
	#define dwg_entlink	  db_handitem
	#define dwg_filelink  db_drawing
	#define gra_rp2ucs    gr_rp2ucs
	#define gra_ucs2rp    gr_ucs2rp
	#define gra_rp2pix    gr_rp2pix
	#define gra_pix2rp    gr_pix2rp
	#define gra_twist4ucs gr_twist4ucs
	#define gra_initview  gr_initview
#endif

// Globals
char *cmd_SlideSavePath;

extern int SDS_UseExportSelSet;
extern bool SDS_DontBitBlt;

// PROTO
HMETAFILE SDS_ReadWMF(char* pszFileName);


#define CMD_ACADSLIDE       "AutoCAD Slide"/*DNT*/   // Needed to identify the top of an AutoCAD Slide.  Do not "translate" to "Icad".
#define CMD_NEWCOLOR        '\xFF'/*DNT*/
#define CMD_NEWVECTOR       '\xFA'/*DNT*/
#define CMD_F7              '\xF7'/*DNT*/
#define CMD_OFFSETVECTOR    '\xFB'/*DNT*/
#define CMD_ENDOFFILE       '\xFC'/*DNT*/
#define CMD_SOLIDFILL       '\xFD'/*DNT*/
#define CMD_COMMONENDVECTOR '\xFE'/*DNT*/

short cmd_mslide(void)   { 

    if (cmd_iswindowopen() == RTERROR) return RTERROR;			

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    char                      fs1[IC_ACADBUF];
    struct resbuf             rb;

    if(SDS_getvar(NULL,DB_QFILEDIA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);

    if( rb.resval.rint && 
		GetActiveCommandQueue()->IsEmpty() && 
		GetMenuQueue()->IsEmpty() && 
		!lsp_cmdhasmore) {
        dia: ;
        if(RTNORM!=sds_getfiled(ResourceString(IDC_SLIDE_CREATE_SNAPSHOT_0, "Create Snapshot" ) ,cmd_SlideSavePath,ResourceString(IDC_SLIDE_SLIDE_FILE_SLD_ENH_1, "Slide File|sld,Enhanced Metafile|emf,Windows Metafile|wmf" ) ,5,&rb)) return(RTCAN);
        if(rb.restype==RTSTR) {
            strcpy(fs1,rb.resval.rstring); IC_FREE(rb.resval.rstring);
        }
		else { goto typeit; } // EBATECH(CNBR) 2002/4/30 Enable TYPE button
    } else {
		typeit: ;
        if (GetActiveCommandQueue()->IsNotEmpty()) 
			{
			SDS_UseExportSelSet=TRUE;
			}
        if(RTNORM!=sds_getstring(1,ResourceString(IDC_SLIDE__NSNAPSHOT_TO_CREA_2, "\nSnapshot to create: " ) ,fs1)) return(RTCAN);
        if(*fs1=='~') goto dia;
    }

    if(!*fs1) return(RTNONE);

	ChangeSlashToBackslashAndRemovePipe(fs1);

	if(strrchr(fs1,'\\')) {
		db_astrncpy(&cmd_SlideSavePath,fs1,strlen(fs1)+1);
		*(strrchr(cmd_SlideSavePath,'\\')+1)=0;
	}

	if(NULL==strrchr((strrchr(fs1,'\\'/*DNT*/) ? strrchr(fs1,'\\'/*DNT*/)+1 : fs1) ,'.'/*DNT*/)) 
		ic_setext(fs1,"sld"/*DNT*/);

	return(cmd_WriteSldofMf(fs1));
} 


short cmd_WriteSldofMf(char *fname)   { 
    if (cmd_iswindowopen() == RTERROR) return RTERROR;			

    FILE                     *fptr=NULL;
    struct resbuf             rb;
    dwg_filelink             *flp;
	dwg_entlink              *elp;
    struct gra_displayobject *tdo1;
	struct gra_view          *view;
    sds_point                 pt,
                              pts,
                              ptc,
                              ptx,
                              ptpx,
                              ptsqr[4]={{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}},
                              sldpt[5],
                              lstpt;
    sds_real                  aspect;
    long                      lvalue[1];
    short                     svalue[1],
                              face,
                              ndims,
                              fi1,
                              fi2,
                              fi3,
                              pcolor=-1;
    int                       topix[2],
                              svpix[2],
                              ptpix[2],
                              lstpix[2]={0},
                              offpix[4],
                              sldpix[10];
    BOOL                      endvpt=FALSE,
                              chkflg,
                              pt2flg;
    short                     type,
                              rev,
                              reppt;
    RT                        ret;
//    HDC                       hDCmeta,
//							  hDC;
//    CIcadView                *pView;
//    RECT                      rect;
//	CDC						 *cDC;
    char                     //*tmp_fname,
//                             *fcp1,
                              fs1[IC_ACADBUF];
  
//    int                       iWidthMM,
                              //iHeightMM,
//                              iWidthPels,
//                              iHeightPels;

    SDS_getvar(NULL,DB_QBKGREDRAW,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES); ret=(short)rb.resval.rint;

    if(!ic_chkext(fname, "wmf"/*DNT*/)) 
	{
		ICADWMFINFO wmfInfo;

        sds_real      width;
        struct resbuf rb1,
                      rb2,
                      rb3;

		memset(&wmfInfo,0,sizeof(ICADWMFINFO));

        // Get the system information.
        SDS_getvar(NULL,DB_QSCREENSIZE,&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
        SDS_getvar(NULL,DB_QVIEWSIZE,&rb2,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
        SDS_getvar(NULL,DB_QVIEWCTR,&rb3,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

        width=rb1.resval.rpoint[0]*(rb2.resval.rreal/rb1.resval.rpoint[1]);
        
        ptsqr[0][0]=rb3.resval.rpoint[0]-(width/2);
        ptsqr[0][1]=rb3.resval.rpoint[1]+(rb2.resval.rreal/2);
        ptsqr[1][0]=rb3.resval.rpoint[0]+(width/2);
        ptsqr[1][1]=rb3.resval.rpoint[1]-(rb2.resval.rreal/2);

        if ((ret=sds_pmtssget(NULL,"C"/*DNT*/,ptsqr[0],ptsqr[1],NULL,SDS_EditPropsSS,0))==RTNORM) {
			ExecuteUIAction( ICAD_WNDACTION_CREATEHWMF, 0L );
			ICADWMFINFO* pWmfInfo = (ICADWMFINFO*)SDS_CMainWindow->m_pvWndAction;
			SDS_CMainWindow->m_pvWndAction=NULL;
			if(NULL!=pWmfInfo)
			{
				SDS_WriteWMF(pWmfInfo,fname);
				::DeleteMetaFile(pWmfInfo->hMetaFile);
				delete pWmfInfo;
			}
            sds_ssfree(SDS_EditPropsSS);
        }
    }
	else
		if(!ic_chkext(fname, "emf"/*DNT*/))
		{
        sds_real      width;
        struct resbuf rb1,
                      rb2,
                      rb3;

        // Get the system information.
        SDS_getvar(NULL,DB_QSCREENSIZE,&rb1,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
        SDS_getvar(NULL,DB_QVIEWSIZE,&rb2,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
        SDS_getvar(NULL,DB_QVIEWCTR,&rb3,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);

        width=rb1.resval.rpoint[0]*(rb2.resval.rreal/rb1.resval.rpoint[1]);
        
        ptsqr[0][0]=rb3.resval.rpoint[0]-(width/2);
        ptsqr[0][1]=rb3.resval.rpoint[1]+(rb2.resval.rreal/2);
        ptsqr[1][0]=rb3.resval.rpoint[0]+(width/2);
        ptsqr[1][1]=rb3.resval.rpoint[1]-(rb2.resval.rreal/2);

        if ((ret=sds_pmtssget(NULL,"C"/*DNT*/,ptsqr[0],ptsqr[1],NULL,SDS_EditPropsSS,0))==RTNORM) {
			ExecuteUIAction( ICAD_WNDACTION_CREATEHEMF, 0L );
		    HENHMETAFILE hMetaFile = (HENHMETAFILE)SDS_CMainWindow->m_pvWndAction;
		    SDS_CMainWindow->m_pvWndAction=NULL;
		    if(NULL!=hMetaFile)
		    {
			    HENHMETAFILE hCopyMetaFile = ::CopyEnhMetaFile(hMetaFile,fname);
			    ::DeleteEnhMetaFile(hMetaFile);
			    ::DeleteEnhMetaFile(hCopyMetaFile);
		    }
            sds_ssfree(SDS_EditPropsSS);
        }
		} 
		else 
		{
 			if((fptr = fopen(fname,"w+b"/*DNT*/)) == NULL)
				goto err;

			// Get a pointer to the entity linklist.
			flp=SDS_CURDWG;
			view=SDS_CURGRVW;

			// Get the system information.
			SDS_getvar(NULL,DB_QSCREENSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			ic_ptcpy(pts,rb.resval.rpoint);
			SDS_getvar(NULL,DB_QVIEWSIZE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			pt[1]=rb.resval.rreal;
			SDS_getvar(NULL,DB_QVIEWCTR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
			ic_ptcpy(ptc,rb.resval.rpoint);

			// Set up the screen dimenstions as points.
			gra_pix2rp(view,0,0,&ptsqr[0][0],&ptsqr[0][1]);
			gra_pix2rp(view,(int)pts[0],0,&ptsqr[1][0],&ptsqr[1][1]);
			gra_pix2rp(view,(int)pts[0],(int)pts[1],&ptsqr[2][0],&ptsqr[2][1]);
			gra_pix2rp(view,0,(int)pts[1],&ptsqr[3][0],&ptsqr[3][1]);

			// Calcualte the aspect ratio.
			aspect=(pts[0]/pts[1])*10000000;

			// Calculate the width in drawing units.
			pt[0]=((pt[1]/pt[1])*pts[0]);

	        // Field            Bytes Description
			// ---------------  ----- ---------------------------------------------------------------------------------------------------------------------
			// ID String         17   "AutoCAD Slide" CR LF ^Z NULL.
			// Type indicator    1    Currently set to 86 (decimal).
			// Level indicator   1    Currently set to 2.
			// High X dot        2    Width of the graphics area-1, in pixels.
			// High Y dot        2    Height of the graphics area-1, in pixels.
			// Aspect ratio      4    Aspet ratio (horizontal size/vertical size in inches) of the graphics area, scaled by 10,000,000. Least significant byte first.
			// Hardware fill     2    Either 0 or 2 (value is unimportant).
			// Test number       2    A number (1234 hex) used to determine whether all 2-byte values in this slide file were written width the high byte first
			//                        (as by Intel 8086-family CPU's) or the low byte first (as by Motorola 68000-family CPU's).
			fprintf(fptr,"%s%c%c%c%c%c%c"/*DNT*/ ,CMD_ACADSLIDE,13,10,26,0,86,2);
			svalue[0]=(short)pts[0]; fwrite(svalue,sizeof(short),1,fptr); 
			svalue[0]=(short)pts[1]; fwrite(svalue,sizeof(short),1,fptr);
			lvalue[0]=(long)aspect;  fwrite(lvalue,sizeof(long),1,fptr);
			svalue[0]=2;             fwrite(svalue,sizeof(short),1,fptr);
			svalue[0]=4660;          fwrite(svalue,sizeof(short),1,fptr);

			for(elp = flp->entnext_noxref(NULL); elp != NULL; elp = flp->entnext_noxref(elp))
			{
				for(tdo1 = (struct gra_displayobject*)elp->ret_disp(); tdo1 != NULL; tdo1 = tdo1->next)
				{
					if(tdo1->npts < 2 || tdo1->chain == NULL || tdo1->type & DISP_OBJ_SPECIAL)
						continue;
					ndims = ((tdo1->type & 0x01) !=0 ) + 2;
					face = ((tdo1->type& 0x02) !=0 );

					for(reppt = 0, type = 0,rev = 1, fi1 = 0, fi2 = (tdo1->npts + face) * ndims;
					fi1 < fi2; fi1 += ndims, ++rev, reppt = 0, type = 0)
					{
						if(face && fi1 >= fi2 - ndims) 
						{
							if(ndims > 2)
								gra_ucs2rp(&(tdo1->chain[0]), pt, view);
							else
							{
								pt[0] = tdo1->chain[0];
								pt[1] = tdo1->chain[1];
							}
							gra_rp2pix(view, pt[0], pt[1], &topix[0], &topix[1]);
						}
						else
						{
							if(ndims > 2)
								gra_ucs2rp(&(tdo1->chain[fi1]), pt, view);
							else
							{
								pt[0] = tdo1->chain[fi1];
								pt[1] = tdo1->chain[fi1 + 1];
							}
							gra_rp2pix(view, pt[0], pt[1], &topix[0], &topix[1]);
						}

						// Since the (0,0) point is the top left corner of the screen the picture will be upsidedown, so we need to turn it rightside up.
						topix[1] = (int)pts[1] - topix[1];

						// This set the color of the entity.
						if(pcolor != tdo1->color)
						{
							svalue[0] = (!tdo1->color) ? 7 : tdo1->color;
							fwrite(svalue, sizeof(char), 1, fptr);
							svalue[0] = CMD_NEWCOLOR;
							fwrite(svalue, sizeof(char), 1, fptr);
							pcolor = tdo1->color;
						}

						// Solid fill
						if((tdo1->type & 0x04) != 0)
						{
							// Get all the points.
							ic_ptcpy(sldpt[rev-1], pt);
							sldpix[2 * (rev - 1)] = topix[0];
							sldpix[2 * (rev - 1) + 1] = topix[1];
							//sldpix[fi1] = topix[0];
							//sldpix[fi1+1] = topix[1];
							if(rev < 5)
								continue; 

							// Are all the points outside of the screen area and not crossing the screen area.
							if((sldpix[0] < 0 && sldpix[2] < 0 && sldpix[4] < 0 && sldpix[6] < 0) || 
								(sldpix[0] > pts[0] && sldpix[2] > pts[0] && sldpix[4] > pts[0] && sldpix[6] > pts[0]) ||
								(sldpix[1] < 0 && sldpix[3] < 0 && sldpix[5] < 0 && sldpix[7] < 0) ||
								(sldpix[1] > pts[1] && sldpix[3] > pts[1] && sldpix[5] > pts[1] && sldpix[7] > pts[1]))
								continue;


							if ((sldpix[0] >= 0 && sldpix[2] >= 0 && sldpix[4] >= 0 && sldpix[6] >= 0) && 
								(sldpix[0] <= pts[0] && sldpix[2] <= pts[0] && sldpix[4] <= pts[0] && sldpix[6] <= pts[0]) &&
								(sldpix[1] >= 0 && sldpix[3] >= 0 && sldpix[5] >= 0 && sldpix[7] >= 0) &&
								(sldpix[1] <= pts[1] && sldpix[3] <= pts[1] && sldpix[5] <= pts[1] && sldpix[7] <= pts[1]))
							{

								// This writes (!!FEFF), (!!)=The number of vertex records,(##)=A vertex.
								// This is only done as the begining record flag.
								svalue[0]=0;                        fwrite(svalue,sizeof(char),1,fptr);
								svalue[0]=CMD_SOLIDFILL;            fwrite(svalue,sizeof(char),1,fptr);
								svalue[0]=tdo1->npts;               fwrite(svalue,sizeof(short),1,fptr);
								svalue[0]=CMD_COMMONENDVECTOR;      fwrite(svalue,sizeof(char),1,fptr);  
								svalue[0]=CMD_NEWCOLOR;             fwrite(svalue,sizeof(char),1,fptr);  
								for(fi3=0;fi3<7;fi3+=2) 
								{
									// This writes the first two bytes (FD00), This is the flag for the start of each record.
									svalue[0]=0;                             fwrite(svalue,sizeof(char),1,fptr);
									svalue[0]=CMD_SOLIDFILL;                 fwrite(svalue,sizeof(char),1,fptr);
									fwrite(sldpix+fi3,sizeof(short),1,fptr); fwrite(sldpix+fi3+1,sizeof(short),1,fptr);
								}

								// This writes (0F7FF), This is the flag for the end of the sold fill records.
								svalue[0]=0;                        fwrite(svalue,sizeof(char),1,fptr);
								svalue[0]=CMD_SOLIDFILL;            fwrite(svalue,sizeof(char),1,fptr);
								svalue[0]=0;                        fwrite(svalue,sizeof(short),1,fptr);  
								svalue[0]=CMD_F7;                   fwrite(svalue,sizeof(char),1,fptr);  
								svalue[0]=CMD_NEWCOLOR;             fwrite(svalue,sizeof(char),1,fptr);  
							}
						}
						else
						{
							// We need to check lines that are not completly inside the screen area.
							if(rev >= 2)
							{
								// Are both points outside of the screen area and not crossing the screen area.
								if((topix[0] < 0 && svpix[0] < 0) || (topix[1] < 0 && svpix[1] < 0) ||
									(topix[0] > pts[0] && svpix[0] > pts[0]) || 
									(topix[1] > pts[1] && svpix[1] > pts[1]))
								{
									ic_ptcpy(lstpt, pt);
									svpix[0] = topix[0];
									svpix[1] = topix[1];
									endvpt = FALSE;
									rev = 1;
									continue;
								}
								
								// Check if at least one point is out side of the screen area.
								// reppt = 0 both points are inside or on the screen area.
								// reppt = 1 (topix) point is outside of screen area.
								// reppt = 2 (svpix) point is outside of screen area.
								// reppt = 3 (topix and svpix) points are both outside of screen area.
								if(topix[0] < 0 || topix[1] < 0 || topix[0] > pts[0] || topix[1] > pts[1])
									reppt |= 0x01;
								if(svpix[0] < 0 || svpix[1] < 0 || svpix[0] > pts[0] || svpix[1] > pts[1])
									reppt |= 0x02;
								
								// if (reppt) was set then some point(s) were outside of the screen area.
								if(reppt)
								{
									for(pt2flg = FALSE, chkflg = TRUE, fi3 = 0; fi3 < 4 && rev; ++fi3)
									{
										// -1 = At least 1 line undefined (coincident points), or both parallel.
										//  0 = Intersection is not on either segment.
										//  1 = Intersection is on the 1st segment (p0-p1) but not the 2nd.
										//  2 = Intersection is on the 2nd segment (p2-p3) but not the 1st.
										//  3 = Intersection is on both line segments.
										ret = ic_linexline(ptsqr[fi3],ptsqr[((fi3==3)?0:fi3+1)],lstpt,pt,ptx);
										switch(ret)
										{
										case 0: 
										case 2:
											continue; 
										case 3:
											switch(reppt)
											{
											case 1:
												ptpix[0]=topix[0]; ptpix[1]=topix[1];
												gra_rp2pix(view,ptx[0],ptx[1],&topix[0],&topix[1]); topix[1]=(int)pts[1]-topix[1]; fi3=3; break;
											case 2:
												ptpix[0]=topix[0]; ptpix[1]=topix[1];
												gra_rp2pix(view,ptx[0],ptx[1],&svpix[0],&svpix[1]); svpix[1]=(int)pts[1]-svpix[1]; fi3=3; break;
											case 3:
												if(!pt2flg)
												{
													ic_ptcpy(ptpx,ptx); pt2flg=TRUE;
												} 
												else 
												{
													ptpix[0]=topix[0]; ptpix[1]=topix[1];
													if(sds_distance(ptx,pt)<sds_distance(ptpx,pt)) 
													{
														gra_rp2pix(view,ptx[0],ptx[1],&topix[0],&topix[1]);   topix[1]=(int)pts[1]-topix[1];
														gra_rp2pix(view,ptpx[0],ptpx[1],&svpix[0],&svpix[1]); svpix[1]=(int)pts[1]-svpix[1];
													}
													else
													{
														gra_rp2pix(view,ptpx[0],ptpx[1],&topix[0],&topix[1]);   topix[1]=(int)pts[1]-topix[1];
														gra_rp2pix(view,ptx[0],ptx[1],&svpix[0],&svpix[1]);     svpix[1]=(int)pts[1]-svpix[1];
													}
													fi3 = 3;
													break;
												}
												
											}
											chkflg = FALSE;
											break;
										}
									}
									if(chkflg)
									{
										svpix[0]=topix[0];
										svpix[1]=topix[1];
										ic_ptcpy(lstpt,pt);
										endvpt=FALSE;
										rev=1;
										continue;
									}
								}
								
								// Try to do a (common end point vector) if a (vector) has been done.
								if(rev > 2)
								{
									offpix[0]=topix[0]-svpix[0];
									offpix[1]=topix[1]-svpix[1];
									if((offpix[0]<=127 && offpix[0]>=-(128)) && (offpix[1]<=127 && offpix[1]>=-(128)))
										type = CMD_COMMONENDVECTOR;
									else
										type = CMD_NEWVECTOR;
								}
								else
									if(rev == 2 && endvpt)
									{
										// Try to do a (offset vector) from the last (vector).
										offpix[0]=topix[0]-lstpix[0];
										offpix[1]=topix[1]-lstpix[1];
										offpix[2]=svpix[0]-lstpix[0];
										offpix[3]=svpix[1]-lstpix[1];
										if((offpix[0] <= 127 && offpix[0]>=-(128)) && (offpix[1]<=127 && offpix[1]>=-(128)) && 
											(offpix[2]<=127 && offpix[2]>=-(128)) && (offpix[3]<=127 && offpix[3]>=-(128)))
											type=CMD_OFFSETVECTOR;
									}
							}
							
							if (lstpix[0] != topix[0] || lstpix[1] != topix[1])
							{
								// All other points.
								switch(type)
								{
                                case CMD_OFFSETVECTOR:
                                    svalue[0]=CMD_OFFSETVECTOR; 
                                    fwrite(offpix  ,sizeof(char),1,fptr); 
                                    fwrite(svalue  ,sizeof(char),1,fptr);
                                    fwrite(offpix+1,sizeof(char),1,fptr);
                                    fwrite(offpix+2,sizeof(char),1,fptr);
                                    fwrite(offpix+3,sizeof(char),1,fptr);
                                    if(reppt)
									{
										lstpix[0]=ptpix[0];
										lstpix[1]=ptpix[1];
									}
									else
									{
										lstpix[0]=topix[0];
										lstpix[1]=topix[1];
									}
                                    break;
                                case CMD_COMMONENDVECTOR:
                                    svalue[0]=CMD_COMMONENDVECTOR; 
                                    fwrite(offpix,sizeof(char),1,fptr); 
                                    fwrite(svalue,sizeof(char),1,fptr);
                                    fwrite(offpix+1,sizeof(char),1,fptr);
                                    if(reppt)
									{
										lstpix[0]=ptpix[0];
										lstpix[1]=ptpix[1];
									}
									else
									{
										lstpix[0]=topix[0];
										lstpix[1]=topix[1];
									}
                                    break;
                                case CMD_NEWVECTOR:
                                    fwrite(topix,sizeof(short),1,fptr); fwrite(topix+1,sizeof(short),1,fptr);
                                default:
                                    if(rev > 2)
									{
                                        fwrite(svpix,sizeof(short),1,fptr); fwrite(svpix+1,sizeof(short),1,fptr);
                                        if(reppt)
										{
											lstpix[0]=ptpix[0];
											lstpix[1]=ptpix[1];
										}
										else
										{
											lstpix[0]=topix[0];
											lstpix[1]=topix[1]; 
										}
									}
									else
										if(rev==2)
										{
											fwrite(topix,sizeof(short),1,fptr); fwrite(topix+1,sizeof(short),1,fptr);
											fwrite(svpix,sizeof(short),1,fptr); fwrite(svpix+1,sizeof(short),1,fptr);
											if(reppt)
											{
												lstpix[0]=ptpix[0];
												lstpix[1]=ptpix[1];
											}
											else
											{
												lstpix[0] = topix[0];
												lstpix[1] = topix[1];
											}
											endvpt=TRUE;
										}
										break;
								}
								ic_ptcpy(lstpt,pt);
								if (reppt)
								{
									svpix[0]=ptpix[0];
									svpix[1]=ptpix[1];
									rev=1;
									endvpt=FALSE;
								}
								else
								{
									svpix[0]=topix[0];
									svpix[1]=topix[1];
								}
							}
	                    }
					} // display object points
				} // display objects list
			} // entity
			// This is the end of slide file flag.
			svalue[0] = 0;
			fwrite(svalue, sizeof(char), 1, fptr);
			svalue[0] = CMD_ENDOFFILE;
			fwrite(svalue, sizeof(char), 1, fptr);
		} // slide file

	if(fptr)
		fclose(fptr);
	return(RTNORM);

	err:
	sprintf(fs1, ResourceString(IDC_SLIDE_ERROR__WRITING_SLI_3, "Error: Writing slide to file. \n%s" ), fname);
	sds_alert(fs1);

	SDS_UseExportSelSet = FALSE;
	if(fptr)
		fclose(fptr);
	return(RTERROR);
}


int cmds_drawSlideFile(char *szSlideFileName, CDrawDevice* pDrawDevice, RECT* pRect);
////////////////////////////////////////////////////////////////
//
// (WMF) files.
//
//
// The file starts with a PMETAHEADER - Placeable metafile header.
//
// Total of 22 bytes
// ----------------------------------------------------------------------
// DWORD	key			- 4 bytes		  = always equal to (0x9AC6CDD7)
// HANDLE	hmf			- 2 bytes
// RECT		rBox			- (2 bytes * 4) - 8 
// WORD		inch			- 2 bytes
// DWORD	reserved			- 4 bytes
// WORD		checksum		- 2 bytes
//
// IF there is a PMETAHEADER then after the PMETAHEADER will be a METAHEADER - metafile header.
//
// Total of 18 bytes. 
// ----------------------------------------------------------------------
// UINT		mtType			-2 bytes		= 0 - If memory 1 - if  file.
// UINT		mtHeaderSize		-2 bytes		= 0009 for standard meta file
// UINT		mtVersion		-2 bytes		= Windows version
// DWORD	mtSize			-4 bytes
// UINT		mtNoObjects		-2 bytes
// DWORD	mtMaxRecord		-4 bytes
// UINT		mtNoParameters		-2 bytes
//
// After the METAHEADER there will be the METARECORDS.
//
// Variable Number of records.
// ----------------------------------------------------------------------
// DWORD	rdSize			-4 bytes		
// UINT		rdFunction		-2 bytes
// UINT		rdParm[1]		-2 bytes

short cmd_vslide(void)   
	{

    if (cmd_iswindowopen() == RTERROR) 
		{
		return RTERROR;			
		}

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

	short rc = RTNORM;

    char                 fs1[IC_ACADBUF];
    struct resbuf        rb;

    HENHMETAFILE         hEnhMeta;
    HMETAFILE            hMeta;
    RECT                 rect;

   
    if(SDS_getvar(NULL,DB_QFILEDIA,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) return(RTERROR);

    if( rb.resval.rint && 
		GetActiveCommandQueue()->IsEmpty() && 
		GetMenuQueue()->IsEmpty()) {
        dia: ;
        if(RTNORM!=sds_getfiled(ResourceString(IDC_SLIDE_VIEW_SNAPSHOT_4, "View Snapshot" ) ,cmd_SlideSavePath,ResourceString(IDC_SLIDE_SLIDE_FILE_SLD_ENH_1, "Slide File|sld,Enhanced Metafile|emf,Windows Metafile|wmf" ) ,260,&rb)) return(RTCAN);
        if(rb.restype==RTSTR) {
            strcpy(fs1,rb.resval.rstring); IC_FREE(rb.resval.rstring);
        }
		else { goto typeit; } // EBATECH(CNBR) 2002/4/30 Enable TYPE button
    } else {
		typeit:;
        if(RTNORM!=sds_getstring(1,ResourceString(IDC_SLIDE__NSNAPSHOT_TO_VIEW_5, "\nSnapshot to view: " ) ,fs1)) return(RTCAN);
        if(*fs1=='~') goto dia;
    }

    if(!*fs1) return(RTNONE);

	ChangeSlashToBackslashAndRemovePipe(fs1);

	if(strrchr(fs1,'\\')) {
		db_astrncpy(&cmd_SlideSavePath,fs1,strlen(fs1)+1);
		*(strrchr(cmd_SlideSavePath,'\\')+1)=0;
    }


    sds_grclear();

	CIcadView *pView=SDS_CURVIEW;

    if(!ic_chkext(fs1,"wmf"/*DNT*/ ))
		{

		// *************************
		// DISPLAY METAFILE
		// *************************

        // If we try to open the metafile and no good then there might be a PMETAHEADER in it.
        if(NULL==(hMeta=GetMetaFile(fs1)))
			{
			if(NULL==(hMeta=SDS_ReadWMF(fs1)))
				{
				return RTERROR;
				}
	        }
		pView->GetClientRect(&rect);

		HDC hDC = pView->GetFrameBufferDC();

		if(!PlayMetaFile(hDC, hMeta))
			{
			rc = RTERROR;
			}
		DeleteMetaFile(hMeta);
	    } 
   else if (!ic_chkext(fs1,"emf"/*DNT*/ ))
		{

		// *************************
		// DISPLAY ENHANCED METAFILE
		// *************************

        if (!(hEnhMeta=GetEnhMetaFile(fs1))) 
			{
			return(RTERROR);
			}
        pView->GetClientRect(&rect);
		HDC hDC = pView->GetFrameBufferDC();
		if (!PlayEnhMetaFile(hDC,hEnhMeta,&rect)) 
			{
			rc = RTERROR;
			}
        DeleteEnhMetaFile(hEnhMeta);

	    } 
	else 
		{

		// *************************
		// DISPLAY AUTOCAD SLIDE FILE
		// *************************
//#define _SDS_SLIDE_
#ifdef _SDS_SLIDE_
		RECT rect;
		HDC hDC;
		if ( ( pView != NULL ) )
			{
			hDC = pView->GetFrameBufferDC();
			if ( hDC != NULL )
				{
				SelectPalette( hDC, HPALETTE( SDS_CMainWindow->m_pPalette ), TRUE );   

				// Default is whole window
				//
				pView->GetClientRect(&rect);

				struct resbuf rb;
				SDS_getvar(NULL,DB_QTILEMODE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
				int tilemode=rb.resval.rint;


				if ( (tilemode == 0) &&
					 (pView->m_pVportTabHip != NULL) )
					{
					int x1,x2,y1,y2;
					if (SDS_GetRectForVP(pView,pView->m_pVportTabHip,&x1,&x2,&y1,&y2) == RTNORM )
						{
						rect.left = x1;
						rect.top = y1;
						rect.right = x2;
						rect.bottom = y2;
						}
					}
		
				if ( !IC_DrawSlideFile( fs1, hDC, &rect ) )
					{
					rc = RTERROR;
					}
				} // end of if ( hDC != NULL )
		}
#else
			RECT rect;
			if(pView != NULL)
			{
				CDrawDevice* pDrawDevice = pView->GetDrawDevice();
				pDrawDevice->selectPalette(HPALETTE(SDS_CMainWindow->m_pPalette), TRUE);   
				pDrawDevice->SetDrawMode(RasterConstants::MODE_COPY);
				pView->GetClientRect(&rect);
					
				struct resbuf rb;
				SDS_getvar(NULL, DB_QTILEMODE, &rb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES);
				int tilemode = rb.resval.rint;
					
				if(tilemode == 0 && pView->m_pVportTabHip != NULL)
				{
					int x1,x2,y1,y2;
					if(SDS_GetRectForVP(pView, pView->m_pVportTabHip, &x1, &x2, &y1, &y2) == RTNORM)
					{
						rect.left = x1;
						rect.top = y1;
						rect.right = x2;
						rect.bottom = y2;
					}
				}
					
				if(!cmds_drawSlideFile(fs1, pDrawDevice, &rect))
					rc = RTERROR;
			}
#endif
		else
			{
			ASSERT( false ); //????
			}
		}

	SDS_DontBitBlt=FALSE;
	SDS_BitBlt2View(0,SDS_CURVIEW);
	return rc;
}



