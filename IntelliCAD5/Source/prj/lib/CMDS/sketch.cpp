/* G:\ICADDEV\PRJ\LIB\CMDS\SKETCH.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 
// THIS FILE HAS BEEN GLOBALIZED!
//** Includes
#include "cmds.h"/*DNT*/
#include "Icad.h"/*DNT*/
#include "IcadApi.h"
#include "inputevent.h" /*DNT*/
#include "icadgridiconsnap.h"

short cmd_sketch(void)        		// This is called "Freehand" in the program.
	{
	if (cmd_iswindowopen() == RTERROR) 
		return RTERROR;			//  Can't do this if there's no drawing open.		

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    int iInputType=1,iPoly;										//	Type of user input - from which device?
	RT	iReturn;
	BOOL bConnectMode=FALSE,bSraight2Cur=FALSE,bEraseMode=FALSE,bDoPrompt=FALSE;								//	Return values
    char fs1[IC_ACADBUF], fs2[IC_ACADBUF];						//  Strings for use in prompts
    BOOL bSketching=FALSE;										//	FALSE=Paused (not sketching), TRUE=Sketching
    sds_point pt1,LastDragPoint,pt210;
    sds_real fr1=1.0,sketchlen=1.0;								//  Segment length (either from default or getdist)
    struct resbuf rb;											//  Contains SysVar info from SDS_getvar
	struct resbuf *rbNewEnt=NULL; 
	struct resbuf *rbNewVert=NULL;	// EBATECH(CNBR) Freehand Sketch with LWPOLYLINE
	struct resbuf *rbt; 
	struct resbuf *rb10; 
	struct resbuf *rb11;
    struct resbuf rbUserInput;									//	Contains Type and Value of user input
    struct resbuf rbucs,rbent;
	int plinetype;

    struct strPoint 
		{
        struct resbuf *rbb,*rbc;    
        struct strPoint *next;
		}
	*pstrStartPoint,*pstrCurrentPoint,*pstrTempPoint;

    SDS_getvar(NULL,DB_QSKPOLY,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    iPoly=rb.resval.rint;
    SDS_getvar(NULL,DB_QPLINETYPE,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    plinetype=rb.resval.rint;

    SDS_getvar(NULL,DB_QUCSXDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
    ic_ptcpy(pt210,rb.resval.rpoint);
	SDS_getvar(NULL,DB_QUCSYDIR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
	ic_crossproduct(pt210,rb.resval.rpoint,pt210);
        
	//get the current ucs
	rbucs.restype=RTSHORT;
	rbucs.resval.rint=1;
    if (iPoly) 
		{
        rbent.restype=RT3DPOINT;
        ic_ptcpy(rbent.resval.rpoint,pt210);
		}
	else
		{
        rbent.restype=RTSHORT;
        rbent.resval.rint=0;
		}

    pstrStartPoint=pstrCurrentPoint=NULL;
    if (SDS_getvar(NULL,DB_QSKETCHINC,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)!=RTNORM) 	// Get default segment length.
		return(RTERROR);
	rb.resval.rreal=fabs(rb.resval.rreal);											// Insure the increment is positive.
    sds_rtos(rb.resval.rreal,-1,-1,fs2);											// Convert segment length to String.
    sprintf(fs1,ResourceString(IDC_SKETCH__NFREEHAND___LENG_0, "\nFreehand:  Length of segments <%s>: " ),fs2);						// Now it's the default in the prompt.
    if ((iReturn=(sds_getdist(NULL,fs1,&fr1)))==RTCAN) return(iReturn);				// Get a new length, Enter, or Cancel
    if (iReturn==RTNORM) 
		{
        sketchlen=rb.resval.rreal=fr1;
        if (SDS_setvar(NULL,DB_QSKETCHINC,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0)!=RTNORM)
			return(RTERROR);				// Set new sketching Increment value
		sketchlen=rb.resval.rreal=fabs(rb.resval.rreal);							// Insure the increment is positive.
		}
	else if (iReturn==RTNONE)
        sketchlen=rb.resval.rreal;		// It's already abs, from getting set at first above.

	if (sds_initget(0,"`"/*DNT*/)!=RTNORM) 
		return(RTERROR);
	SDS_PromptMenu(1);

    sds_printf(ResourceString(IDC_SKETCH__NPRESS_ENTER_TO__1, "\nPress ENTER to end/Pen down/(Begin...)" ));

	// Martin Waardenburg, 05-06-2000: TODO: 
	// This needs to change to the standard way of command option processing.
	// I have no idea why it has been done this way.
	CString PenUp = ResourceString(IDC_SKETCH_PENUP, "PenUp");
	CString Write = ResourceString(IDC_SKETCH_WRITE, "Write");
	CString Register = ResourceString(IDC_SKETCH_REGISTER, "Register");
	CString Continue = ResourceString(IDC_SKETCH_CONTINUE, "Continue");
	CString Straight = ResourceString(IDC_SKETCH_STRAIGHT, "Straight");
	CString Erase = ResourceString(IDC_SKETCH_ERASE, "Erase");
	CString Delete = ResourceString(IDC_SKETCH_DELETE, "Delete");
	CString Exit = ResourceString(IDC_SKETCH_EXIT, "Exit");
	CString Quit = ResourceString(IDC_SKETCH_QUIT, "Quit");

	SDS_SetCursor(IDC_ICAD_PENCIL);
	while(RTNORM==(iReturn=sds_grread((1+2+8),&iInputType,&rbUserInput))) {			// Get input from user.
        if (iInputType>=40)			// I'm not seeing when this would be >= 40 ?
       	    {
			if (rbUserInput.restype==RTSTR) 
				{
				InputEvent theEvent( SDS_EVM_POPUPCOMMAND, rbUserInput.resval.rstring );
                SDS_ProcessInputEvent( &theEvent );
	            }
            iReturn=RTNORM;
            goto out;
			}
		int key;
		switch(iInputType) 
			{
			case 11:	// Right mouse click
                   iReturn=RTNORM;
                   goto out;		
                   break;
		    case 2:		// Keyboard Input.  (Character code)
          	    if (rbUserInput.restype==RTSHORT) 
					{
					bDoPrompt=1;
					key = sds_toupper(rbUserInput.resval.rint);
					if (key == *PenUp || key == 'P'/*DNT*/)	// EBATECH
						{
                            bSketching=(!bSketching);
							bEraseMode=FALSE;
							bConnectMode=FALSE;
							if (bSketching)
								{
								pstrTempPoint= new struct strPoint;
								memset(pstrTempPoint,0,sizeof(struct strPoint));
								if (pstrStartPoint) pstrCurrentPoint=pstrCurrentPoint->next=pstrTempPoint;
								else     pstrStartPoint=pstrCurrentPoint=pstrTempPoint;
								break;
								}
							break;
						}
					else if ((key == *Write || key == 'W'/*DNT*/) ||	// EBATECH
						(key == *Register) || key == 'R'/*DNT*/)		// EBATECH
						{
                            bSketching=FALSE;		// Stops sketching (pen up)
                            bEraseMode=FALSE;	
                            bConnectMode=FALSE;	
                            iReturn=RT3DPOINT;		// This hits the "if (iReturn==RT3DPOINT)" below. Has nothing to do with 3D points.
                            goto record1;			// Skips the "if (iReturn==RTNORM)" below
                            record2: ;				// Comes back for more after saving what was already done.
                            break;
						}
					else if (key == *Continue || key == 'C'/*DNT*/)		// EBATECH
						{
							if (pstrStartPoint==NULL) 
								{
								sds_printf(ResourceString(IDC_SKETCH__NNO_FREEHAND_TO_11, "\nNo freehand to continue from.\n" ));
								break;
								}
							sds_printf(ResourceString(IDC_SKETCH__NMOVE_CURSOR_TO_12, "\nMove cursor to end of last freehand segment.\n" ));
							bSketching=TRUE;
							bConnectMode=TRUE;
							bEraseMode=FALSE;
                            break;
						}
					else if (key == *Straight || key == 'S'/*DNT*/ || key == '.'/*DNT*/)	// EBATECH
						{
			               ic_ptcpy(rbUserInput.resval.rpoint,LastDragPoint);											// Turns current pt into new "prev pt"
						bSraight2Cur=TRUE;
						bSketching=TRUE;
						bEraseMode=FALSE;
						goto StraightSeg;
                        break;
						}
					else if ((key == *Erase || key == 'E'/*DNT*/) ||	// EBATECH
						(key == *Delete || key == 'D'/*DNT*/))			// EBATECH
						{
						sds_printf(ResourceString(IDC_SKETCH__NMOVE_CURSOR_TO_13, "\nMove cursor to ends of freehand to delete.\n" ));
						bEraseMode=(!bEraseMode);
						bSketching=FALSE;
						if (bEraseMode && pstrCurrentPoint && pstrCurrentPoint->rbb) 
							{
							pstrTempPoint= new struct strPoint;
							memset(pstrTempPoint,0,sizeof(struct strPoint));
							if (pstrStartPoint) pstrCurrentPoint=pstrCurrentPoint->next=pstrTempPoint;
							else     pstrStartPoint=pstrCurrentPoint=pstrTempPoint;
							break;
							}
						break;
						}
					else if (key == *Exit || key == 'X'/*DNT*/|| key == 13)		// EBATECH
						{
						iReturn=RTNORM;
						goto out;		
						break;
						}
					else if (key == *Quit || key == 'Q'/*DNT*/)		// EBATECH
						{
						sds_printf(ResourceString(IDC_SKETCH__NQUITTING_FREEH_14, "\nQuitting Freehand without saving..." ));		//  Take this line out - just for testing.
						iReturn=RTNONE;		// This avoids the "if (iReturn==RTNORM)" section below that saves the sketch.
						goto out;	
						break;
						}
					}
                break;
            case 3:			// The user selected a point.  (3D point)
				bDoPrompt=1;
				bSketching=(!bSketching);
				bEraseMode=FALSE;
				bConnectMode=FALSE;
				if (bSketching)
					{
					ic_ptcpy(pt1,rbUserInput.resval.rpoint);
					SDS_SnapPt(pt1,pt1);			// If Snap is on, takes the point, and returns the point, but snapped to where it should be.
					pstrTempPoint= new struct strPoint ;
					memset(pstrTempPoint,0,sizeof(struct strPoint));
					if (pstrStartPoint)
						pstrCurrentPoint=pstrCurrentPoint->next=pstrTempPoint;
					else
						pstrStartPoint=pstrCurrentPoint=pstrTempPoint;
					pstrCurrentPoint->rbb=pstrCurrentPoint->rbc=sds_buildlist(RT3DPOINT,pt1,0);
					break;
					}
            case 5:			// Returns drag-mode coordinates. (3D point)
				StraightSeg: ;
                ic_ptcpy(LastDragPoint,rbUserInput.resval.rpoint);											// Turns current pt into new "prev pt"
                if (bEraseMode)																// If we are sketching...
					{
					for (pstrTempPoint=pstrStartPoint; pstrTempPoint!=NULL; pstrTempPoint=pstrTempPoint->next) 
						{
						if (pstrTempPoint->rbb==NULL || pstrTempPoint->rbb->rbnext==NULL)
							continue;
						if (sds_distance(pstrTempPoint->rbb->resval.rpoint,rbUserInput.resval.rpoint)<=sketchlen) 
							{
							sds_grdraw(pstrTempPoint->rbb->resval.rpoint,pstrTempPoint->rbb->rbnext->resval.rpoint,-1,0);									
							rbt=pstrTempPoint->rbb->rbnext;
							pstrTempPoint->rbb->rbnext=NULL;
							sds_relrb(pstrTempPoint->rbb);
							pstrTempPoint->rbb=rbt;
							ic_ptcpy(pt1,pstrTempPoint->rbb->resval.rpoint);
							goto Loopout;
						}
						for (rbt=pstrTempPoint->rbb; rbt!=NULL && rbt->rbnext!=NULL && rbt->rbnext->rbnext!=NULL; rbt=rbt->rbnext);
						if (rbt==NULL || rbt->rbnext==NULL) 
							goto Loopout;
						if (sds_distance(rbUserInput.resval.rpoint,rbt->rbnext->resval.rpoint)<=sketchlen) 
							{
							sds_grdraw(rbt->resval.rpoint,rbt->rbnext->resval.rpoint,-1,0);									
							sds_relrb(rbt->rbnext);
							rbt->rbnext=NULL;
							pstrTempPoint->rbc=rbt;
							ic_ptcpy(pt1,rbt->resval.rpoint);
							goto Loopout;
							}
						}
					break;
					}
                if (bSketching) 																// If we are sketching...
                    {
					if (!pstrCurrentPoint->rbb)													// ...and if there is no structure of points yet
                        {
						pstrCurrentPoint->rbb=pstrCurrentPoint->rbc=sds_buildlist(RT3DPOINT,rbUserInput.resval.rpoint,0);			// Builds 2 LL of points
                        ic_ptcpy(pt1,rbUserInput.resval.rpoint);											// Turns current pt into new "prev pt"
 						SDS_SnapPt(pt1,pt1);		// If Snap is on, takes the point, and returns the point, but snapped to where it should be.
                        break;
						}
                    if (bConnectMode && sds_distance(pt1,rbUserInput.resval.rpoint)>=sketchlen) 
						break;
					bConnectMode=0;
					if (sds_distance(pt1,rbUserInput.resval.rpoint)>=sketchlen || bSraight2Cur)				// ...and if prev pt to here > min dist
                        {
						pstrCurrentPoint->rbc=pstrCurrentPoint->rbc->rbnext=sds_buildlist(RT3DPOINT,rbUserInput.resval.rpoint,0);	// Builds 2 LL of points
                        // That other CAD package uses green for the temporary lines that
                        // grdraw uses during sketch. We do too now...
                        sds_grdraw(pt1,rbUserInput.resval.rpoint,3,0);										// Draws line from prev pt to current pt
                        ic_ptcpy(pt1,rbUserInput.resval.rpoint);											// Turns current pt into new "prev pt"
						}
					if (bSraight2Cur)
						bSraight2Cur=FALSE;	
	                }
                break;
	        }
		Loopout: ;		
		if (bDoPrompt) 
			{
			bDoPrompt=FALSE;
			if (pstrStartPoint) 
				{
				if (bEraseMode) 
					{
					SDS_SetCursor(IDC_ICAD_ERASER);
					if (sds_initget(SDS_RSG_NODOCCHG,ResourceString(IDC_SKETCH_INITGET_DELETE_O_15, "Delete_off|D Write,_then_resume|W ~ ` ~_D ~_W ~_" ))!=RTNORM) 
                        return(RTERROR);
					SDS_PromptMenu(1);
					sds_printf(ResourceString(IDC_SKETCH__RPRESS_ENTER_TO_16, "\rPress ENTER to end/Pen %s/Quit/Delete off/Write to drawing/(Deleting...): " ),bSketching ?ResourceString(IDC_SKETCH_UP_17, "up" ):ResourceString(IDC_SKETCH_DOWN_18, "down" ));
					}
				else 
					{
					SDS_SetCursor(IDC_ICAD_PENCIL);
 				    if (sds_initget(SDS_RSG_NODOCCHG,ResourceString(IDC_SKETCH_INITGET_DELETE_O_19, "Delete_on|D Connect_to_end|C Write,_then_resume|W ~ ` ~_D ~_C ~_W ~_" ))!=RTNORM) 
                        return(RTERROR);
					SDS_PromptMenu(1);
					sds_printf(ResourceString(IDC_SKETCH__RPRESS_ENTER_TO_20, "\rPress ENTER to end/Pen %s/Quit/Delete on/Connect/Straight to cursor/ Write to drawing/%s: " ) ,bSketching ? ResourceString(IDC_SKETCH_UP_17, "up" ) :ResourceString(IDC_SKETCH_DOWN_18, "down" ),bSketching ?  ResourceString(IDC_SKETCH__SKETCHING_____21, "(Sketching...)" ) : ResourceString(IDC_SKETCH__PAUSED_____22, "(Paused...)" ) );
					}
				}
			else 
				{
				SDS_SetCursor(IDC_ICAD_PENCIL);
				if (sds_initget(SDS_RSG_NODOCCHG,"`"/*DNT*/)!=RTNORM)
					return(RTERROR);
				SDS_PromptMenu(1);
				sds_printf(ResourceString(IDC_SKETCH__RPRESS_ENTER_TO_23, "\rPress ENTER to end/Pen %s/Quit/%s: " ),bSketching ? ResourceString(IDC_SKETCH_UP_17, "up" ):ResourceString(IDC_SKETCH_DOWN_18, "down" ) ,bSketching ? ResourceString(IDC_SKETCH__SKETCHING_____21, "(Sketching...)" ) :ResourceString(IDC_SKETCH__PAUSED_____22, "(Paused...)" ));
				}
			}
		}

    out: ;

	SDS_PromptMenu(0);
	sds_initget(0,NULL);
	SDS_SetCursor(IDC_ICAD_CROSS);

    if (iReturn==RTNORM) 
		{

        record1: ;

        sds_printf(ResourceString(IDC_SKETCH__NWRITING_FREEHA_24, "\nWriting freehand sketch to drawing..." ));

        SDS_getvar(NULL,DB_QELEVATION,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES);
        pt1[0]=pt1[1]=0.0;pt1[2]=rb.resval.rreal;
		sds_trans(pt1,&rbucs,&rbent,0,pt1);
        pt1[0]=pt1[1]=0.0;

		//get skpoly again
        for (pstrTempPoint=pstrStartPoint; pstrTempPoint!=NULL; pstrTempPoint=pstrTempPoint->next) 
			{
            if (pstrTempPoint->rbb) 
				{
                if (iPoly) 
					{
					if (plinetype==0)
						{
						FreeResbufIfNotNull(&rbNewEnt);
						rbNewEnt=sds_buildlist(RTDXF0,"POLYLINE"/*DNT*/,66,1,70,0,10,pt1,210,pt210,0);
						if (sds_entmake(rbNewEnt)!=RTNORM) 
							{ 
							FreeResbufIfNotNull(&rbNewEnt);
							return(RTERROR);
							}
						FreeResbufIfNotNull(&rbNewEnt);
						sds_trans(pstrTempPoint->rbb->resval.rpoint,&rbucs,&rbent,0,pstrTempPoint->rbb->resval.rpoint);
						rbNewEnt=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,pstrTempPoint->rbb->resval.rpoint,40,0.0,41,0.0,0);
						if (sds_entmake(rbNewEnt)!=RTNORM) 
							{ 
							FreeResbufIfNotNull(&rbNewEnt);
							return(RTERROR);
							}
						}
					else	 // start lwpline
						{
						FreeResbufIfNotNull(&rbNewEnt);
						rbNewEnt=sds_buildlist(RTDXF0,"LWPOLYLINE"/*DNT*/,210,pt210,90,0,70,0,0);
						for(rbNewVert=rbNewEnt; rbNewVert->rbnext != NULL ; rbNewVert=rbNewVert->rbnext ); // EBATECH(CNBR)
						sds_trans(pstrTempPoint->rbb->resval.rpoint,&rbucs,&rbent,0,pstrTempPoint->rbb->resval.rpoint);
//						LwplrbAddVertFromData(&rbNewEnt, pstrTempPoint->rbb->resval.rpoint,0.0,0.0,0.0);
						LwplrbAddVertFromData(&rbNewVert, pstrTempPoint->rbb->resval.rpoint,0.0,0.0,0.0); // EBATECH(CNBR)
						}
					}
				else
					ic_ptcpy(pt1,pstrTempPoint->rbb->resval.rpoint);
                for (rbt=pstrTempPoint->rbb->rbnext; rbt!=NULL; rbt=rbt->rbnext) 
					{
                    if (iPoly) 
						{
						if (plinetype==0)
							{
							if (rbt==pstrTempPoint->rbb->rbnext)
								{
								FreeResbufIfNotNull(&rbNewEnt);
								rbNewEnt=sds_buildlist(RTDXF0,"VERTEX"/*DNT*/,10,rbt->resval.rpoint,40,0.0,41,0.0,0);
								ic_assoc(rbNewEnt,10);
								rb10=ic_rbassoc;
								}
							else
								ic_ptcpy(rb10->resval.rpoint,rbt->resval.rpoint);
							sds_trans(rb10->resval.rpoint,&rbucs,&rbent,0,rb10->resval.rpoint);
							if (sds_entmake(rbNewEnt)!=RTNORM) 
								{ 
								FreeResbufIfNotNull(&rbNewEnt);
								return(RTERROR);
								}
							}
						else	 // add vert to lwpline
							{
							sds_point temppt;
							sds_trans(rbt->resval.rpoint,&rbucs,&rbent,0,temppt);
//							LwplrbAddVertFromData(&rbNewEnt,temppt,0.0,0.0,0.0);
							LwplrbAddVertFromData(&rbNewVert,temppt,0.0,0.0,0.0); // EBATECH(CNBR)
							}
						}
					else 
						{
						if (rbt==pstrTempPoint->rbb->rbnext)
							{
							FreeResbufIfNotNull(&rbNewEnt);
							rbNewEnt=sds_buildlist(RTDXF0,db_hitype2str(DB_LINE),10,pt1,11,rbt->resval.rpoint,210,pt210,0);
							ic_assoc(rbNewEnt,10);
							rb10=ic_rbassoc;
							ic_assoc(rbNewEnt,11);
							rb11=ic_rbassoc;
							}
						else
							{
							ic_ptcpy(rb10->resval.rpoint,pt1);
							ic_ptcpy(rb11->resval.rpoint,rbt->resval.rpoint);
							}
                        sds_trans(rb10->resval.rpoint,&rbucs,&rbent,0,rb10->resval.rpoint);
                        sds_trans(rb11->resval.rpoint,&rbucs,&rbent,0,rb11->resval.rpoint);
                        if (sds_entmake(rbNewEnt)!=RTNORM) 
							{
							FreeResbufIfNotNull(&rbNewEnt);
                            return(RTERROR);
	                        }
                        ic_ptcpy(pt1,rbt->resval.rpoint);
		                }
			        }
                if (iPoly) 
					{
					if (plinetype==0)
						{
						FreeResbufIfNotNull(&rbNewEnt);
						rbNewEnt=sds_buildlist(RTDXF0,"SEQEND"/*DNT*/,0);
						if (sds_entmake(rbNewEnt)!=RTNORM) 
							{
							FreeResbufIfNotNull(&rbNewEnt);
							return(RTERROR);
							}
						FreeResbufIfNotNull(&rbNewEnt);
						}
					else // make lwpline
						{
						LwplrbSetNumVerts(rbNewEnt);
						if (sds_entmake(rbNewEnt)!=RTNORM) 
							{
							FreeResbufIfNotNull(&rbNewEnt);
							return(RTERROR);
							}
						FreeResbufIfNotNull(&rbNewEnt);
						}
					}
				else
					FreeResbufIfNotNull(&rbNewEnt);
	            }
		    }
    
        if (iReturn==RT3DPOINT)
			{
			// user wanted to write what they have so far to disk.
			// need to free llist we have so far & start over...
			for (pstrCurrentPoint=pstrStartPoint; pstrCurrentPoint!=NULL;) 
				{
				pstrTempPoint=pstrCurrentPoint;
				pstrCurrentPoint=pstrCurrentPoint->next;
				IC_RELRB(pstrTempPoint->rbb);
				delete pstrTempPoint;
				}
			pstrStartPoint=pstrCurrentPoint=pstrTempPoint=NULL;
			goto record2;					//  It's up above under "case 'R': "
			}
		}
	else 
		{
		// Undraw the grdraw lines.
        if (SDS_getvar(NULL,DB_QBKGCOLOR,&rb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES)==RTNORM)
            {
            // Redraw the segments in the current background color. With a bit of trickery
            // for white background.
            int color = rb.resval.rint = 7 ? 256 : rb.resval.rint;
            for (pstrTempPoint=pstrStartPoint; pstrTempPoint!=NULL; pstrTempPoint=pstrTempPoint->next) 
                for (rbt=pstrTempPoint->rbb; rbt!=NULL && rbt->rbnext!=NULL; rbt=rbt->rbnext)
	                sds_grdraw(rbt->resval.rpoint,rbt->rbnext->resval.rpoint,color,0);									
            }
        else
            {
            // Fail safe, in case we can't resolve the color.
            sds_redraw(NULL, 4); // Just in case.
            }
		}

    // Free up the linked lists.
    for (pstrCurrentPoint=pstrStartPoint; pstrCurrentPoint!=NULL;) 
		{
        pstrTempPoint=pstrCurrentPoint;
        pstrCurrentPoint=pstrCurrentPoint->next;
        IC_RELRB(pstrTempPoint->rbb);
        delete pstrTempPoint;
		}
	pstrStartPoint=pstrCurrentPoint=pstrTempPoint=NULL;

    return(iReturn); 
	}



