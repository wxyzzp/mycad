/* COPY_AND_MOVE.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * Supports the copy and move commands
 *
 */
// THIS FILE HAS BEEN GLOBALIZED!
//** Includes
#include "cmds.h"/*DNT*/
#include "CloneHelper.h"
#include "Icad.h" /*DNT*/
#include "IcadApi.h"
#include "fixup1005.h"
// EBATECH(shiba)-[
#include "IcadDoc.h"
// ]-EBATECH(shiba) 

// **********
// GLOBALS
//
extern struct cmd_drag_globalpacket cmd_drag_gpak;
extern int SDS_PointDisplayMode;
extern sds_point SDS_PointDisplay;
extern bool    SDS_DontBitBlt;
extern int SDS_AtNodeDrag;

/*-------------------------------------------------------------------------*//**
This function duplicates and moves entities.  If 1 is passed to "copy", 
entities are duplicated.  If 0 is passed to "copy", entities are moved.
This function could be enhanced by doing the copy differently.  It is too slow
with a lot of entities.  Also, both copy and move leave a little piece of
dragging line behind.

@author ?
@param	copy	 => copy entities if 1, move entities if 0
@return ?
*//*--------------------------------------------------------------------------*/
static int	cmd_copy_and_move
(
short		copy
) 
{				
	// Prompting is frozen, 4/30/96
    char fs1[IC_ACADBUF];
    sds_name ss1, ssnew, ename;
    sds_point pt2;
    sds_matrix mtx;
    RT rc, ret;
	int fi1, fi2, fi3, vect = 0, mult, highlight;
	bool vectorModePossible = true;
    long ssct;
    struct resbuf setgetrb, *elist, rbucs, rbwcs;
    struct cmd_drag_globalpacket *gpak;
    // SystemMetrix(Hiro)-[FIX: the copied annotation is associated with invalid leader.
    //ReactorFixups reactorFixup;
    // ]-SystemMetrix(Hiro) FIX: the copied annotation is associated with invalid leader.

    gpak = &cmd_drag_gpak;
    gpak->reference = 0; gpak->refval = 0.0; gpak->rband = 1;
    elist = NULL; rc = mult = 0;
	rbucs.restype=rbwcs.restype = RTSHORT;
	rbucs.resval.rint = 1;
	rbwcs.resval.rint = 0;
	ssnew[0] = ssnew[1] = 0L;

	db_drawing *olddwg = SDS_CURDWG;
	CIcadView *oldview = SDS_CURVIEW;
	CIcadDoc *olddoc = SDS_CURDOC;
	
	sds_name ssCopies;
	ssCopies[0]=ssCopies[1]=0L;
    
	// Get selection set of entities to move or copy.
	if (copy) 
	{
		if (RTNORM != sds_pmtssget(ResourceString(IDC_COPY_AND_MOVE__NSELECT_E_0, "\nSelect entities to copy: "),
									NULL, NULL, NULL, NULL, ss1, 0)) 
			return (-1);

			// Send notification to sds-apps in case of COPY
			if( copy )
				SDS_CallUserCallbackFunc(SDS_CBBEGINCLONE,(void *)ss1,NULL,NULL);

		// Don't want to modify association on original entities for copy so there is no call to cmd_checkReactors.
	} 
	else 
	{
	    if (RTNORM != sds_pmtssget(ResourceString(IDC_COPY_AND_MOVE__NSELECT_E_1, "\nSelect entities to move: "),
									NULL, NULL, NULL, NULL, ss1, 0)) 
			return (-1);
	}

    // Get highlight mode.
    if (RTNORM != SDS_getvar(NULL, DB_QHIGHLIGHT, &setgetrb, SDS_CURDWG, &SDS_CURCFG, &SDS_CURSES)) 
	{ 
		rc = (-2); 
		goto bail; 
	}

    highlight = setgetrb.resval.rint;

	for( ; ; ) 
	{
		if (copy) 
		{
			// Options for Copying.
			if (mult) 
			{
				if (vect) 
				{
					sds_initget(SDS_RSG_NOLIM | SDS_RSG_OTHER,NULL);
					strcpy(fs1, ResourceString(IDC_COPY_AND_MOVE__NENTER_DI_2, "\nEnter direction vector (x,y,z): "));
				} 
				else 
				{
					sds_initget(SDS_RSG_NOLIM | SDS_RSG_OTHER, ResourceString(IDC_COPY_AND_MOVE_INITGET_VE_3, "Vector ~_Vector"));
					strcpy(fs1, ResourceString(IDC_COPY_AND_MOVE__NVECTOR___4, "\nVector/<Base point>: "));
				}
			} 
			else 
			{
				if (vect) 
				{
					sds_initget(SDS_RSG_NOLIM | SDS_RSG_OTHER, ResourceString(IDC_COPY_AND_MOVE_INITGET_MU_5, "Multiple_copies|Multiple ~_Multiple" ));
					strcpy(fs1, ResourceString(IDC_COPY_AND_MOVE__NMULTIPLE_6, "\nMultiple/<Direction vector (x,y,z)>: "));
				} 
				else 
				{
					sds_initget(SDS_RSG_NOLIM|SDS_RSG_OTHER,ResourceString(IDC_COPY_AND_MOVE_INITGET_MU_7, "Multiple_copies|Multiple Vector ~_Multiple ~_Vector"));
					strcpy(fs1, ResourceString(IDC_COPY_AND_MOVE__NMULTIPLE_8, "\nMultiple/Vector/<Base point>: "));			// First prompt for Copy
				}
			}
		} 
		else 
		{
			// Options for Moving.
			if (vect) 
			{
				sds_initget(SDS_RSG_NOLIM | SDS_RSG_OTHER,NULL);
				strcpy(fs1, ResourceString(IDC_COPY_AND_MOVE__NENTER_DI_2, "\nEnter direction vector (x,y,z): "));
				// Note - responding with a pick moves the selection that many units relative to 0
			} 
			else 
			{
				sds_initget(SDS_RSG_NOLIM | SDS_RSG_OTHER, ResourceString(IDC_COPY_AND_MOVE_INITGET_VE_3, "Vector ~_Vector"));
				strcpy(fs1, ResourceString(IDC_COPY_AND_MOVE__NVECTOR___4, "\nVector/<Base point>: "));						// First prompt for Move
			}
		}

		// Gets the base point or vector.
		ret = internalGetpoint(NULL, fs1, gpak->pt1);

		if (ret == RTERROR) 
		{
			rc = (-2); 
			goto bail;
		} 
		else if (ret == RTNONE) 
		{
			rc = (-1); 
			goto bail;
		} 
		else if (ret == RTCAN) 
		{
			rc = (-1); 
			goto bail;
		} 
		else if (ret == RTNORM) 
		{
			break;
		} 
		else if (ret == RTSTR || ret == RTKWORD) 
		{
			if (sds_getinput(fs1) != RTNORM) 
			{ 
				rc = (-2); 
				goto bail; 
			}

			if (strisame("MULTIPLE"/*DNT*/, fs1)) 
			{
				if (copy) 
				{		
					// They should never have gotten here if copy!=1 in the first place.
					mult = 1;
					continue;
				} 
				else  
					cmd_ErrorPrompt(CMD_ERR_PTKWORD,0);	// Multiple isn't allowed with Move
			} 
			else if (strisame("VECTOR"/*DNT*/, fs1)) 
			{
				vect = 1;	// Skips over getting a 2nd point
				continue;
			} 
			else 
				cmd_ErrorPrompt(CMD_ERR_PTKWORD, 0);
		} 
		else 
		{
			if (copy) 
				cmd_ErrorPrompt(CMD_ERR_PTKWORD, 0);
			else 
				cmd_ErrorPrompt(CMD_ERR_PT, 0);

			continue;
		}
	}

	if (vect)
	{
		ic_ptcpy(pt2, gpak->pt1);
		gpak->pt1[0] = gpak->pt1[1] = gpak->pt1[2] = 0.0;
		ret = RTNORM;
	}

	// Gets ending point of the displacement vector.
	// This needs to take a null response and use it to mean that the first x,y,z "point"
	// entered was really a vector - for consistency with AutoCAD.
	extern int SDS_AtCopyMoveCommand;
	SDS_AtCopyMoveCommand = copy + 1;

	for (fi3 = 0; ; fi3++)
	{
		if (fi3 != 0 || !vect)
		{	
			// skip on 1st pass if vect
			if (mult)
			{
				if (fi3)
				{
					SDS_setvar(NULL,DB_QLASTPOINT,&setgetrb,SDS_CURDWG,&SDS_CURCFG,&SDS_CURSES,0);
				}
				else
				{
					setgetrb.restype = RT3DPOINT;
					ic_ptcpy(setgetrb.resval.rpoint, gpak->pt1);
				}
			}

			SDS_PointDisplayMode=5;
			ic_ptcpy(SDS_PointDisplay, gpak->pt1);

			if (vect)
				ret = cmd_drag(CMD_TRANSLATE_DRAG, ss1, ResourceString(IDC_COPY_AND_MOVE__NENTER_DI_2, "\nEnter direction vector (x,y,z): " ), 
								NULL, pt2, fs1);
			else
				ret = cmd_drag(CMD_TRANSLATE_DRAG, ss1, ResourceString(IDC_COPY_AND_MOVE__NDISPLAC_10, "\nDisplacement point: " ),
								NULL, pt2, fs1);

			SDS_PointDisplayMode = 0;
		}

		if (ret == RTERROR) 
		{
			rc = (-2); 
			goto bail;
		} 
		else if (ret == RTCAN) 
		{
			rc = (-1); 
			goto bail;
		} 
		else if (ret == RTNORM || ret == RTNONE) 
		{
			if (ret == RTNONE)
			{
				if (!vect && fi3 == 0) 
				{
					ic_ptcpy(pt2, gpak->pt1);
					gpak->pt1[0] = gpak->pt1[1] = gpak->pt1[2] = 0.0;
					vect = 1;
				}
				else 
					break;
			}
		
			// Create transformation matrix. Just translation.
			for (fi1 = 0; fi1 <= 3; fi1++) 
			{
				for(fi2 = 0; fi2 < 3; fi2++) 
					mtx[fi1][fi2] = 0;

				if (fi1 < 3)
					mtx[fi1][3] = pt2[fi1] - gpak->pt1[fi1];
			}

			// Fill diagonal of transformation matrix.
			for (fi1 = 0; fi1 <= 3; fi1++) 
				mtx[fi1][fi1] = 1;

			if (copy || olddwg != SDS_CURDWG) 
			{
				if (olddwg != SDS_CURDWG) 
				{
					SDS_SetUndo(IC_UNDO_COMMAND_BEGIN, (void *) copy ? "COPY"/*DNT*/ : "MOVE"/*DNT*/, NULL, NULL, SDS_CURDWG);
				}

				// Set the flag to copying so new associations will not be made until copy is over.
				if (copy)
					SDS_CURDWG->SetCopyingFlag(true);

                struct resbuf filter;
                filter.restype = RTSTR; 
				filter.resval.rstring = "*"/*DNT*/; 
				filter.rbnext = NULL;

				int	curprogress = 0, tcalc;
				long ssLen;
				SDS_ProgressStart();

				if (copy)
					SDS_DontBitBlt = 1;

				sds_sslength(ss1, &ssLen);
				sds_ssfree(ssnew);

                // SystemMetrix(Hiro)-[FIX: the copied annotation is associated with invalid leader.
                ReactorFixups reactorFixup;
                // ]-SystemMetrix(Hiro) FIX: the copied annotation is associated with invalid leader.

				// Remove proxy entities from the selection set.
				long removedCount = 0;
				bool userNotified = false;
				for(ssct=0L;sds_ssname(ss1,ssct,ename)==RTNORM;ssct++) 
				{
					if (IC_TYPE_FROM_ENAME(ename) == DB_ACAD_PROXY_ENTITY) 
					{
						if (userNotified == false)
						{
							userNotified = true;
							sds_printf(ResourceString(CMD_ERR_DWGPROXY, "Proxy entities can not be edited." ));
						}
						removedCount++;
						sds_redraw(ename,IC_REDRAW_UNHILITE);
						sds_ssdel(ename, ss1);
					}
				}
				if (removedCount)
				{
					sprintf(fs1, "%s%d", ResourceString(IDC_ICADSELECTDIA__ENTITIES__8, " Entities removed="), removedCount);
					sds_printf(fs1);
				}

				for (sds_ssadd(NULL, NULL, ssnew), ssct = 0; sds_ssname(ss1, ssct, ename) == RTNORM; ssct++) 
				{
					elist = sds_entgetx(ename,&filter);

					// Deal with complex entities.
					sds_name savname;
					ic_encpy(savname,ename);

					// If we have a polyline or an insert with attributes, process everything BUT the seqend here.
					if (ic_assoc(elist,66) == 0 && 
						ic_rbassoc->resval.rint == 1 && 
						ic_assoc(elist,0) == 0 && 
							(strisame("INSERT"/*DNT*/, ic_rbassoc->resval.rstring) || 
							strisame("POLYLINE"/*DNT*/, ic_rbassoc->resval.rstring))) 
					{
						while (!strisame("SEQEND"/*DNT*/, ic_rbassoc->resval.rstring)) 
						{
							SDS_simplexforment(NULL, elist, mtx);

							if (olddwg!=SDS_CURDWG) 
							{
								if (cmd_ItemInsert(olddwg, SDS_CURDWG, NULL, elist) == FALSE) 
								{ 
									rc = (-2); 
									goto bail; 
								}
							} 
							else 
							{
								if (sds_entmake(elist) != RTNORM) 
								{ 
									rc = (-2); 
									goto bail; 
								}
							}

							IC_RELRB(elist);

							if (sds_entnext_noxref(ename, ename) != RTNORM) 
							{ 
								rc = (-2); 
								goto bail; 
							}

							elist = sds_entgetx(ename, &filter);
							ic_assoc(elist, 0);
						}
					}

					// delete the original entity, be it simple or complex, if moving between drawing files
					if (!copy && olddwg != SDS_CURDWG) 
					{
						// EBATECH(shiba)-[ 
						//	FIX :When an entity of document with two or more views is moved to 
						//		 view of another document, garbage (afterimage of a figure)
						//		 is displayed on view before movement.
//						SDS_RedrawEntity(savname, -IC_REDRAW_UNDRAW, oldview, olddoc, 1);
						CIcadView *pView;
						CIcadDoc  *pDoc=olddoc;

						POSITION pos=pDoc->GetFirstViewPosition();				
						while (pos!=NULL)
						{						
							pView=(CIcadView *)pDoc->GetNextView(pos);						
							if(pView == NULL)
								break;			
							SDS_RedrawEntity(savname, -IC_REDRAW_UNDRAW, pView, olddoc, 1);
						}
						// ]-EBATECH(shiba) 
						sds_entdel(savname);
					}

					// for complex entities, elist now holds only the seqend, so insert it in the target drawing
					// for simple entities, elist is THE entity -- insert it also.
					SDS_simplexforment(NULL, elist, mtx);

					if (olddwg != SDS_CURDWG) 
					{
						SDS_DontBitBlt = 0;

						if (cmd_ItemInsert(olddwg, SDS_CURDWG, NULL, elist) == FALSE) 
						{ 
							rc = (-2); 
							goto bail; 
						}
					} 
					else 
					{
                        if (IC_TYPE_FROM_ENAME(elist->resval.rlname) == DB_IMAGE) 
						{
                            SDS_entmakeImageCopy(elist);
                        } 
						else 
						{
                            // This is for dimensions.
                            SDS_AtNodeDrag = 1;

							if (sds_entmake(elist) != RTNORM) 
							{ 
								rc = (-2); 
								goto bail; 
							}

                            // This is for dimensions.
                            SDS_AtNodeDrag = 0;
                        }
					}

					IC_RELRB(elist);

					// Setup the table so the reactors can be fixed.
					if (copy)
					{
						sds_name copiedEnt;
						sds_entlast(copiedEnt);
						sds_ssadd( copiedEnt, ssCopies, ssCopies );
						db_handitem *oldEntity = (db_handitem*) ename[0];

						// old entity may be a seqend so need to get the polyline
						if (oldEntity->ret_type() == DB_SEQEND)
							((db_seqend *) oldEntity)->get_mainent(&oldEntity);

						db_handitem *newEntity = (db_handitem*) copiedEnt[0];

						reactorFixup.AddHanditem(newEntity);
						reactorFixup.AddHandleMap(oldEntity->RetHandle(), newEntity->RetHandle());
					}

					tcalc=(int) (((double) ssct / ssLen) * 100);

					if (curprogress + 5 <= tcalc) 
					{
						curprogress = tcalc;
						SDS_ProgressPercent(curprogress);
					}
				}

				// Set the flag to copying so new associations will not be made until copy is over.
				if (copy)
					SDS_CURDWG->SetCopyingFlag(false);

                // SystemMetrix(Hiro)-[FIX: the copied annotation is associated with invalid leader.
                if (copy)
                  {
                    // fix the reactors on the copied entities
                    reactorFixup.ApplyFixups(SDS_CURDWG);
                  }
                // ]-SystemMetrix(Hiro) FIX: the copied annotation is associated with invalid leader.
			}

			if (copy) 
			{
				// Send notification to apps
				SDS_CallUserCallbackFunc(SDS_CBENDCLONE,(void *)ss1,ssCopies,(void*)0);
				sds_ssfree( ssCopies );
				if (!mult)
					break;
			} 
			else 
			{
				// Ugly trick here!!  SDS_progxformss will cause a setting of an undo, if the current
				// drawing has changed we DON'T want this undo in the undo list.  because it will be
				// an undo that could be for a drawing that could have been closed.  So to full the
				// SDS_SetUndo function we will set the SDS_CurrentlyUndoing flag.  Yuck!!!

				// extern int SDS_CurrentlyUndoing;

				if (olddwg == SDS_CURDWG) 
				{
					// SDS_CurrentlyUndoing=TRUE;
				 
					if (SDS_progxformss((copy) ? ssnew : ss1, mtx, 1) != RTNORM) 
					{
						// if(olddwg!=SDS_CURDWG) SDS_CurrentlyUndoing=FALSE;

						rc = (-2); 
						goto bail;
					}
				}

				// if(olddwg!=SDS_CURDWG) SDS_CurrentlyUndoing=FALSE;

				break;
			}
		} 
		else 
		{
			cmd_ErrorPrompt(CMD_ERR_PT, 0);
			continue;
		}
	}

	if (copy)
	{
		// fix the reactors on the copied entities
        // SystemMetrix(Hiro)-[FIX: the copied annotation is associated with invalid leader.
        //reactorFixup.ApplyFixups(SDS_CURDWG);
        // ]-SystemMetrix(Hiro) FIX: the copied annotation is associated with invalid leader.
//		SDS_CallUserCallbackFunc(SDS_CBENDCLONE,(void *)ss1,ss1,(void*)0);
	}
	else
	{
		// break association appropriately if moving one entity without the reactor
		// or update the reactors if have both.
		cmd_checkReactors (ss1);
	}

bail:
	if(copy) SDS_BitBlt2Scr(1);
	SDS_ProgressStop();
	SDS_DontBitBlt=0;
	SDS_AtCopyMoveCommand=0;

	sds_entmake(NULL);

	if((rc<0 || copy || olddwg!=SDS_CURDWG) && highlight) {
		if(olddwg==SDS_CURDWG)
			SDS_DontBitBlt=1;

	    //*** Unhighlight selection set of entities if entities did not get moved.
		for(ssct=0L; sds_ssname(ss1,ssct,ename)==RTNORM; ssct++) {
			if(olddwg==SDS_CURDWG) {
				sds_redraw(ename,IC_REDRAW_UNHILITE);
				if(((ssct+1)%2000)==0) SDS_BitBlt2Scr(1);
			} else {
				// Special case here.  We need to bypass sds_redraw() because
				// we may have copyied from one drawing to another.
				SDS_RedrawEntity(ename,IC_REDRAW_UNHILITE,oldview,olddoc,1);
			}
		}
	}

	// After entities are unhighlighted, redraw everything if moving so entities underneath are redrawn
	if ((!copy) && (olddwg == SDS_CURDWG))
	{
		SDS_DontBitBlt=1;
		sds_redraw(NULL,IC_REDRAW_DRAW);
		if(((ssct+1)%2000)==0) SDS_BitBlt2Scr(1);
	}

	if(olddwg==SDS_CURDWG) {
		SDS_BitBlt2Scr(1);
		SDS_DontBitBlt=0;
	}

	if(olddwg!=SDS_CURDWG) {
		SDS_SetUndo(IC_UNDO_COMMAND_END,(void *)copy ? "COPY"/*DNT*/ : "MOVE"/*DNT*/,NULL,NULL,SDS_CURDWG);
	}

	sds_ssfree(ss1); sds_ssfree(ssnew);
	sds_ssfree( ssCopies );
	return(rc);

}


short cmd_copy(void) {
    if (cmd_iswindowopen() == RTERROR) return RTERROR;
	
	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    return(RT(cmd_copy_and_move(1)));
}

short cmd_move(void) {
    if (cmd_iswindowopen() == RTERROR) return RTERROR;

	//BugZilla No. 78050; 25-03-2002	
	if (cmd_istransparent() == RTERROR) return RTERROR;

    return(RT(cmd_copy_and_move(0)));
}


